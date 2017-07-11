#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libcgroup.h>
#include <string.h>
#include <error.h>

#include <signal.h>

#include "met.pb-c.h"

#define SIZE 0x4000
#define RSS_MEMORY_LIMIT 10000 *1024 //100KB
#define MAX_MSG_SIZE 10000
#define MAX_COUNT 10000
#define ALARM_SECONDS 900
#define UID 1001
#define GID 1001
// #define DEBUG

int min ( int a, int b ) { return a < b ? a : b; }

static void before_main(void) __attribute__((constructor));

char MEM_ERR_FULL[] = "Do not you think you had enough memory?\n";
char MEM_SUCC[] = "Moar memory!\n";
char WRITE_FAIL[] = "You do not have all this memory.\n";
char WRITE_SUCC[] = "That is enough, I'm tired.\n";
char READ_FAIL[] = "You cannot read this memory.\n";
char READ_SUCC[] = "That is enough, I'm tired.\n";
char AMOUNT_FAIL[] = "Wrong Request.\n";



void *base=0;
unsigned long int counter=1;
unsigned long long int allocated=0;
char canallocate=0;

void handle_alarm( int sig ) {
    exit(-2);
}



unsigned int get_num(){
    char buf[10];
    memset(buf, 0, 10);
    printf("gimmi a numba!\n");
    read(0, buf, 9);
    return atoi(buf);
}


void motd(){
    printf("Welcome to the jungle!\n");
    printf("Here you are free to read and write!\n");
    printf("If you need more memory, just ask!\n");
}

void menu(){
    printf("1) Gimme More!\n");
    printf("2) Put it down!\n");
    printf("3) Show me the stuff!\n");
    printf("4) Get out!\n");
}

void print_result(Result * r){
    // fprintf(stderr, "%d %d %s\n", r->id, r->status, r->msg);
}

int get_size(int num){
    switch(num){
        case 1:
            return 0x150000;
        case 2:
            return 0x100000;
        case 3:
            return 0x16000;
        case 4:
            return 0x8000;
        case 5:
            return 0x4000;
        case 6:
            return 0x2000;
        case 7:
            return 0x1000;
        }
    return 0;
}

Result *gimme_more(Command *r){
    void *anon;
    Result *resp = malloc(sizeof(Result));
    result__init(resp);
    // #ifdef DEBUG
        // fprintf(stderr, "size: %d\n", r->size);
        // fprintf(stderr, "canallocate %d\n", canallocate);
        // fprintf(stderr, "canallocate >= r->size: %d\n", canallocate >= r->size);
    //     fprintf(stderr, "r->size <= 0: %d\n", r->size <= 0);
    //     fprintf(stderr, "r->size > COMMAND__SIZE___16K: %d\n", r->size > COMMAND__SIZE___16K);
    // #endif

    if (canallocate >= r->size || r->size <= 0 || r->size > COMMAND__SIZE___1K){
        // fprintf(stderr, "%s", MEM_ERR_FULL);
        resp->msg = MEM_ERR_FULL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        return resp;
    }

    // fprintf(stderr, "%s", MEM_SUCC);
    resp->msg = MEM_SUCC;
    resp->status = RESULT__STATUS_TYPE__SUCCESS;
    anon = mmap(base + allocated, get_size(r->size), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    allocated += get_size(r->size);

    #ifdef DEBUG
        fprintf(stderr, "Allocated %p %p\n", base, anon);
        fprintf(stderr, "Allocated %lx\n", allocated);
    #endif

    if (anon < base){
        resp->msg = MEM_ERR_FULL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        // fprintf(stderr, "%s", MEM_ERR_FULL);
        canallocate = r->size;
    }
    #ifdef DEBUG
         print_result(resp);
    #endif

    return resp;
}


Result *write_mem(Command * r){
    unsigned long int i;
    Result *resp = malloc(sizeof(Result));
    result__init(resp);

    if (!r->has_ammount){
        resp->msg = AMOUNT_FAIL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        fprintf(stderr, "%s", AMOUNT_FAIL);
        return resp;
    }
    i = r->ammount;

    fprintf(stderr, "I know you would write everything, but it is huge. We'll take all night.\n");
    fprintf(stderr, "Just tell me where I should start.\n");
    if (i > allocated){
        resp->msg = WRITE_FAIL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        fprintf(stderr, "%s", WRITE_FAIL);
        return resp;
    }
    read(0, base+i, 0x1000);
    resp->msg = WRITE_SUCC;
    resp->status = RESULT__STATUS_TYPE__SUCCESS;
    fprintf(stderr, "%s", WRITE_SUCC);
    return resp;
}

Result *read_mem(Command *r){
    unsigned long int i;
    #ifdef DEBUG
        fprintf(stderr, "Reading stuf!\n");
    #endif

    Result *resp = malloc(sizeof(Result));
    result__init(resp);

    if (!r->has_ammount){
        resp->msg = AMOUNT_FAIL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        fprintf(stderr, "%s", AMOUNT_FAIL);
        return resp;
    }
    i = r->ammount;

    fprintf(stderr, "I could print the whole memory and wait years for you to parse it,\n");
    fprintf(stderr, "Just tell me where I should start.\n");
    if (i > allocated){
        resp->msg = READ_FAIL;
        resp->status = RESULT__STATUS_TYPE__FAILED;
        fprintf(stderr, "%s", READ_FAIL);
        return resp;
    }
    write(1, base+i, 0x1000);
    resp->msg = READ_SUCC;
    resp->status = RESULT__STATUS_TYPE__SUCCESS;
    fprintf(stderr, "%s", READ_SUCC);
    return resp;
}

static void before_main(void){
    void *anon;
    cgroup_init();
    struct cgroup *cg = cgroup_new_cgroup("met");
    struct cgroup_controller* controller = cgroup_add_controller(cg, "memory");
    cgroup_set_value_int64(controller, "memory.limit_in_bytes", RSS_MEMORY_LIMIT); //100KB
    cgroup_set_value_int64(controller, "memory.swappiness", 0); //100KB
    cgroup_create_cgroup(cg, 0);
    cgroup_attach_task (cg);
    if (getuid() == 0) {
        /* process is running as root, drop privileges */
        if (setgid(GID) != 0) {
            printf("setgid: Unable to drop group privileges\n");
            exit(-1);
        }
        if (setuid(UID) != 0) {
            printf("setuid: Unable to drop user privileges\n");
            exit(-1);
        }

    }
    if (getuid() == 0 || getgid() == 0){
        printf("You should not be root!\n");
        exit(-1);
    }


    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    anon = base = (char*)mmap(base, SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    allocated += SIZE;
    // printf("Base Allocated at %p\n", anon);
    signal( SIGALRM, handle_alarm );
    alarm( ALARM_SECONDS );
}

void custom_read(char * buffer, size_t size){
    int i;

    for (i = 0; i < size; i++)
        buffer[i] = getchar();

    // if (i == size - 1)
    //     fflush_stdin();

    // buffer[i]='\0';
}

void read_fixed_size(int fd, char *buf,  size_t size){
    custom_read(buf, size);
    // size_t len;
    // #ifdef DEBUG
    //  int i=0;
    // #endif

    // while (size > 0){
    //     len = read(0, buf, size);
    //     size -= len;
    //     #ifdef DEBUG
    //         fprintf(stderr, "len: %d  size %d char: %x buf: %x\n", len, size, buf[0], buf);
    //     #endif
    //     buf += len;

    // }
}

Request *get_request(){
    #ifdef DEBUG
        register long int sp asm ("sp");
        fprintf(stderr, "get req %p\n", sp);
    #endif
    Request *msg;
    uint8_t buf[MAX_MSG_SIZE];
    memset(buf, 0, MAX_MSG_SIZE);
    size_t read_size = get_num();
    size_t msg_len = min(read_size, MAX_MSG_SIZE);
    // #ifdef DEBUG
    //     fprintf(stderr, "rs: %d  mslen %d\n", read_size, msg_len);
    // #endif
    read_fixed_size(0, buf, msg_len);
    // #ifdef DEBUG
    //     fprintf(stderr, "packed: %s\n", buf);
    // #endif
    msg = request__unpack (NULL, msg_len, buf); // Deserialize the serialized input
    // #ifdef DEBUG
    //     fprintf(stderr, "upacked!\n");
    // #endif
    if (msg == NULL)
    { // Something failed
        fprintf(stderr, "error unpacking incoming message %d %s\n", msg_len, buf);
        // return -1;
        raise(SIGINT);
        exit(-1);
    }
    return msg;
}

int response_size(Request *r){
    unsigned int tot = 0;
    unsigned int i;
    for (i = 0; i < r->n_command; i++)
        { // Iterate through all repeated int32
            tot += r->command[i]->count;
        }
    return tot;
}

void send_response(Response *r){
    char *buf;
    unsigned int size;
    size = response__get_packed_size(r);
    // #ifdef DEBUG
    //     fprintf(stderr, "Sending response of size %d\n", size);
    // #endif
    buf = malloc(size);
    response__pack(r, buf);
    write(1, buf, size);
    free(buf);
}

void free_response(Response *r){
    unsigned int i;
    Result *res;
    // #ifdef DEBUG
    //     fprintf(stderr, "Free response 0x%x\n",r);
    // #endif
    for (i=0; i < r->n_result; i++){
        res = r->result[i];
        // #ifdef DEBUG
        //     fprintf(stderr, "Free result 0x%x index:%d\n",res, i);
        // #endif
        free(res);
    }
    free(r);
}


int main(){
    char buf[0x8000];
    Request *r;
    long int i, j, res_index;
    unsigned int choice, resp_size;
    Response *response;
    Result *result=NULL;
    Result **results;
    motd();
    menu();
    #ifdef DEBUG
        register long int sp asm ("sp");
        fprintf(stderr, "Main %p\n", sp);
        unsigned long long page_count;
        fprintf(stderr, "--------------------\nbase %p, page_count %p\n", base, &page_count);
        page_count = ((int)&page_count - (int)base) / SIZE;
        fprintf(stderr, "--------------------\nabout %lld pages. Can you do it?\n", page_count);
    #endif

    while (1){
        response = malloc(sizeof(Response));
        response__init(response);
        r = get_request();
        if (r == -1){
            continue;
        }
        resp_size = response_size(r) * sizeof(Result *);
        results = malloc(resp_size);
        res_index = 0;

        // #ifdef DEBUG
        //     fprintf(stderr, "request: 0x%x, resp_size %d, results: 0x%x, response: 0x%x \n",r, resp_size, results, response);
        // #endif
        if (r->n_command > 20){
            exit(-1);
        }
        for (i = 0; i < r->n_command; i++)
        { // Iterate through all repeated int32
            choice = r->command[i]->type;
            // #ifdef DEBUG
            //     fprintf(stderr, "id %d, choice: %d count: %d\n",r->command[i]->id,  choice, r->command[i]->count);
            // #endif
            if (r->command[i]->count > MAX_COUNT){
                fprintf(stderr, "we gotta problem!\n");
                exit(-1);
            }
            for (j = 0; j < r->command[i]->count; j++){
                switch(choice){
                    case COMMAND__COMMAND_TYPE__MEMORY:
                        result=gimme_more(r->command[i]);
                        break;
                    case COMMAND__COMMAND_TYPE__WRITE:
                        result=write_mem(r->command[i]);
                        break;
                    case COMMAND__COMMAND_TYPE__READ:
                        result=read_mem(r->command[i]);
                        break;
                    case COMMAND__COMMAND_TYPE__EXIT:
                        return 0;
                        break;
                    default:
                        // if (r->command[i]->has_ammount){
                        //     #ifdef DEBUG
                        //         fprintf(stderr, "Alloca %lx\n", r->command[i]->ammount);
                        //     #endif
                        //     char *top = alloca(r->command[i]->ammount);
                        //     top[0] = 0;
                        // }
                        main();
                        // return 0;
                    }
                if (result != NULL){
                    if (result->status == RESULT__STATUS_TYPE__FAILED || j == r->command[i]->count -1){
                        result->id = r->command[i]->id;
                        // #ifdef DEBUG
                        //     fprintf(stderr, "results: %p, index:%d, ids %d, altro id: %d\n", result,  res_index, r->command[i]->id, result->id);
                        // #endif
                        results[res_index++] = result;
                    }
                    else{
                        free(result);
                    }
                }
            }
        }
        response->n_result = res_index;
        response->result = results;
        // #ifdef DEBUG
        //     fprintf(stderr, "results: %p, %p num:%d\n", results, response->result, res_index);
        // #endif

        request__free_unpacked(r,NULL);
        send_response(response);
        free(results);
        // free_response(response);
    }
}