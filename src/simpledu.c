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

//---Files
#include "utils.h"
#include "simpledu.h"

// ./simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]

// • -a, --all – a informação exibida diz respeito também a ficheiros;
// • -b, --bytes – apresenta o número real de bytes de dados (ficheiros) ou alocados (diretórios);
// • -B, --block-size=SIZE – define o tamanho (bytes) do bloco para efeitos de representação;
// • -l, --count-links – contabiliza múltiplas vezes o mesmo ficheiro;
// • -L, --dereference – segue links simbólicos;
// • -S, --separate-dirs – a informação exibida não inclui o tamanho dos subdiretórios;
// • --max-depth=N – limita a informação exibida a N (0,1, …) níveis de profundidade de diretórios

int main(int argc, char *argv[]){

    //'boolean' variable initialization
    int all = 0;
    int b = 0;
    int B = 0; int Bsize = 1024; // Bsize corresponds to block size indicated
    int path = 0; char pathAd[PATH_MAX];
    int L = 0;
    int S = 0;
    int mDepth = 0; int maxDepth = 0; //maxDepth corresponds to max depth value

    //worng usage
    if (argc > 10 || argc < 2 || strcmp(argv[1], "-l") != 0){ 
        write(STDOUT_FILENO, "USAGE: ./simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",
        strlen("USAGE: ./simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n"));
        exit(1);
    }

    //interpret argv content
    for (int i = 2; i < argc; i++){
        //printf("Argument: %s\n", argv[i]);

        //set fot [-a] or [-all]
        if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"-all") == 0) all = 1;

        //set for [-b]
        else if (strcmp(argv[i],"-b") == 0) b = 1;

        //set for [-B size]
        else if (strcmp(argv[i], "-B") == 0){
            if(is_number(argv[i+1]) == 1){
                B = 1;
                Bsize = atoi(argv[i+1]); i++;
            }
        }

        //set for [-L]
        else if (strcmp(argv[i],"-L") == 0) L = 1;

        //set for [-S]
        else if (strcmp(argv[i],"-S") == 0) S = 1;

        //set for [--max-depth=N]
        else if (argv[i][0] == '-' && argv[i][1] == '-'){
            char maxD[13];
            slice_str(argv[i], maxD, 0, 11);
            if (strcmp(maxD, "--max-depth=") == 0){
                slice_str(argv[i], maxD, 12, 13);
                maxDepth = atoi(maxD);
                mDepth = 1;
            }
        }

        //set for path
        else{
            path = 1;
            strcpy(pathAd, argv[i]);
        }
    }

    init(all, b, B, Bsize, path, L, S, mDepth, maxDepth, pathAd);

    return 0;
}

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
    char paths[PATH_MAX];

    int dir_count = 0;

    if ((dirp = opendir(pathAd)) == NULL)
    {
        perror(pathAd);
        exit(2);
    }

    while ((direntp = readdir(dirp)) != NULL){
        snprintf(fp, sizeof(fp), "%s/%s", pathAd, direntp->d_name);
        if (lstat(fp, &stat_buf)==-1)
        {
            perror("lstat ERROR");
            exit(3);
        }
        if (S_ISREG(stat_buf.st_mode)) {
            long num = stat_buf.st_size;

            //send through pipe to main process

            if (all) {
                if(b && !B){
                    printf("%-8ld %s \n", num, fp);
                }
                else{
                    round_up_4096(&num);
                    num = num / Bsize;
                    printf("%-8ld %s \n", num, fp);
                }
            }
        }
        
        else if (S_ISDIR(stat_buf.st_mode)) {
            dir_count++;
            strcpy(directoryname, direntp->d_name);
            if(directoryname[0] != '.' || directoryname[1] != '\0'){
                if(directoryname[1] != '.' || directoryname[2] != '\0'){
                    pid = fork();

                    if (pid == 0){
                        char* cmd[10];
                        cmd_builder(all, b, B, Bsize, path, L, S, mDepth, maxDepth, fp, cmd);
                        execvp("./simpledu", cmd);
                    }
                    else if (pid > 0){
                        wait(&status);
                        printf("%-8ld %s \n", stat_buf.st_size, fp);
                    }
                }
            }
            memset(directoryname, 0, sizeof(directoryname));
        }
        
    }
    closedir(dirp);

    return 0;
}


