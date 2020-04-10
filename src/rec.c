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

//---Files
#include "utils.h"
#include "reg.h"
#include "rec.h"

extern int file;

int init(int all, int b, int B, int Bsize, int path, 
            int L, int S, int mDepth, int maxDepth, char* pathAd, struct timespec start){

    struct sigaction pause_signal;
    pause_signal.sa_handler = sigint_handler;
    sigemptyset(&pause_signal.sa_mask);
    pause_signal.sa_flags = 0;

    if (sigaction(SIGINT,&pause_signal,NULL) < 0)
    {
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }

    DIR *dirp;
    struct dirent *direntp;
    struct stat stat_buf;
    char directoryname[150] = { '\0' };
    
    int status;
    pid_t pid = 0;
    char fp[PATH_MAX];

    int dirSize = 0;

    if ((dirp = opendir(pathAd)) == NULL)
    {
        initial_file(pathAd, start, b, B, Bsize);
        write_exit(0, start);
        exit(0);
    }

    while ((direntp = readdir(dirp)) != NULL){
        int pp[2];
        if (pipe(pp) == -1) printf("Pipe error %s\n", strerror(errno));

        sprintf(fp, "%s/%s", pathAd, direntp->d_name);

        if (lstat(fp, &stat_buf)==-1){
            perror("lstat ERROR");
            write_exit(3, start);
            exit(3);
        }

        if (S_ISREG(stat_buf.st_mode)) {
            long num = stat_buf.st_size;
            entry(num, B, Bsize, fp, start);
            char sendFile[50];

            if((B && !b) || (!B && !b)){
                num = stat_buf.st_blocks*512/Bsize + ((stat_buf.st_blocks*512)%Bsize != 0);
                dirSize += stat_buf.st_blocks*512;
            }
            else dirSize += num;

            if (all &&(!mDepth || maxDepth > 0)) {
                if (b && B) sprintf(sendFile, "%ld\t%s \n", num/Bsize + (num%Bsize != 0), fp);
                else sprintf(sendFile, "%ld\t%s \n", num, fp);
                write(STDOUT_FILENO, sendFile, strlen(sendFile));
            }
        }
        
        else if (!L && S_ISLNK(stat_buf.st_mode)){
            long num = stat_buf.st_size;
            if ((B && !b) || (!B && !b)){
                num = stat_buf.st_blocks*512 / Bsize;
            }

            if (all && (!mDepth || maxDepth > 0)) {
                char sendFile[50];
                sprintf(sendFile, "%ld\t%s \n", num, fp);
                write(STDOUT_FILENO, sendFile, strlen(sendFile));
            }

            dirSize += num;
        }

        else{
            strcpy(directoryname, direntp->d_name);
            if(check_point_folders(directoryname)){
                entry(stat_buf.st_size, B, Bsize, fp, start);
                write_create(start);
                cmd_builder(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp, file);
                pid = fork();

                if (pid == 0 ){
                    if (mDepth) maxDepth--;
                    int addSize = init(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp, start);

                    close(pp[READ]);
                    char* send = malloc(MAX_INPUT);
                    sprintf(send, "%d", addSize);

                    send_pipe(send, start);
                    if (write(pp[WRITE], send, strlen(send)) == -1){
                        perror("Writing to pipe");
                        write_exit(4, start);
                        exit(4);
                    }

                    free(send);
                    closedir(dirp);
                    write_exit(0, start);
                    exit(0);
                }

                else if (pid > 0){
                    //Ignoring SIGINT
                    pause_signal.sa_handler = SIG_IGN;
                    if (sigaction(SIGINT,&pause_signal,NULL) < 0)
                    {
                            fprintf(stderr,"Unable to install SIGINT handler\n");
                            exit(1);
                    }

                    //Wait for child
                    wait(&status);

                    //No longer ignoring SIGINT again    
                    pause_signal.sa_handler = sigint_handler;
                    if (sigaction(SIGINT,&pause_signal,NULL) < 0)
                    {
                            fprintf(stderr,"Unable to install SIGINT handler\n");
                            exit(1);
                    }
                    if (!S){
                        close(pp[WRITE]);
                        char* receive = malloc(MAX_INPUT);
                        read(pp[READ], receive, MAX_INPUT);
                        dirSize += atoi(receive);
                        receive_pipe(receive, start);
                        free(receive);
                    }
                }
            }
            memset(directoryname, 0, sizeof(directoryname));
        }
    }

    dirSize += 4096;

    char* sendDir = malloc(MAX_INPUT);

    long copy = dirSize;
    if ((B && !b) || (!B && !b)) copy = dirSize / Bsize + (dirSize % Bsize != 0);

    if (b && B) sprintf(sendDir,"%ld\t%s \n", copy/Bsize + (copy%Bsize != 0), pathAd);
    else sprintf(sendDir,"%ld\t%s \n", copy, pathAd);

    if ((!mDepth || maxDepth >= 0)) write(STDOUT_FILENO, sendDir, strlen(sendDir));

    free(sendDir);
    
    closedir(dirp);

    return dirSize;
}

void initial_file(char* path, struct timespec start, int b, int B, int Bsize){
    struct stat stat_buf;
    if (lstat(path, &stat_buf)==-1){
        perror("lstat ERROR");
        write_exit(3, start);
        exit(3);
    }
    long num = stat_buf.st_size;
        entry(num, B, Bsize, path, start);
        char sendFile[50];

        if((B && !b) || (!B && !b)){
            num = stat_buf.st_blocks*512/Bsize + ((stat_buf.st_blocks*512)%Bsize != 0);
        }
        if (b && B) sprintf(sendFile, "%ld\t%s \n", num/Bsize + (num%Bsize != 0), path);
        else sprintf(sendFile, "%ld\t%s \n", num, path);
        write(STDOUT_FILENO, sendFile, strlen(sendFile));
}