#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "__macro.h"

typedef struct Result Result;
typedef enum ResultType ResultType;
typedef struct LinkValue LinkValue;
typedef struct Value Value;
typedef struct Statement Statement;
typedef struct Inter Inter;
typedef struct VarList VarList;

typedef ResultType (*VarInfo)(char **name, int *times, INTER_FUNCTIONSIG);

ResultType globalIterStatement(Inter *inter, Result *result);
bool operationSafeInterStatement(INTER_FUNCTIONSIG);
bool ifBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool functionSafeInterStatement(INTER_FUNCTIONSIG);
bool cycleBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool tryBranchSafeInterStatement(INTER_FUNCTIONSIG);

ResultType operationStatement(INTER_FUNCTIONSIG);
ResultType setFunction(INTER_FUNCTIONSIG);
ResultType callFunction(INTER_FUNCTIONSIG);
ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info);
ResultType getBaseValue(INTER_FUNCTIONSIG);
ResultType getList(INTER_FUNCTIONSIG);
ResultType getDict(INTER_FUNCTIONSIG);
ResultType ifBranch(INTER_FUNCTIONSIG);
ResultType whileBranch(INTER_FUNCTIONSIG);
ResultType tryBranch(INTER_FUNCTIONSIG);
ResultType breakCycle(INTER_FUNCTIONSIG);
ResultType continueCycle(INTER_FUNCTIONSIG);
ResultType regoIf(INTER_FUNCTIONSIG);
ResultType restartCode(INTER_FUNCTIONSIG);
ResultType returnCode(INTER_FUNCTIONSIG);
ResultType raiseCode(INTER_FUNCTIONSIG);

ResultType includeFile(INTER_FUNCTIONSIG);

ResultType assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);

char *setStrVarName(char *old, bool free_old, INTER_FUNCTIONSIG_CORE);
char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE);
char *getNameFromValue(Value *value, INTER_FUNCTIONSIG_CORE);
ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG);

#endif //VIRTUALMATH_RUN_H
