#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H
#include "__virtualmath.h"

#if OUT_INTER_LOG && OUT_INTER_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define printResult(...)
#define writeLog_(...)
#endif

char *setStrVarName(char *old, bool free_old, struct Inter *inter);
char *setNumVarName(NUMBER_TYPE num, struct Inter *inter);
char *getNameFromValue(Value *value, struct Inter *inter);
ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG);

Statement *getRunInfoStatement(Statement *funtion_st);
bool popStatementVarList(Statement *funtion_st, VarList **function_var, VarList *out_var, Inter *inter);

void newFunctionYield(Statement *funtion_st, Statement *node, VarList *new_var, Inter *inter);
void updateFunctionYield(Statement *function_st, Statement *node);

void updateBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, enum StatementInfoStatus status);
void newWithBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *value, LinkValue *_exit_, LinkValue *_enter_);
void newForBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                       Inter *inter, LinkValue *iter);

ResultType setFunctionArgument(struct Argument **arg, LinkValue *function_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);
void freeFunctionArgument(Argument *arg, Argument *base);
LinkValue *findStrVar(char *name, bool free_old, INTER_FUNCTIONSIG_CORE);
LinkValue *checkStrVar(char *name, bool free_old, INTER_FUNCTIONSIG_CORE);
void addStrVar(char *name, bool free_old, LinkValue *value, LinkValue *father, INTER_FUNCTIONSIG_CORE);
LinkValue *findAttributes(char *name, bool free_old, LinkValue *value, Inter *inter);
void addAttributes(char *name, bool free_old, LinkValue *value, LinkValue *belong, Inter *inter);
ResultType elementDownOne(LinkValue *element, LinkValue *index, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType getIter(LinkValue *value, int status, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST);
#endif //VIRTUALMATH___RUN_H
