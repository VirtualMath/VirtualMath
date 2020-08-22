#ifndef VIRTUALMATH_ARGUEMENT_H
#define VIRTUALMATH_ARGUEMENT_H

struct Args{
    char *error_file;
    char *out_file;
} args;

typedef struct Args Args;

int getArgs(int argc, char **argv);
void freeArgs(void);

#endif //VIRTUALMATH_ARGUEMENT_H
