#define LIMITER 1000000

void write_create(struct timespec start);

void write_exit(int exit_code, struct timespec start);

void send_pipe(char* sent, struct timespec start);

void receive_pipe(char* received, struct timespec start);

void entry(long size, int B, int Bsize, char* path, struct timespec start);

double set_time(struct timespec start);
