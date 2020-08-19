#ifndef VIRTUALMATH_OFUNC_H
#define VIRTUALMATH_OFUNC_H
#include "__macro.h"
#include "io.h"
#include "object.h"
#include "sys.h"

struct Argument;
struct VarList;
struct FatherValue;

struct NameFunc{
    char *name;
    OfficialFunction of;
    enum FunctionPtType type;
};
typedef struct NameFunc NameFunc;

void registeredBaseFunction(struct LinkValue *father, Inter *inter);

#endif //VIRTUALMATH_OFUNC_H
