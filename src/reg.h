#define LIMITER 999999

void write_create(char** cmd);

void write_exit(int exit_code);

long set_initialtime();

void set_lasttime();

void send_pipe(char* sent);

void receive_pipe(char* received);
