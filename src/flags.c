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

//-----Files
#include "utils.h"

extern int file;


void handleB_output(char** lines, int linesSize, int Bsize){
    //write() --> STDOUT     ROUNDING --> num = (num + Bsize - 1) / Bsize;
    char* copy;
    char* number; 
    char* path;

    for (int i = 0; i < linesSize; i++){
        copy = malloc(strlen(lines[i]));
        strcpy(copy, lines[i]);
    
        number = strtok_r(copy, " ", &copy);
        path = strtok_r(copy, " ", &copy);
        
        write(file, "number: ", strlen("number: "));
        write(file, number, strlen(number));

        long size = atoi(number);
        round_up_4096(&size);
        size = (size + Bsize/2) / Bsize;

        char test[50];
        int_to_char(size, test);
        write(file, "  is now: ", strlen("  is now: "));
        write(file, test, strlen(test));
        write(file, "\n", 1);

        char sendDir[50];
        sprintf(sendDir,"%-8ld %s \n", size, path);
        write(STDOUT_FILENO, sendDir, strlen(sendDir));
        write(file, sendDir, strlen(sendDir));

    }
}