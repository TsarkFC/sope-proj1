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
#include <time.h>

//---Files
#include "utils.h"
#include "rec.h"
#include "reg.h"

// ./simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]

// • -a, --all – a informação exibida diz respeito também a ficheiros;
// • -b, --bytes – apresenta o número real de bytes de dados (ficheiros) ou alocados (diretórios);
// • -B, --block-size=SIZE – define o tamanho (bytes) do bloco para efeitos de representação;
// • -l, --count-links – contabiliza múltiplas vezes o mesmo ficheiro;
// • -L, --dereference – segue links simbólicos;
// • -S, --separate-dirs – a informação exibida não inclui o tamanho dos subdiretórios;
// • --max-depth=N – limita a informação exibida a N (0,1, …) níveis de profundidade de diretórios

int file;


int main(int argc, char *argv[]){

    struct sigaction pause_signal;
    pause_signal.sa_handler = sigint_handler;
    sigemptyset(&pause_signal.sa_mask);
    pause_signal.sa_flags = 0;

    if (sigaction(SIGINT,&pause_signal,NULL) < 0)
    {
            fprintf(stderr,"Unable to install SIGINT handler\n");
            exit(1);
    }


    struct timespec start;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

    //'boolean' variable initialization
    int all = 0;
    int b = 0;
    int B = 0; int Bsize = 1024; // Bsize corresponds to block size indicated
    int path = 0; char pathAd[PATH_MAX];
    int L = 0;
    int S = 0;
    int mDepth = 0; int maxDepth = 0; //maxDepth corresponds to max depth value

    //wrong usage
    if (argc > 10){ 
        write(STDOUT_FILENO, "USAGE: ./simpledu [-l] [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",
        strlen("USAGE: ./simpledu [-l] [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n"));
        write_exit(1, start);
        exit(1);
    }
    //interpret argv content
    for (int i = 1; i < argc; i++){

        //set fot [-a] or [-all]
        if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"-all") == 0) all = 1;

        //set for [-b]
        else if (strcmp(argv[i],"-b") == 0) {
            b = 1;
            B = 0;
            Bsize = 1024;
        }

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
            char maxD[50];
            slice_str(argv[i], maxD, 0, 11);
            if (strcmp(maxD, "--max-depth=") == 0){
                slice_str(argv[i], maxD, 12, 13);
                maxDepth = atoi(maxD);
                mDepth = 1;
            }
        }
        else if (strcmp(argv[i],"-l") == 0) continue;

        //set for path
        else{
            path = 1;
            strcpy(pathAd, argv[i]);
        }
    }

    //Set default path
    if (!path) strcpy(pathAd,".");

    char log_file[MAX_INPUT];

    if (getenv("LOG_FILENAME") == NULL){
        strcpy(log_file, "reg.txt");
    }else strcpy(log_file, getenv("LOG_FILENAME"));

    file = open(log_file, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (file == -1){
        printf("File error (change) %s\n", strerror(errno));
    }

    //Ignore signals

    pause_signal.sa_handler = sigint_handler;
    if (sigaction(SIGINT,&pause_signal,NULL) < 0)
    {
            fprintf(stderr,"Unable to install SIGINT handler\n");
            exit(1);
    }

    init(all, b, B, Bsize, path, L, S, mDepth, maxDepth, pathAd, start);
    
    write_exit(0, start);
    close(file);
    return 0;
}
