#ifndef VIRTUALMATH_OFUNC_H
#define VIRTUALMATH_OFUNC_H
#include "__macro.h"
#include "io.h"

struct Argument;
struct VarList;
typedef enum ResultType (*OfficialFunction)(OfficialFunctionSig);
typedef void (*Registered)(RegisteredFunctionSig);

struct NameFunc{
    char *name;
    OfficialFunction of;
};
typedef struct NameFunc NameFunc;

void registeredFunctionCore(OfficialFunction of, char *name, struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void iterNameFunc(NameFunc list[],struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void registeredBaseFunction(struct LinkValue *father, Inter *inter);

#endif //VIRTUALMATH_OFUNC_H
