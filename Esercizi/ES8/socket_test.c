#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

int main(){
    int fd_skt, fd_c;
    if(fork()!=0){
        fd_skt = socket(AF_UNIX,SOCK_STREAM,0);
    }
    else{
        fd_c=socket(AF_UNIX,SOCK_STREAM,0);
    }
    return 0;
}