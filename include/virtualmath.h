#ifndef VIRTUALMATH_VIRTUALMATH_H
#define VIRTUALMATH_VIRTUALMATH_H
#include "macro.h"
#include "arguement.h"

typedef struct Inter Inter;
typedef struct Result Result;
struct Inter *runBaseInter(char *code_file, char *debug_dir);
void freeInter(Inter *inter, int self);
#endif //VIRTUALMATH_VIRTUALMATH_H
