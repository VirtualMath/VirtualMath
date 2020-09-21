#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "__macro.h"

typedef struct Result Result;
typedef enum ResultType ResultType;
typedef struct LinkValue LinkValue;
typedef struct Value Value;
typedef struct Statement Statement;
typedef struct StatementList StatementList;
typedef struct Inter Inter;
typedef struct VarList VarList;
typedef struct Parameter Parameter;
typedef struct Argument Argument;
typedef struct DecorationStatement DecorationStatement;

typedef ResultType (*VarInfo)(char **name, int *times, INTER_FUNCTIONSIG);

ResultType globalIterStatement(Result *result, Inter *inter, Statement *st);
bool operationSafeInterStatement(INTER_FUNCTIONSIG);
bool ifBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool functionSafeInterStatement(INTER_FUNCTIONSIG);
bool blockSafeInterStatement(INTER_FUNCTIONSIG);
bool cycleBranchSafeInterStatement(INTER_FUNCTIONSIG);
bool tryBranchSafeInterStatement(INTER_FUNCTIONSIG);
Statement *checkLabel(Statement *base, char *label);

ResultType operationStatement(INTER_FUNCTIONSIG);
ResultType setClass(INTER_FUNCTIONSIG);
ResultType setFunction(INTER_FUNCTIONSIG);
ResultType setLambda(INTER_FUNCTIONSIG);
ResultType callBack(INTER_FUNCTIONSIG);
ResultType elementSlice(INTER_FUNCTIONSIG);

ResultType callBackCore(LinkValue *function_value, Argument *arg, fline line, char *file, int pt_sep, INTER_FUNCTIONSIG_NOT_ST);
ResultType callBackCorePt(LinkValue *function_value, Parameter *pt, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);

ResultType setDecoration(DecorationStatement *ds, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info);
ResultType getBaseValue(INTER_FUNCTIONSIG);
ResultType getList(INTER_FUNCTIONSIG);
ResultType getDict(INTER_FUNCTIONSIG);
ResultType setDefault(INTER_FUNCTIONSIG);

ResultType ifBranch(INTER_FUNCTIONSIG);
ResultType whileBranch(INTER_FUNCTIONSIG);
ResultType forBranch(INTER_FUNCTIONSIG);
ResultType withBranch(INTER_FUNCTIONSIG);
ResultType tryBranch(INTER_FUNCTIONSIG);
ResultType breakCycle(INTER_FUNCTIONSIG);
ResultType continueCycle(INTER_FUNCTIONSIG);
ResultType regoIf(INTER_FUNCTIONSIG);
ResultType restartCode(INTER_FUNCTIONSIG);
ResultType returnCode(INTER_FUNCTIONSIG);
ResultType yieldCode(INTER_FUNCTIONSIG);
ResultType raiseCode(INTER_FUNCTIONSIG);
ResultType assertCode(INTER_FUNCTIONSIG);
ResultType gotoLabel(INTER_FUNCTIONSIG);
ResultType runLabel(INTER_FUNCTIONSIG);

ResultType includeFile(INTER_FUNCTIONSIG);
ResultType importFile(INTER_FUNCTIONSIG);
ResultType fromImportFile(INTER_FUNCTIONSIG);

ResultType pointAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType listAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType assCore(Statement *name, LinkValue *value, bool check_aut, bool setting, INTER_FUNCTIONSIG_NOT_ST);
ResultType downAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST);
ResultType varAss(Statement *name, LinkValue *value, bool check_aut, bool setting, INTER_FUNCTIONSIG_NOT_ST);

ResultType delOperation(INTER_FUNCTIONSIG);
ResultType delCore(Statement *name, bool check_aut, INTER_FUNCTIONSIG_NOT_ST);
ResultType listDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST);
ResultType varDel(Statement *name, bool check_aut, INTER_FUNCTIONSIG_NOT_ST);
ResultType pointDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST);
ResultType downDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST);
bool is_quitExc(LinkValue *value, Inter *inter);
#endif //VIRTUALMATH_RUN_H
