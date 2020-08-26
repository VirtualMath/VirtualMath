#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"
#include "__base.h"
#include "__run.h"

LinkValue *registeredFunctionCore(OfficialFunction of, char *name, struct LinkValue *belong, INTER_FUNCTIONSIG_CORE);
void iterNameFunc(NameFunc list[],struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void iterClassFunc(NameFunc list[], LinkValue *father, INTER_FUNCTIONSIG_CORE);
Value *makeBaseChildClass(Value *inherit, Inter *inter);
bool checkIndex(vnum *index, const vnum *size, INTER_FUNCTIONSIG_NOT_ST);
bool checkSlice(vnum *first, vnum *second, const vnum *stride, vnum size, INTER_FUNCTIONSIG_NOT_ST);
#endif //VIRTUALMATH___OFUNC_H
