#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H
#include "__virtualmath.h"

wchar_t *setStrVarName(wchar_t *old, bool free_old, struct Inter *inter);
wchar_t *setIntVarName(vint num, struct Inter *inter);
wchar_t *setDouVarName(vdou num, struct Inter *inter);
wchar_t *getNameFromValue(Value *value, struct Inter *inter);
ResultType getBaseVarInfo(wchar_t **name, int *times, FUNC);
ResultType getBaseSVarInfo(wchar_t **name, int *times, FUNC);
ResultType getVarInfo(wchar_t **name, int *times, FUNC);

bool popYieldVarList(Statement *st, VarList **return_, VarList *out_var, Inter *inter);

ResultType setFunctionArgument(struct Argument **arg, struct Argument **base, LinkValue *_func, fline line, char *file, int pt_sep, FUNC_NT);
void freeFunctionArgument(Argument *arg, Argument *base);
LinkValue *findStrVar(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, FUNC_NT);
LinkValue *findStrVarOnly(wchar_t *name, bool free_old, FUNC_CORE);
LinkValue *checkStrVar(wchar_t *name, bool free_old, FUNC_CORE);
void addStrVar(wchar_t *name, bool free_old, bool setting, LinkValue *value, fline line, char *file, bool run, FUNC_NT);
LinkValue *findAttributes(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, FUNC_NT);

bool addAttributes(wchar_t *name, bool free_old, LinkValue *value, fline line, char *file, bool run, FUNC_NT);
void newObjectSetting(LinkValue *name, fline line, char *file, FUNC_NT);
ResultType getElement(LinkValue *from, LinkValue *index, fline line, char *file, FUNC_NT);
ResultType getIter(LinkValue *value, int status, fline line, char *file, FUNC_NT);
bool checkBool(LinkValue *value, fline line, char *file, FUNC_NT);
wchar_t *getRepoStr(LinkValue *value, bool is_repo, fline line, char *file, FUNC_NT);
bool is_iterStop(LinkValue *value, Inter *inter);
bool is_indexException(LinkValue *value, Inter *inter);
bool checkAut(enum ValueAuthority value, enum ValueAuthority base, fline line, char *file, char *name, bool pri_auto, FUNC_NT);
LinkValue *make_new(Inter *inter, LinkValue *belong, LinkValue *class);
int run_init(LinkValue *obj, Argument *arg, fline line, char *file, FUNC_NT);
bool setBoolAttrible(bool value, wchar_t *var, fline line, char *file, LinkValue *obj, FUNC_NT);
bool runVarFunc(LinkValue *var, fline line, char *file, FUNC_NT);
bool setVarFunc(LinkValue *var, LinkValue *new, fline line, char *file, FUNC_NT);
#endif //VIRTUALMATH___RUN_H
