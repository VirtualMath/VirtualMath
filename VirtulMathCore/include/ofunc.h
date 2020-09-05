#ifndef VIRTUALMATH_OFUNC_H
#define VIRTUALMATH_OFUNC_H
#include "__macro.h"
#include "io.h"
#include "object.h"
#include "vobject.h"
#include "sys.h"
#include "num.h"
#include "str.h"
#include "bool.h"
#include "pass.h"
#include "list.h"
#include "dict.h"
#include "function.h"
#include "listiter.h"
#include "dictiter.h"
#include "error_.h"

struct Argument;
struct VarList;
struct Inherit;
struct Inter;

struct NameFunc{
    char *name;
    OfficialFunction of;
    enum FunctionPtType type;
};
typedef struct NameFunc NameFunc;

void registeredBaseFunction(struct LinkValue *father, Inter *inter);
void registeredFunctionName(Inter *inter, LinkValue *belong);
#endif //VIRTUALMATH_OFUNC_H
