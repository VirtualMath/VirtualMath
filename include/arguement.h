#ifndef VIRTUALMATH_ARGUEMENT_H
#define VIRTUALMATH_ARGUEMENT_H

struct Args{
    char *file;
    char *log_file;
    int level;
    bool stdout_inter;
} args;

typedef struct Args Args;

int getArgs(int argc, char *argv[]);
void freeArgs();

#endif //VIRTUALMATH_ARGUEMENT_H
