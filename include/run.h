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
typedef struct Parameter Parameter;
typedef struct DecorationStatement DecorationStatement;
typedef ResultType (*VarInfo)(char **name, int *times, INTER_FUNCTIONSIG);

ResultType globalIterStatement(Result *result, LinkValue *base_father, Inter *inter, Statement *st);
bool operationSafeInterStatement(INTER_FUNCTIONSIG);
bool ifBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool functionSafeInterStatement(INTER_FUNCTIONSIG);
bool cycleBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool tryBranchSafeInterStatement(INTER_FUNCTIONSIG);
Statement *checkLabel(Statement *base, char *label);

ResultType operationStatement(INTER_FUNCTIONSIG);
ResultType setClass(INTER_FUNCTIONSIG);
ResultType setFunction(INTER_FUNCTIONSIG);
ResultType setLambda(INTER_FUNCTIONSIG);
ResultType callBack(INTER_FUNCTIONSIG);
ResultType callBackCore(LinkValue *function_value, Parameter *parameter, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType callClass(LinkValue *class_value, Parameter *parameter, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType callFunction(LinkValue *function_value, Parameter *parameter, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST);
ResultType setDecoration(DecorationStatement *ds, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info);
ResultType getBaseValue(INTER_FUNCTIONSIG);
ResultType getList(INTER_FUNCTIONSIG);
ResultType getDict(INTER_FUNCTIONSIG);
ResultType setDefault(INTER_FUNCTIONSIG);

ResultType ifBranch(INTER_FUNCTIONSIG);
ResultType whileBranch(INTER_FUNCTIONSIG);
ResultType withBranch(INTER_FUNCTIONSIG);
ResultType tryBranch(INTER_FUNCTIONSIG);
ResultType breakCycle(INTER_FUNCTIONSIG);
ResultType continueCycle(INTER_FUNCTIONSIG);
ResultType regoIf(INTER_FUNCTIONSIG);
ResultType restartCode(INTER_FUNCTIONSIG);
ResultType returnCode(INTER_FUNCTIONSIG);
ResultType raiseCode(INTER_FUNCTIONSIG);
ResultType assertCode(INTER_FUNCTIONSIG);
ResultType gotoLabel(INTER_FUNCTIONSIG);
ResultType runLabel(INTER_FUNCTIONSIG);

ResultType includeFile(INTER_FUNCTIONSIG);
ResultType importFile(INTER_FUNCTIONSIG);
ResultType fromImportFile(INTER_FUNCTIONSIG);

ResultType pointAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);

char *setStrVarName(char *old, bool free_old, INTER_FUNCTIONSIG_CORE);
char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE);
char *getNameFromValue(Value *value, INTER_FUNCTIONSIG_CORE);
ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG);
ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG);

#endif //VIRTUALMATH_RUN_H
