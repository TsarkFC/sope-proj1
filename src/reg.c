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

double set_time(struct timespec start){
    struct timespec stop;

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

    double ret = ( stop.tv_sec - start.tv_sec ) / 1000.0 + ( stop.tv_nsec - start.tv_nsec ) / 1000000.0;

    return ret;
}

void write_create(struct timespec start){
    char writecreate[10] = "CREATE";
    char create[50];
    sprintf(create, "%.2f - %-8d - %-10s - ", set_time(start), getpid(), writecreate);
    write(file, create, strlen(create));
}

void write_exit(int exit_code, struct timespec start){
    char writeexit[10] = "EXIT";
    char code[50];
    sprintf(code, "%.2f - %-8d - %-10s - %d\n", set_time(start), getpid(), writeexit, exit_code);
    write(file, code, strlen(code));
}

void receive_pipe(char* received, struct timespec start){
    char receivepipe[10] = "RECV_PIPE";
    char receive[LIMITER];
    sprintf(receive, "%.2f - %-8d - %-10s - %s\n", set_time(start), getpid(), receivepipe, received);
    write(file, receive, strlen(receive));
}

void send_pipe(char* sent, struct timespec start){
    char sendpipe[10] = "SEND_PIPE";
    char send[PATH_MAX];
    sprintf(send, "%.2f - %-8d - %-10s - %s\n", set_time(start), getpid(), sendpipe, sent);
    write(file, send, strlen(send));
}

void entry(long size, int B, int Bsize, char* path, struct timespec start){
    if (B){
        round_up_4096(&size);
        size = size / Bsize + (size % Bsize != 0);
    } 

    char entry[10] = "ENTRY";
    char send[PATH_MAX];
    sprintf(send, "%.2f - %-8d - %-10s - %-8ld %s\n", set_time(start), getpid(), entry, size, path);
    write(file, send, strlen(send));
}

void send_signal(char* signal, struct timespec start, pid_t pid){
    char send[PATH_MAX];
    sprintf(send, "%.2f\t - %-8d - SEND_SIGNAL - %-10s - %d\n", set_time(start), getpid(), signal, pid);
    write(file, send, strlen(send));
}

void recv_signal(char* signal, struct timespec start){
    char recv[PATH_MAX];
    sprintf(recv, "%.2f\t - %-8d - RECV_SIGNAL - %-10s\n", set_time(start), getpid(), signal);
    write(file, recv, strlen(recv));
}