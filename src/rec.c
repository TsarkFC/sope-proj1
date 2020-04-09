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

//---Files
#include "utils.h"
#include "flags.h"
#include "reg.h"
#include "rec.h"

extern int file;

int init(int all, int b, int B, int Bsize, int path, 
            int L, int S, int mDepth, int maxDepth, char* pathAd){
    /**USED FOR TESTING REASONS ONLY**/
    //if (all) printf("Got all\n");
    //if (b) printf("Got b\n");
    //if (L) printf("Got -L\n");
    //if (S) printf("Got -S\n");
    //if (mDepth) printf("Got %d depth\n", maxDepth);
    //if (B) printf("Got -B %d\n", Bsize);

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
        char test[50];
        sprintf(test, "Path error: %s\n", pathAd);
        write(file, test, strlen(test));
        perror(pathAd);
        write_exit(2);
        exit(2);
    }

    // int pp[2];
    // if (pipe(pp) == -1) printf("Pipe error %s\n", strerror(errno));

    while ((direntp = readdir(dirp)) != NULL){
        int pp[2];
        if (pipe(pp) == -1) printf("Pipe error %s\n", strerror(errno));

        sprintf(fp, "%s/%s", pathAd, direntp->d_name);

        if (lstat(fp, &stat_buf)==-1){
            perror("lstat ERROR");
            write_exit(3);
            exit(3);
        }

        if (S_ISREG(stat_buf.st_mode)) {
            long num = stat_buf.st_size;
            entry(num, B, Bsize, fp);
            char sendFile[50];

            // dirSize += num;

            if(!(b && !B)){
                //round_up_4096(&num);
                //num = (num) / Bsize + (num % Bsize != 0);
                num = stat_buf.st_blocks*512/Bsize + ((stat_buf.st_blocks*512)%Bsize != 0);
                dirSize += stat_buf.st_blocks*512;
            }
            else dirSize += num;

            if ((!mDepth || maxDepth > 0)) {
                sprintf(sendFile, "%ld\t%s \n", num, fp);
                write(STDOUT_FILENO, sendFile, strlen(sendFile));
            }
        }
        
        else if (!L && S_ISLNK(stat_buf.st_mode)){
            long num = stat_buf.st_size;
            if (!(b && !B)){
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
                entry(stat_buf.st_size, B, Bsize, fp);
                set_lasttime();
                write_create();
                cmd_builder(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp, file);
                pid = fork();

                if (pid == 0 ){
                    if (mDepth) maxDepth--;
                    int addSize = init(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp);

                    close(pp[READ]);
                    char* send = malloc(MAX_INPUT);
                    sprintf(send, "%d", addSize);

                    send_pipe(send);
                    if (write(pp[WRITE], send, strlen(send)) == -1){
                        perror("Writing to pipe");
                        write_exit(4);
                        exit(4);
                    }

                    free(send);
                    closedir(dirp);
                    write_exit(0);
                    exit(0);
                }

                else if (pid > 0){
                    wait(&status);
                        
                    if (!S){
                        close(pp[WRITE]);
                        char* receive = malloc(MAX_INPUT);
                        read(pp[READ], receive, MAX_INPUT);
                        dirSize += atoi(receive);
                        receive_pipe(receive);
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
    if (!b) copy = dirSize / Bsize + (dirSize % Bsize != 0);

    sprintf(sendDir,"%ld\t%s \n", copy, pathAd);
    if ((!mDepth || maxDepth > 0)) {
        write(STDOUT_FILENO, sendDir, strlen(sendDir));
    }
    free(sendDir);
    
    closedir(dirp);

    return dirSize;
}