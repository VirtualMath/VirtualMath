#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"
#include "__base.h"
#include "__run.h"

void newObjectSettingPresetting(LinkValue *func, LinkValue *name, Inter *inter);
LinkValue *registeredFunctionCore(OfficialFunction of, wchar_t *name, FUNC_NT);

bool iterNameFunc(NameFunc *list, FUNC_NT);
bool iterClassFunc(NameFunc *list, FUNC_NT);

void iterBaseNameFunc(NameFunc *list, struct LinkValue *belong, FUNC_CORE);
void iterBaseClassFunc(NameFunc *list, LinkValue *belong, FUNC_CORE);
LinkValue *makeBaseChildClass(LinkValue *inherit, Inter *inter);

bool checkIndex(vnum *index, const vnum *size, FUNC_NT);
bool checkSlice(vnum *first, vnum *second, const vnum *stride, vnum size, FUNC_NT);
void addBaseClassVar(wchar_t *name, LinkValue *obj, LinkValue *belong, Inter *inter);
#endif //VIRTUALMATH___OFUNC_H
