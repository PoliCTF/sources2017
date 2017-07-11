#include <stdio.h>
#include <stdlib.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <malloc.h>
#include <alloca.h>
#include <time.h>
#include <stdbool.h>

int readn(char*, unsigned int);

unsigned int length = 0;
char* name = NULL;
int* points = 0;
int registered = 0;
int socket_desc , client_sock , c ;
struct sockaddr_in server , client;

int readn(char* buf, unsigned int n){
	int nread = 0;
	char c[10];
	nread = read(STDIN_FILENO,buf,(n));
	if (nread == -1){
		puts("Error reading \n");
		exit(0);
	}
	buf[nread] = 0;
	return nread;
}

void game(void){ 
  srand(time(NULL));

  int r = rand() % 10 + 1;
  bool correct = false; 
  int guess; 
  int counter = 0; 

  while(!correct)
  {
    printf("Guess my number! "); 
    scanf("%d", &guess);
    getchar();
	counter++;
    if (guess < r) {
        printf("Your guess is too low. Guess again.\n");
    }
    else if (guess > r) { 
        printf("Your guess is too high. Guess again.\n");
    }
    else {
        printf("You guessed correctly in %d tries! Congratulations!\n", counter); 
        correct = true; 
    }

    
  }

  return;   
}

void doRegister(){
	puts("Welcome, New Player!");
	puts("Introduce yourself:");
	registered = 0;
	while(!registered){
		puts("How long is your name?");
		int len = 0;
		scanf("%d",&len);
		if (len>100){
			puts("Way too long!");
			exit(0);
		}
		
		unsigned int local = abs(len);
		name = alloca(local+1);
		length = len;
		puts("Enter your name:");
		int nread;
		nread = readn(name,length+1);
		if (nread <= 0){
			puts("Invalid name");
			exit(0);
		}
		puts("Arey you sure about the name?");
		puts("[y]es\n[c]ancel\n[r]etry");
		char minibuf[3];
		minibuf[0]=0;
		readn(minibuf,3);
		char *swap;
		switch (minibuf[0]){
			case 'y':
				swap = malloc(nread);
				if (swap == NULL){
					puts("Malloc error");
					exit(0);
				}
				strncpy(swap, name, nread);
				name = swap;
				registered = 1;
				break;
			case 'c':
				return;
			case 'r':
				break;
			default :
				break;		
		}

	}
	return;
}


void doPlay(){
	if (registered == 0)
	{
		puts("You need to register first!!");
		return;
	}
	puts("Welcome ");
	printf("%s\n",name);
	game();
}

void menu(){
	while(1){
		
		
		puts("[1] Register");
		puts("[2] Play");
		puts("[3] Exit");
		
		char minibuf[3];
		minibuf[0]=0;
		readn(minibuf,3);
		switch(minibuf[0]){
			case '1':
				doRegister();
				break;
			case '2':
				doPlay();
				break;
			case '3':
				exit(0);
				break;
			default:
				break;
		}
	}
}

int do_child(){
	puts("Welcome to Guess the Number! \n");
	menu();
	
}


int main(int argc , char *argv[])
{
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
  		
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    int enable = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    perror("setsockopt(SO_REUSEADDR) failed");
	    return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("bind failed. Error");
        return 1;
    }
     
    listen(socket_desc , 3);
     
    c = sizeof(struct sockaddr_in);
    while (1){ 
	printf("waiting!->\n");
    	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    	if (client_sock < 0)
    	{
        	perror("accept failed");
        	return 1;
    	}
    	int pid;
	if((pid = fork()) == -1){
		perror("fork");
		close(client_sock);
		exit(1);
	}else if(pid > 0){
		close(client_sock);
		continue;
	}else if(pid == 0){
		//TODO :
		//DROP PRIVILEGE
		dup2(client_sock,fileno(stdin));
		dup2(client_sock,fileno(stdout));
		dup2(client_sock,fileno(stderr));
		close(client_sock);
		do_child();
		break;
	}

	break;
    }
    close(socket_desc);
    return 0;
}
