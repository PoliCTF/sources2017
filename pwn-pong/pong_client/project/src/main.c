/* This tutorial will open a 640 x 480 screen,
** load an image and allow the user to control it
** using the arrow keys. Pressing Space will fire
** a bullet. Collision detection will allow the bullet
** to hit one of the UFO sprites on the screen.
** SDL_TTF is used to increment the player's score
** Pressing escape
** or closing the window will exit the program
*/

#include "main.h"

extern void init(char *);
extern void cleanup(void);
extern void getInput(void);
extern void draw(void);
extern void initPlayer1(void);
extern void doPlayer1(int);
extern void initPlayer2(void);
extern void doPlayer2(int);
extern void doEntities(int,int);
extern void addBullet(int, int);
extern void loadAllSprites(void);
//extern void loadAllSounds(void);
extern void addUFO(int, int);
extern void doCollisions(void);
extern TTF_Font *loadFont(char *, int);
extern void delay(unsigned int);
void sendStart();


/*SENDER METODS*/
int setupSocket(){
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
            return -1;	
    }
    return 1;
}

int setupSender(char *sendIp,int port) //ritorna -1 per errore nel bindare il socket, 0 per ip errato
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

int sendMessage(char mex[])  //1 se riesce, -1 se non riesce
{   
	//char buf[BUFLEN];

 	//strncpy(buf,mex,BUFLEN);
	if (sendto(s, mex, strlen(mex), 0,(const struct sockaddr *) &si_other, slen)==-1)
	{
		return -1;
	}
        //fprintf(stderr,"mex '%s' l '%d'\n",mex,(int)strlen(mex));
	return 1;
}

void closeSender()
{
	close(s);
}
/*END*/


void initGameLoop(){
    /* Start up SDL */
	
	init("Deer pong :P");
	
	/* Call the cleanup function when the program exits */
	
	atexit(cleanup);
	
	go = 1;
	
	/* Load all the sprites */
	
	loadAllSprites();
	
//	/* Load all the sounds */
//	
//	loadAllSounds();
	
	/* Load the font */
	
	game.font = loadFont("font/blackWolf.ttf", 16);
	
	/* Intialise the player */
	
	initPlayer1();
        
        initPlayer2();
	
        addBullet(315,235);
        uv_timer_start(&connect_timer, sendStart,10,500);
}

void sendStart(){
    numeroInvii++;
    if (numeroInvii>10){
        fprintf(stderr,"The server is not responding.\n");
        exit(0);
    }
    char messaggio[4];
    sprintf(messaggio, "%d|%d\n", -1, -1);
    if (sendMessage(messaggio) == -1) {
        fprintf(stderr, "Error sending start message\n");
        exit(1);
    }
    
}

void inputLoop()
{
        getInput();
        if(input.down1==1 || input.up1==1){
            char messaggio[4];
            sprintf(messaggio,"%d|%d\n",input.up1,input.down1);
            //fprintf(stderr,"'%d' '%s'\n",input_grezzo,messaggio);
            if(sendMessage(messaggio)==-1)
            {
                fprintf(stderr,"Errore nel sendMessage nel ciclo\n");
                exit(1);
            }
        }

}

void gameLoop(char *messaggio){
    int p1y,p2y,ballx,bally,endl,winnerl;
    
    sscanf(messaggio,"%d|%d|%d|%d|%d|%d",&p1y,&p2y,&ballx,&bally,&endl,&winnerl);
    
    
    
    doPlayer1(p1y);
    doPlayer2(p2y);

    doEntities(ballx,bally);

    end=endl;
    winner=winnerl;

    draw();

}





void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
        pacchetti++;
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
            
	//struct sockaddr_in* client= (struct sockaddr_in*)addr;
	
	//char *ip = inet_ntoa(client->sin_addr);

	//fprintf(stderr,"Recv -> : '%s' from -> '%s'\n",buf->base,ip);
        if (startTimerOn==1){
            uv_timer_stop(&connect_timer);
            startTimerOn=0;
        }
	gameLoop(buf->base);
        free(buf->base);
        
        
	//uv_udp_recv_stop(req);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	buf->base = malloc(suggested_size);
	buf->len = suggested_size;
}
void setupListener()
{
    si_me.sin_family = AF_INET;
    si_me.sin_port = 0;//SERVER_PORT //LOCAL_PORT
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        perror("Error binding port.\n");
        exit(1);
    }
    
    uv_udp_init(loop, &recv_socket);
    uv_udp_open(&recv_socket,s);
//    struct sockaddr_in recv_addr;
//    uv_ip4_addr("0.0.0.0", LOCAL_PORT, &recv_addr);
//    uv_udp_bind(&recv_socket, (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);
}
void intro(int argc,char *argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    strcpy((char *)&send_ip_dyn,argv[1]);
    dyn=1;
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
    printf("Levels :\n - 1 (PWN) Find my secret\n"
                     " - 2 (PWN) Now you are warmed up ... :P\n"
                     " - 3 (REVERSING/PROGRAMMING) Survive 30 second against the server and it'll send you the flag\n"
                     "\nLevel? : ");
    bzero(buffer,strlen(buffer));
    //fgets(buffer,1,stdin);
    
    
    buffer[0]=fgetc(stdin);
    //buffer[0]='0';
    
    char str[4];
    bzero(str,strlen(str));
    snprintf(str,4,"Lv%c",buffer[0]);
    n = write(sockfd,str,strlen(str));
    if (n < 0) 
        perror("ERROR writing to socket");
    bzero(buffer,strlen(buffer));
    while ( (n = read(sockfd, buffer, sizeof(buffer)-1)) > 0)
    {
        buffer[n] = 0;
        if(strncmp("OK", buffer,2)==0){
            int letti = sscanf(buffer,"OK %d",&SERVER_PORT);
            if (letti==1){
                close(sockfd);
                return;
            }
                
        }
        
    }
    if (n < 0) 
        perror("ERROR reading from socket");
    printf("Wrong level or network error!\n");
    close(sockfd);
    exit(0);
}


int main(int argc,char *argv[])
{
    intro(argc,argv);
    
    //fprintf(stderr,"port %d, ip %s",SERVER_PORT,send_ip_dyn);
    if(setupSocket()==-1)
    {
        fprintf(stderr,"Errore nel setup del Socket\n");
        exit(1);
    }
    
    loop=uv_default_loop();
    setupListener();
    if(setupSender((dyn == 1) ? send_ip_dyn:"127.0.0.1",SERVER_PORT)==-1)
    {
        fprintf(stderr,"Errore nel setup del Sender\n");
        exit(1);
    }
    

    uv_timer_init(loop, &connect_timer);    
    uv_timer_init(loop, &input_timer);
    initGameLoop();

    uv_timer_start(&input_timer, inputLoop, 0, 16);
    return uv_run(loop, UV_RUN_DEFAULT);
}