#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H
#include "__virtualmath.h"

wchar_t *setStrVarName(wchar_t *old, bool free_old, struct Inter *inter);
wchar_t *setNumVarName(vnum num, struct Inter *inter);
wchar_t *getNameFromValue(Value *value, struct Inter *inter);
ResultType getBaseVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);

bool popYieldVarList(Statement *st, VarList **return_, VarList *out_var, Inter *inter);

ResultType setFunctionArgument(struct Argument **arg, struct Argument **base, LinkValue *_func, fline line, char *file, int pt_sep, INTER_FUNCTIONSIG_NOT_ST);
void freeFunctionArgument(Argument *arg, Argument *base);
LinkValue *findStrVar(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *findStrVarOnly(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE);
LinkValue *checkStrVar(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE);
void addStrVar(wchar_t *name, bool free_old, bool setting, LinkValue *value, fline line, char *file, bool run, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *findAttributes(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, INTER_FUNCTIONSIG_NOT_ST);

bool addAttributes(wchar_t *name, bool free_old, LinkValue *value, fline line, char *file, bool run, INTER_FUNCTIONSIG_NOT_ST);
void newObjectSetting(LinkValue *name, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType getElement(LinkValue *from, LinkValue *index, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType getIter(LinkValue *value, int status, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool checkBool(LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
wchar_t *getRepoStr(LinkValue *value, bool is_repo, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool is_iterStop(LinkValue *value, Inter *inter);
bool is_indexException(LinkValue *value, Inter *inter);
bool checkAut(enum ValueAuthority value, enum ValueAuthority base, fline line, char *file, char *name, bool pri_auto, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *make_new(Inter *inter, LinkValue *belong, LinkValue *class);
int run_init(LinkValue *obj, Argument *arg, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool setBoolAttrible(bool value, wchar_t *var, fline line, char *file, LinkValue *obj, INTER_FUNCTIONSIG_NOT_ST);
bool runVarFunc(LinkValue *var, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool setVarFunc(LinkValue *var, LinkValue *new, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
#endif //VIRTUALMATH___RUN_H
