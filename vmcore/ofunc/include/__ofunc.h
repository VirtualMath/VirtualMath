#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"
#include "__base.h"
#include "__run.h"

LinkValue *registeredFunctionCore(OfficialFunction of, wchar_t *name, enum NameFuncVar nfv, FUNC_NT);

bool iterNameFunc(NameFunc *list, FUNC_NT);
bool iterClassFunc(NameFunc *list, FUNC_NT);

void iterBaseNameFunc(NameFunc *list, struct LinkValue *belong, FUNC_CORE);
void iterBaseClassFunc(NameFunc *list, LinkValue *belong, FUNC_CORE);
LinkValue *makeBaseChildClass(LinkValue *inherit, Inter *inter);

bool checkIndex(vint *index, const vint *size, FUNC_NT);
bool checkSlice(vint *first, vint *second, const vint *stride, vint size, FUNC_NT);
void addBaseClassVar(wchar_t *name, LinkValue *obj, LinkValue *belong, Inter *inter);
#endif //VIRTUALMATH___OFUNC_H
