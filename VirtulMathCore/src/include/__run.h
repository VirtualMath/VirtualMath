#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H
#include "__virtualmath.h"

wchar_t *setStrVarName(wchar_t *old, bool free_old, struct Inter *inter);
wchar_t *setNumVarName(vnum num, struct Inter *inter);
wchar_t *getNameFromValue(Value *value, struct Inter *inter);
ResultType getBaseVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG);

bool popStatementVarList(Statement *funtion_st, VarList **function_var, VarList *out_var, Inter *inter);

void newFunctionYield(Statement *funtion_st, Statement *node, VarList *new_var, Inter *inter);
void updateFunctionYield(Statement *function_st, Statement *node);

void updateBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, enum StatementInfoStatus status);
void newWithBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *value, LinkValue *_exit_, LinkValue *_enter_);
void newForBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                       Inter *inter, LinkValue *iter);

ResultType setFunctionArgument(struct Argument **arg, struct Argument **base, LinkValue *_func, fline line, char *file, int pt_sep, INTER_FUNCTIONSIG_NOT_ST);
void freeFunctionArgument(Argument *arg, Argument *base);
LinkValue *findStrVar(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE);
LinkValue *checkStrVar(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE);
void addStrVar(wchar_t *name, bool free_old, bool setting, LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *findAttributes(wchar_t *name, bool free_old, LinkValue *value, Inter *inter);

bool addAttributes(wchar_t *name, bool free_old, LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
void newObjectSetting(LinkValue *name, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType getElement(LinkValue *from, LinkValue *index, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType getIter(LinkValue *value, int status, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool checkBool(LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
wchar_t *getRepoStr(LinkValue *value, bool is_repo, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
bool is_iterStop(LinkValue *value, Inter *inter);
bool is_indexException(LinkValue *value, Inter *inter);
bool checkAut(enum ValueAuthority value, enum ValueAuthority base, fline line, char *file, char *name, bool pri_auto, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *make_new(Inter *inter, LinkValue *belong, LinkValue *class);
int init_new(LinkValue *obj, Argument *arg, char *message, INTER_FUNCTIONSIG_NOT_ST);
bool setBoolAttrible(bool value, wchar_t *var, fline line, char *file, LinkValue *obj, INTER_FUNCTIONSIG_NOT_ST);
#endif //VIRTUALMATH___RUN_H
