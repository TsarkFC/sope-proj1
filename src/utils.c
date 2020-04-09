#include "utils.h"

extern int file;

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

void cmd_builder(int all, int b, int B, int Bsize, int path, int L, int S, int mDepth, int maxDepth, char* pathAd, int file){

    write(file, "./simpledu -l ", strlen("./simpledu -l "));

    if (all) write(file, "-a ", strlen("-a "));
    if (b) write(file, "-b ", strlen("-b "));
    if (path) write(file, pathAd, strlen(pathAd));
    if (L) write(file, "-L ", strlen("-L "));
    if (S) write(file, "-S ", strlen("-S "));
    if (mDepth){
        char max[50];
        char no[50];
        int_to_char(maxDepth, no);
        sprintf(max, "%s%s", "--max-depth=",no);
        write(file, max, strlen(max));
        write(file, " ", 1);
    }
    if (B) {
        char str[50];
        write(file,"-B ", strlen("-B"));
        int_to_char(Bsize, str);
        write(file ,str, strlen(str));
        write(file, " ", strlen(" "));
    }
    write(file, "\n", 1);
}

void round_up_4096(long * num){

    if (*num == 0) return;
    // int no = 0;
    // while(*num > no){
    //     no += 4096;
    // }
    // *num = no;

    if (*num % 4096 != 0) *num = (*num / 4096) * 4096 + 4096;
}

_Bool is_number(char*a){
    if(strlen(a) == 0){
        return 0;
    }
    for(int i = 0; i < strlen(a); i++){
        if((a[i] > 57 || a[i] < 48)){
            return 0;
        }
    }
    return 1;
}

int check_point_folders(char* directoryname){
    if(directoryname[0] != '.' || directoryname[1] != '\0'){
        if(directoryname[1] != '.' || directoryname[2] != '\0')
            return 1;
    }
    return 0;
}

