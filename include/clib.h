#ifndef VIRTUALMATH_CLIB_H
#define VIRTUALMATH_CLIB_H
#include "__macro.h"

struct Inter;
struct LinkValue;
void runClib(char *file, struct LinkValue *belong, INTER_FUNCTIONSIG_CORE);
bool checkCLib(char *file);
#endif //VIRTUALMATH_CLIB_H
