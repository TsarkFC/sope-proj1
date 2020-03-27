#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void slice_str(const char * str, char * buffer, size_t start, size_t end){
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

void int_to_char(int no, char* ch){
    sprintf(ch, "%d", no);
}

void cmd_builder(int all, int b, int B, int Bsize, int path, int L, int S, int mDepth, int maxDepth, char* pathAd, char** cmd){
    cmd[0] = "./simpledu";
    cmd[1] = "-l";
    
    int i = 2;

    if (all) {
        cmd[i] = "-a";
        i++;
    }
    if (b) {
        cmd[i] = "-b";
        i++;
    }
    if (B) {
        char str[100] = "";
        cmd[i] = "-B";
        i++;
        int_to_char(Bsize, str);
        cmd[i] = str;
        i++;
    }
    if (path) {
        cmd[i] = pathAd;
        i++;
    }
    if (L) {
        cmd[i] = "-L";
        i++;
    }
    if (S) {
        cmd[i] = "-S";
        i++;
    }
    if (mDepth){
        char max[100] = "--max-depth=";
        int_to_char(maxDepth, max);
        cmd[i] = max;
    }

    
}