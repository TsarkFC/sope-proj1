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

void printCMD(char** cmd, int file){
    int j = 0;
    while (cmd[j] != 0){
        write(file, cmd[j], strlen(cmd[j]));
        write(file, "  ", strlen("  "));
        j++;
    }
    //write(file, "\n", 1);
}

void cmd_builder(int all, int b, int B, int Bsize, int path, int L, int S, int mDepth, int maxDepth, char* pathAd, char** cmd, int file){
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
        char max[50];
        char no[50];
        int_to_char(maxDepth, no);
        sprintf(max, "%s%s", "--max-depth=",no);
        cmd[i]= max;
        i++;
    }
    if (B) {
        char str[50];
        cmd[i] = "-B";
        i++;
        int_to_char(Bsize, str);
        cmd[i] = str;
        i++;
    }
    printCMD(cmd, file);
    write(file, "\n", 1);
}

void round_up_4096(long * num){
    char test1[50];
    int_to_char(*num, test1);

    if (*num == 0) return;
    int no = 0;
    while(*num > no){
        no += 4096;
    }
    *num = no;
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

int line_divider(char content[], char* lines[], int file){
    char* line;
    line = strtok(content, "\n");
    int i = 0;
    while (line != NULL){
        lines[i] = malloc(strlen(line));
        strcpy(lines[i], line);
        i++;
        line = strtok(NULL, "\n");
    }

    return i;
}

void add_initial_numbers(char** lines, int* dirSize, char* to, char* from, int file, int lineSize){
    char* num;
    char* copy;
    copy = malloc(strlen(lines[lineSize-1]));
    strcpy(copy, lines[lineSize-1]);

    // write(file, "Last line ", strlen("Last line "));
    // write(file, copy, strlen(copy));
    // write(file, "\n", 1);

    num = strtok(copy, " ");

    *dirSize += atoi(num);

    // write(file, "Adding ", strlen("Adding "));
    // write(file, num, strlen(num));
    // write(file, " to ", strlen(" to "));
    // write(file, to, strlen(to));
    // write(file, "\n", 1);

    free(copy);
}

void freeLines(char** lines, int linesSize){
    for (int i = 0; i < linesSize; i++){
        memset(lines[i], 0, strlen(lines[i]));
    }
}

