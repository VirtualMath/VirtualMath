#ifndef VIRTUALMATH_ARGUEMENT_H
#define VIRTUALMATH_ARGUEMENT_H

extern char *HelloString;
struct Args{
    char *error_file;
    char *out_file;
    char *in_file;
    bool run_commandLine;
} args;

typedef struct Args Args;

int getArgs(int argc, char **argv);
void freeArgs(void);

#endif //VIRTUALMATH_ARGUEMENT_H
