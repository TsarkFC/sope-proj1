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

//---Files
#include "utils.h"
#include "flags.h"

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

    char* cmd[10];

    int dirSize = 0;

    if ((dirp = opendir(pathAd)) == NULL)
    {
        char test[50];
        sprintf(test, "Path error: %s\n", pathAd);
        write(file, test, strlen(test));
        perror(pathAd);
        exit(2);
    }

    while ((direntp = readdir(dirp)) != NULL){
        int pp[2];
        if (pipe(pp) == -1) printf("Pipe error %s\n", strerror(errno));

        sprintf(fp, "%s/%s", pathAd, direntp->d_name);

        if (lstat(fp, &stat_buf)==-1){
            perror("lstat ERROR");
            exit(3);
        }

        if (S_ISREG(stat_buf.st_mode)) {
            long num = stat_buf.st_size;
            char sendFile[50];

            if(!(b && !B)){
                round_up_4096(&num);
                num = (num + Bsize/2) / Bsize;
            }

            if (all && (!mDepth || maxDepth > 0)) {
                sprintf(sendFile, "%-8ld %s \n", num, fp);
                write(STDOUT_FILENO, sendFile, strlen(sendFile));
            }
            
            dirSize += num;
            
        }
        
        else if (S_ISDIR(stat_buf.st_mode)) {

            strcpy(directoryname, direntp->d_name);
            if(check_point_folders(directoryname)){
                pid = fork();

                if (pid == 0 ){
                    if (mDepth) maxDepth--;
                    cmd_builder(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp, cmd, file);

                    close(pp[READ]);
                    if (dup2(pp[WRITE], STDOUT_FILENO) == -1) printf("Dup error %s\n", strerror(errno));
                    execvp("./simpledu", cmd);
                }

                else if (pid > 0){
                    wait(&status);

                    close(pp[WRITE]);
                    char content[PATH_MAX];
                    char copy[PATH_MAX];

                    while (read(pp[READ], content, PATH_MAX)){
                        strcpy(copy, content);
                        char* lines[MAX_INPUT] = { '\0' };
                        int linesSize = line_divider(copy, lines, file);
                        
                        if (!S){
                            add_initial_numbers(lines, &dirSize, pathAd, fp, file, linesSize);
                        }

                        if ((!mDepth || (maxDepth > 0))) {
                            write(STDOUT_FILENO, content, strlen(content));
                        }
                        
                        memset(content, 0, sizeof(content));
                        freeLines(lines, linesSize);
                    }
                }
            }

            memset(directoryname, 0, sizeof(directoryname));
        }
        
        else{ //Links
            long num = stat_buf.st_size;
            char sendFile[50];

            if(!(b && !B)){
                num = 0;
            }

            if (all && (!mDepth || maxDepth > 0)) {
                sprintf(sendFile, "%-8ld %s \n", num, fp);
                write(STDOUT_FILENO, sendFile, strlen(sendFile));
            }

            dirSize += num;
        }
    }

    if (b && !B){
        dirSize += 4096;
    }
    else{
        long tempSize = 4096;
        round_up_4096(&tempSize);
        tempSize = ((tempSize + Bsize/2) / Bsize);
        dirSize += tempSize;
    }

    char sendDir[50];
    sprintf(sendDir,"%-8d %s \n", dirSize, pathAd);
    write(STDOUT_FILENO, sendDir, strlen(sendDir));
    

    closedir(dirp);

    return 0;
}