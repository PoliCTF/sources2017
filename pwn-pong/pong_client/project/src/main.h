#include "structs.h"

Game game;
Control input;
Entity player1,player2, *self, entity[MAX_ENTITIES],ball;
Sprite sprite[MAX_SPRITES];
Sound sound[MAX_SOUNDS];
int BULLET_X_SPEED = 3;
int BULLET_Y_SPEED = 2;
int end=0;
int winner;
int input_grezzo=-2;
int input_su=0;
int input_giu=0;
unsigned int frameLimit;
int go;
uv_timer_t input_timer;
uv_timer_t connect_timer;
int numeroInvii=0;
int startTimerOn=1;
/*LISTENER VARIABLES*/
#define LOCAL_PORT 4242
//#define LOCAL_PORT 3232

uv_loop_t *loop;
uv_udp_t recv_socket;
uv_udp_t* send_socket;
struct sockaddr_in si_me;
int attuale=0;
int old=0;
int pacchetti=0;
/*END*/

/*SENDER VARIABLES*/
int SERVER_PORT;
#define SEND_IP "127.0.0.1"
char send_ip_dyn[20];
int dyn=0;
struct sockaddr_in si_other;
int s,slen;
int i=0;

/*END*/