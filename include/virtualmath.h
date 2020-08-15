#ifndef VIRTUALMATH_VIRTUALMATH_H
#define VIRTUALMATH_VIRTUALMATH_H
#include "macro.h"
#include "arguement.h"

extern jmp_buf memVirtualMath_Env;
extern bool memVirtualMathUseJmp;
typedef struct Inter Inter;
typedef struct Result Result;
Inter *runBaseInter(char *code_file, char *debug_dir, int *status);
void freeInter(Inter *inter, bool self, bool show_gc);
#endif //VIRTUALMATH_VIRTUALMATH_H
