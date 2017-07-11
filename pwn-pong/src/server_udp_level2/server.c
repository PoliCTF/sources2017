#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

int RECV_PORT;
uv_loop_t *loop;
uv_udp_t recv_socket;
struct sockaddr_in si_me;
int i=0;
char ip[20];


#define SECONDS_TO_START 30
#define SECONDS_TO_PLAY 10
/*SENDER VARIABLES*/
#define SEND_IP "127.0.0.1"
char send_ip_dyn[20];
int dyn=0;
struct sockaddr_in si_other;
int s,slen;
/*END*/

/*GAME VARIABLES*/
#define DELAY 20
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 60
#define BALL_DIMENSION 10
#define GAP 5

#define MAX_ENTITIES 20
#define MAX_RELOAD_TIME 20

#define PLAYER_SPEED 10 
#define BALL_X_MIN_SPEED 3
#define BALL_X_MAX_SPEED 10
#define BALL_Y_MAX_SPEED 7
int p1y,p2y,ballx,bally,end,winner;
int ball_x_speed = BALL_X_MIN_SPEED;
int ball_y_speed = 0;
#define startPy 210
#define startBx 315
#define startBy 235
#define startEnd 0
#define startWinner 0
//////
int ai_offset=0;
int new_ai_offset=0;
int started=0;
/*END*/

/*TIMER VARIABLES*/
uv_timer_t updateTimer;
/*END*/

void closeSender();
void start_game();
void updateGameData(int,int);
void updateGame();
void broadcast();
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
void setupListener();
int setupSocket();
int setupSender(char *sendIp,int port);
int sendMessage(char mex[]);
int parse(char *c,int num,int *i1,int *i2);


void start_game(){
    if(started)
        return;
    p1y=startPy;
    p2y=startPy;
    ballx=startBx;
    bally=startBy;
    ball_x_speed = BALL_X_MIN_SPEED;
    ball_y_speed = 0;
    end=startEnd;
    winner=startWinner;
    int r=uv_timer_start(&updateTimer, broadcast, 1000, DELAY);
    if (r!=0)
        perror(uv_strerror(r));
    
    //////
    started=1;
    srand(time(NULL));
    int signum=(rand() % 2)?-1:1;
    ai_offset =signum * (int)(((double)rand() / (double)(RAND_MAX))*((double)PLAYER_HEIGHT/2 + (double)BALL_DIMENSION/2));
    new_ai_offset=ai_offset;
//    fprintf(stderr,"ai_offset %d\n",ai_offset);
}

void incr(int *pt,int val){
    *pt+=val;
}

void updateGameData(int azione1,int azione2){
    if (azione1==1){
        incr(&p1y,-PLAYER_SPEED);
        if(p1y<0)
            p1y=0;
            
    }
    else
        if (azione2==1){
            incr(&p1y,+PLAYER_SPEED);
            if((p1y + PLAYER_HEIGHT)>SCREEN_HEIGHT)
                p1y=SCREEN_HEIGHT-PLAYER_HEIGHT;
        }
}

void updateGame(){
        ballx += ball_x_speed;
        bally += ball_y_speed;
        if (new_ai_offset!=ai_offset){
            int d=new_ai_offset-ai_offset;
            d = d / abs(d);
//            fprintf(stderr,"new %d, old %d, D : %d\n",new_ai_offset,ai_offset,d);
            ai_offset +=d;
        }
        p2y=bally-25+ai_offset;
        
        if ( bally < 0 || (bally + BALL_DIMENSION) > SCREEN_HEIGHT )
        {
            ball_y_speed=-ball_y_speed;
            bally += ball_y_speed * 2;   
        }
        
        if ((ballx < (SCREEN_WIDTH/2) && (bally + BALL_DIMENSION > p1y && bally < (p1y + PLAYER_HEIGHT))) || ((ballx > (SCREEN_WIDTH/2) && (bally + BALL_DIMENSION > p2y && bally < (p2y + PLAYER_HEIGHT)))))
        {
            if (ballx<GAP+PLAYER_WIDTH) 
            {
                int x0,x1,y0,y1;
                x0=ballx-ball_x_speed;
                x1=ballx;
                y0=bally-ball_y_speed;
                y1=bally;
                float m = (float) (y0-y1)/(x0-x1);
                float q = (float) -1*x1*m + y1;
                int punto_intersezione = (int) trunc((float)m*(GAP+PLAYER_WIDTH) + q);
                int centroPalla = punto_intersezione+(BALL_DIMENSION/2);
                int centroP1 = p1y+(PLAYER_HEIGHT/2);
                int diff = centroP1-centroPalla;            
                float percentuale_x = (float) (abs(diff)/ (float)(BALL_DIMENSION/2 + PLAYER_HEIGHT/2));
                percentuale_x = (float) (BALL_X_MAX_SPEED - BALL_X_MIN_SPEED) * percentuale_x;
                ball_x_speed = BALL_X_MIN_SPEED + (int) trunc(percentuale_x);
                ball_y_speed = (int)((float)(BALL_Y_MAX_SPEED)*(float)(-1*diff)/(float)(BALL_DIMENSION/2 + PLAYER_HEIGHT/2));
                ballx=GAP+PLAYER_WIDTH+1;
            }
            else
            {
                if (ballx + BALL_DIMENSION > (SCREEN_WIDTH - GAP - PLAYER_WIDTH)){
//                    ball_x_speed= - ball_x_speed;
//                    ballx=(SCREEN_WIDTH - GAP - PLAYER_WIDTH) - (ballx + BALL_DIMENSION - (SCREEN_WIDTH - GAP - PLAYER_WIDTH)) - BALL_DIMENSION;
                    //////
                    int x0,x1,y0,y1;
                    x0=ballx-ball_x_speed;
                    x1=ballx;
                    y0=bally-ball_y_speed;
                    y1=bally;
                    float m = (float) (y0-y1)/(x0-x1);
                    float q = (float) -1*x1*m + y1;
                    int punto_intersezione = (int) trunc((float)m*(SCREEN_WIDTH - GAP - PLAYER_WIDTH - BALL_DIMENSION) + q);
                    int centroPalla = punto_intersezione+(BALL_DIMENSION/2);
                    int centroP2 = p2y+(PLAYER_HEIGHT/2);
                    int diff = centroP2-centroPalla;            
                    float percentuale_x = (float) (abs(diff)/ (float)(BALL_DIMENSION/2 + PLAYER_HEIGHT/2));
                    percentuale_x = (float) (BALL_X_MAX_SPEED - BALL_X_MIN_SPEED) * percentuale_x;
                    ball_x_speed =  -1*BALL_X_MIN_SPEED + (int) -1*trunc(percentuale_x);
                    ball_y_speed = (int)((float)(BALL_Y_MAX_SPEED)*(float)(-1*diff)/(float)(BALL_DIMENSION/2 + PLAYER_HEIGHT/2));
                    ballx=SCREEN_WIDTH - GAP - PLAYER_WIDTH-BALL_DIMENSION - 1;
                    int signum=(rand() % 2)?-1:1;
                    new_ai_offset =signum * (int)(((double)rand() / (double)(RAND_MAX ))*((double)PLAYER_HEIGHT/2 + (double)BALL_DIMENSION/2));
//                    fprintf(stderr,"off : %d\n",ai_offset);
                }
            }
        }
        else
        {
            if (ballx<GAP+PLAYER_WIDTH || ballx + BALL_DIMENSION > (SCREEN_WIDTH - GAP - PLAYER_WIDTH))
            {
                end=1;
            }
        }
        
        
        if (end==1){
            if (ballx < SCREEN_WIDTH/2)
            {
                winner=2;
            }
            else if (ballx > SCREEN_WIDTH/2){        
                winner=1;
            }
            return;
        }   
}

void broadcast(){
    updateGame();
    char messaggio[40];
    snprintf(messaggio,39,"%d|%d|%d|%d|%d|%d",p1y,p2y,ballx,bally,end,winner);
    if(sendMessage(messaggio)==-1)
        fprintf(stderr,"Errore in sendMessage\n");
    if (end==1){
        uv_timer_stop(&updateTimer);
        closeSender();
        exit(0);
    }
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	buf->base = malloc(suggested_size);
	buf->len = suggested_size;
}

//
//controllare che il pacchetto in arrivo arrivi dallo stesso ip di inizio, magari anche passando quello usato nella connsessione tcp tramite param
//
//BISOGNA EVITARE LO STALLO CON PALLA ORIZZONTALE


void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
	if (nread < 0) {
		fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		uv_close((uv_handle_t*) req, NULL);
		free(buf->base);
		return;
	}
	if (nread==0){
            free(buf->base);
            return;
        }
        
    
        if(!started){
		alarm(SECONDS_TO_PLAY);
	}
        
	struct sockaddr_in* client= (struct sockaddr_in*)addr;
	
	char *ip = inet_ntoa(client->sin_addr);
        int c_port = ntohs(client->sin_port);
//        if (strncmp(ip,ip_sender,16)){
//            fprintf(stderr,"ip_dovuto %s, ip_arrivato %s\n",ip,ip_sender);
//            free(buf->base);
//            return;
//        }
        //fprintf(stderr,"%d Recv -> : '%s' from -> '%s:%d'\n",i++,buf->base,ip_sender,c_port);
        int azione1=-1;
        int azione2=-1;
        
        if (!parse(buf->base,nread,&azione1,&azione2)){
            free(buf->base);
            return;
        }
        if (azione1==-1){      
            start_game();
            if(setupSender(ip,c_port)==-1)
            {
                fprintf(stderr,"Errore in setupSender\n");
                exit(1);
            }
        }
        else
            updateGameData(azione1,azione2);
              
	free(buf->base);
}


int parse(char *c,int num,int *i1,int *i2){
    char toParse[7];
    /*for (cont = 0; cont<num; cont++){
    	toParse[cont]=c[cont];
    	fprintf(stderr,"writing %d at %p, cont : %d\n",c[cont],&(toParse[cont]),cont);
    }*/
    memcpy(toParse,c,num);
    int n=0;
    if ((n=sscanf(toParse,"%d|%d\n",i1,i2))<2){
        return 0;
    }
    else
        return 1;
}
void setupListener()
{
    uv_udp_init(loop, &recv_socket);
    uv_udp_open(&recv_socket,s);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);
}

int setupSender(char *sendIp,int port)
{
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
        if (inet_aton(sendIp, &si_other.sin_addr)==0) {
            return 0;
        }
	slen=sizeof(si_other);
	return 1;	
}

int sendMessage(char mex[])
{
	if (sendto(s, mex, strlen(mex), 0,(const struct sockaddr *) &si_other, slen)==-1)
	{
		return -1;
	}
	return 1;
}

void closeSender()
{
	close(s);
}
/*END*/

void setAlarm(){
    alarm(SECONDS_TO_START);
}

int main(int argc,char *argv[]) {
    if (argc<2)
    {
        fprintf(stderr,"Level0 -> Usage : param 1 = fd\n");
        exit(-1);
    }
    setAlarm();
    sscanf(argv[1],"%d",&s);
    //sscanf(argv[2],"%s",ip);
    int len = sizeof(si_me);
    if( getsockname(s , (struct sockaddr*)&si_me, (socklen_t *) &len) == -1)
    {
        perror("Level0 -> Error reading socket preferences");
        
        return 1;
    }
    loop = uv_default_loop();
    setupListener();
    uv_timer_init(loop, &updateTimer);
    atexit(closeSender);
    return uv_run(loop, UV_RUN_DEFAULT);
}
