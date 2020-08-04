#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "__macro.h"

typedef Result (*VarInfo)(char **name, int *times, INTER_FUNCTIONSIG);

Result globalIterStatement(Inter *inter);
bool operationSafeInterStatement(Result *result, INTER_FUNCTIONSIG);
bool ifBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG);
bool functionSafeInterStatement(Result *result, INTER_FUNCTIONSIG);
bool cycleBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG);
bool tryBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG);

Result operationStatement(INTER_FUNCTIONSIG);
Result setFunction(INTER_FUNCTIONSIG);
Result callFunction(INTER_FUNCTIONSIG);
Result getVar(INTER_FUNCTIONSIG, VarInfo var_info);
Result getBaseValue(INTER_FUNCTIONSIG);
Result getList(INTER_FUNCTIONSIG);
Result getDict(INTER_FUNCTIONSIG);
Result ifBranch(INTER_FUNCTIONSIG);
Result whileBranch(INTER_FUNCTIONSIG);
Result tryBranch(INTER_FUNCTIONSIG);
Result breakCycle(INTER_FUNCTIONSIG);
Result continueCycle(INTER_FUNCTIONSIG);
Result regoIf(INTER_FUNCTIONSIG);
Result restartCode(INTER_FUNCTIONSIG);
Result returnCode(INTER_FUNCTIONSIG);
Result raiseCode(INTER_FUNCTIONSIG);

Result includeFile(INTER_FUNCTIONSIG);

Result assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_CORE);

char *setStrVarName(char *old, bool free_old, INTER_FUNCTIONSIG_CORE);
char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE);
char *getNameFromValue(Value *value, INTER_FUNCTIONSIG_CORE);
Result getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
Result getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
Result getVarInfo(char **name, int *times, INTER_FUNCTIONSIG);

#endif //VIRTUALMATH_RUN_H
