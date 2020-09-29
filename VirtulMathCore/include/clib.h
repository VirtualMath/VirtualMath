#ifndef VIRTUALMATH_CLIB_H
#define VIRTUALMATH_CLIB_H
#include "__macro.h"

struct Inter;
struct LinkValue;
void importClibCore(char *file, struct LinkValue *belong, FUNC_CORE);
bool checkCLib(char *file);
#endif //VIRTUALMATH_CLIB_H
