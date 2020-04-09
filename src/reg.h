#define LIMITER 1000000

void write_create(char** cmd);

void write_exit(int exit_code);

long set_initialtime();

void set_lasttime();

void send_pipe(char* sent);

void receive_pipe(char* received);

void entry(long size, int B, int Bsize, char* path);
