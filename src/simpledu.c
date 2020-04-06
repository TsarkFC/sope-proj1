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
#include "rec.h"
#include "flags.h"

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

    //TO KNOW WHAT IS RECEIVED THROUGHT EXEC
    // for (int i = 0; i < argc; i++){
    //     printf("%s ", argv[i]);
    // }
    // printf("\n");

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
            char maxD[50];
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

    //Set default path
    if (!path) strcpy(pathAd,".");

    file = open("reg.txt", O_WRONLY | O_APPEND);
    init(all, b, B, Bsize, path, L, S, mDepth, maxDepth, pathAd);
    close(file);

    return 0;
}
