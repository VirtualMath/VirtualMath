#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"
#include "__base.h"
#include "__run.h"

void newObjectSettingPresetting(LinkValue *func, LinkValue *name, Inter *inter);
LinkValue *registeredFunctionCore(OfficialFunction of, char *name, INTER_FUNCTIONSIG_NOT_ST);

bool iterNameFunc(NameFunc *list, INTER_FUNCTIONSIG_NOT_ST);
bool iterClassFunc(NameFunc *list, INTER_FUNCTIONSIG_NOT_ST);

void iterBaseNameFunc(NameFunc *list, struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void iterBaseClassFunc(NameFunc *list, LinkValue *father, INTER_FUNCTIONSIG_CORE);
Value *makeBaseChildClass(Value *inherit, Inter *inter);
bool checkIndex(vnum *index, const vnum *size, INTER_FUNCTIONSIG_NOT_ST);
bool checkSlice(vnum *first, vnum *second, const vnum *stride, vnum size, INTER_FUNCTIONSIG_NOT_ST);
void addBaseClassVar(char *name, LinkValue *obj, LinkValue *belong, Inter *inter);
#endif //VIRTUALMATH___OFUNC_H
