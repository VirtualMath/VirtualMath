#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H
#include "__virtualmath.h"

#if OUT_INTER_LOG && OUT_INTER_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define printResult(...)
#define writeLog_(...)
#endif

char *setStrVarName(char *old, bool free_old, INTER_FUNCTIONSIG_CORE);
char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE);
char *getNameFromValue(Value *value, INTER_FUNCTIONSIG_CORE);
ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG);

Statement *getRunInfoStatement(Statement *funtion_st);
bool popStatementVarList(Statement *funtion_st, VarList **function_var, VarList *out_var, Inter *inter);

void newFunctionYield(Statement *funtion_st, Statement *node, VarList *new_var, Inter *inter);
void updateFunctionYield(Statement *function_st, Statement *node);
void freeFunctionYield(Statement *function_st, Inter *inter);

void updateBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, enum StatementInfoStatus status);
void newWithBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *value, LinkValue *_exit_, LinkValue *_enter_);

ResultType setFunctionArgument(struct Argument **arg, LinkValue *function_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);
void freeFunctionArgument(Argument *arg, Argument *base);

#endif //VIRTUALMATH___RUN_H
