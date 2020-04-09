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
    setenv("LAST", time, 0);
}

double set_time(){
    long time = atoi(getenv("TIME"));

    clock_t current = clock();

    double ret = (current + time) / CLOCKS_PER_SEC;
    ret *= 0.01;
    char test[50];
    sprintf(test, "NUMBER: %ld\n", current + time);
    write(file, test, strlen(test));
}

void write_create(char** cmd){
    char create[50];
    sprintf(create, "%f - %-8d - CREATE - ", set_time(), getpid());
    write(file, create, strlen(create));
    printCMD(cmd, file);
}

int write_exit(){
    return 0;
}