#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>

//-----Files
#include "utils.h"
#include "reg.h"

extern int file;

long set_initialtime(){
    clock_t begin = clock();
    char time[50];
    sprintf(time, "%ld", begin);
    char* last;
    if ((last = getenv("TIME")) != NULL){
        long last_time =  begin + atoi(last);
        char ltime[50];
        sprintf(ltime, "%ld", last_time);
        setenv("TIME", ltime, 1);
    }
    else setenv("TIME", time, 1);

    return begin;
}

void set_lasttime(){
    clock_t last = clock();

    char time[50];
    sprintf(time, "%ld", last + atoi(getenv("TIME")));
    setenv("TIME", time, 0);
}

double set_time(){
    long time = atoi(getenv("TIME"));

    clock_t current = clock();

    double ret = (double)(current + time) / CLOCKS_PER_SEC;
    ret *= 1000;
    return ret;
}

void write_create(char** cmd){
    char writecreate[10] = "CREATE";
    char create[50];
    sprintf(create, "%f - %-8d - %-10s - ", set_time(), getpid(), writecreate);
    write(file, create, strlen(create));
    printCMD(cmd, file);
}

void write_exit(int exit_code){
    char writeexit[10] = "EXIT";
    char code[50];
    sprintf(code, "%f - %-8d - %-10s - %d\n", set_time(), getpid(), writeexit, exit_code);
    write(file, code, strlen(code));
}

void receive_pipe(char* received){
    char receivepipe[10] = "RECV_PIPE";
    char receive[LIMITER];
    sprintf(receive, "%f - %-8d - %-10s - (block bellow)\n", set_time(), getpid(), receivepipe);
    write(file, receive, strlen(receive));
    write(file, "--------------------------\n", strlen("--------------------------\n"));
    write(file, received, strlen(received));
    write(file, "--------------------------\n", strlen("--------------------------\n"));
}

void send_pipe(char* sent){
    char sendpipe[10] = "SEND_PIPE";
    char send[PATH_MAX];
    sprintf(send, "%f - %-8d - %-10s - %s", set_time(), getpid(), sendpipe, sent);
    write(file, send, strlen(send));
}