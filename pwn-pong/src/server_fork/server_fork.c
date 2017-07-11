#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <arpa/inet.h>
#include <unistd.h>
int RECV_PORT;
#define DEFAULT_BACKLOG 128
#define REFRESH_RATE 100//20
#define COEFFICIENT REFRESH_RATE/10
uv_loop_t *loop;
uv_tcp_t recv_socket;
struct sockaddr_in si_me;

#define MAX_LEVEL 3
#define MIN_LEVEL 0

/*GAME VARIABLES*/
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 60
#define BALL_DIMENSION 10
#define GAP 5

#define MAX_ENTITIES 20
#define MAX_RELOAD_TIME 20

#define PLAYER_SPEED 8 

#define BALL_X_SPEED 2 * COEFFICIENT
#define BALL_Y_SPEED 1 * COEFFICIENT
int p1y,p2y,ballx,bally,end,winner;
int BULLET_X_SPEED = BALL_X_SPEED;
int BULLET_Y_SPEED = BALL_Y_SPEED;
#define startPy 210
#define startBx 315
#define startBy 235
#define startEnd 0
#define startWinner 0
/*END*/

/*TIMER VARIABLES*/
uv_timer_t updateTimer;
/*END*/

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_new_connection(uv_stream_t *server, int status);
void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
int sendMessage(uv_stream_t *client, char *messaggio);
void echo_sendMessage(uv_write_t *req, int status);
void freeHandler(uv_handle_t *c);

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	buf->base = malloc(suggested_size);
	buf->len = suggested_size;
}

void on_new_connection(uv_stream_t *server, int status) {
	if (status < 0) {
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	fprintf(stderr,"Connesso nuovo client\n");
	uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
	uv_tcp_init(loop, client);
	if (uv_accept(server, (uv_stream_t*) client) == 0) {
		uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
	}
	else {
		uv_close((uv_handle_t*) client, freeHandler);
	}
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	if (nread < 0) {
		if (nread != UV_EOF)
			fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		free(buf->base);
                uv_close((uv_handle_t*) client, freeHandler);
                return;
	}
	if (nread==0){
            free(buf->base);
            uv_close((uv_handle_t*) client, freeHandler); 
            return;
        }
	
        fprintf(stderr," - Recv -> : '%s' \n",buf->base);
        
        int level;
        
        if(sscanf(buf->base,"Lv%d",&level)<1){
            fprintf(stderr,"Error on level request packet!");
            free(buf->base);
            uv_close((uv_handle_t*) client, freeHandler); 
            return;
        }
        
        if(level<=MAX_LEVEL && level >=MIN_LEVEL){
            int s;
            if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
            {
                free(buf->base);
                uv_close((uv_handle_t*) client, freeHandler); 
                return;
            }
            struct sockaddr_in si_me;
            si_me.sin_family = AF_INET;
            si_me.sin_port = 0;
            si_me.sin_addr.s_addr = htonl(INADDR_ANY);
            if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
            {
                perror("Error binding port");
                free(buf->base);
                uv_close((uv_handle_t*) client, freeHandler); 
                return;
            }
            int len=sizeof(si_me);
            if( getsockname(s , (struct sockaddr*)&si_me, (socklen_t *) &len) == -1)
            {
                perror("Error reading socket preferences");
                free(buf->base);
                uv_close((uv_handle_t*) client, freeHandler);
                return;
            }
            fprintf(stderr,"Binded port %d\n",ntohs(si_me.sin_port));
            /*struct sockaddr name;
            int namelen=sizeof(name);
            uv_tcp_getpeername((uv_handle_t*)client, &name, &namelen);
            
            struct sockaddr_in* arrivato= (struct sockaddr_in*)client;
            char *ip;
            ip=inet_ntoa(arrivato->sin_addr);
            fprintf(stderr,"ip: %s\n",ip);*/
            int h=fork();
            if (h == 0)
            {
                sendMessage(client,"From forked child\n");
                char path[40];
                snprintf(path,39,"./level%d",level);
                fprintf(stderr,"%s",path);
                if(chdir(path)==0){
                    char levelFile[40];
                    char *arg[3];
                    arg[0]=malloc(sizeof(char)*40);
                    arg[1]=malloc(sizeof(char)*40);
                    //arg[2]=malloc(sizeof(char)*20);
                    arg[2]=NULL;
                    snprintf(levelFile,39,"./level%d",level);
                    snprintf(arg[0],39,"./level%d",level);
                    snprintf(arg[1],19,"%d",s);
                    //snprintf(arg[2],15,"%s",ip);
                    execv(levelFile, arg);
                    free(arg[0]);
                    free(arg[1]);
                    fprintf(stderr,"Forked say -> Error opening '%s'\n",levelFile);
                    perror("Execve ");
                }
                close(s);
                uv_close((uv_handle_t*) client, freeHandler); 
                exit(1);
            }
            else if (h > 0)
            {
                //parent
                char mex[20];
                sprintf(mex,"OK %d\n",ntohs(si_me.sin_port));
                sendMessage(client,mex);
                close(s);
            }
            else
            {
                perror("Fork failed");
                sendMessage(client,"NOPE\n");
            }
        }
        else{
            sendMessage(client,"NOPE\n");
        }
        
        
        free(buf->base);
        uv_close((uv_handle_t*) client, freeHandler);
}

void freeHandler(uv_handle_t *c){
    free(c);
}

int sendMessage(uv_stream_t *client, char *messaggio){
        uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
        uv_buf_t wrbuf = uv_buf_init(messaggio,strlen(messaggio));
        return uv_write(req, (uv_stream_t*) client, &wrbuf, 1, echo_sendMessage);
}

void echo_sendMessage(uv_write_t *req, int status) {
	if (status) {
		fprintf(stderr, "SendMessage error %s\n", uv_strerror(status));
	}
	free(req);
}



int main(int argc,char *argv[]) {
    if (argc<2)
    {
        printf("Usage : %s PORT\n",argv[0]);
        exit(-1);
    }
    sscanf(argv[1],"%d",&RECV_PORT);
    
    loop = uv_default_loop();
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr("0.0.0.0", RECV_PORT, &si_me);

    uv_tcp_bind(&server, (const struct sockaddr*)&si_me, 0);
    int r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}