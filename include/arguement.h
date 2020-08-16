#ifndef VIRTUALMATH_ARGUEMENT_H
#define VIRTUALMATH_ARGUEMENT_H

struct Args{
    char *log_file;
    bool stdout_inter;
} args;

typedef struct Args Args;

int getArgs(int argc, char **argv);
void freeArgs(void);

#endif //VIRTUALMATH_ARGUEMENT_H
