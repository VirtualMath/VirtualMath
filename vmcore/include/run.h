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

typedef ResultType (*VarInfo)(wchar_t **name, int *times, FUNC);

ResultType globalIterStatement(Result *result, Inter *inter, Statement *st, bool p_clock);
bool operationSafeInterStatement(FUNC);
bool ifBranchSafeInterStatement(FUNC);
bool functionSafeInterStatement(FUNC);
bool includeSafeInterStatement(FUNC);
bool blockSafeInterStatement(FUNC);
bool cycleBranchSafeInterStatement(FUNC);
bool withBranchSafeInterStatement(FUNC);
bool tryBranchSafeInterStatement(FUNC);
Statement *checkLabel(Statement *base, wchar_t *label);

ResultType operationStatement(FUNC);
ResultType setClass(FUNC);
ResultType setFunction(FUNC);
ResultType setLambda(FUNC);
ResultType callBack(FUNC);
ResultType elementSlice(FUNC);

ResultType callBackCore(LinkValue *function_value, Argument *arg, fline line, char *file, int pt_sep, FUNC_NT);
ResultType callBackCorePt(LinkValue *function_value, Parameter *pt, long line, char *file, FUNC_NT);

ResultType setDecoration(DecorationStatement *ds, LinkValue *value, FUNC_NT);
ResultType getVar(FUNC, VarInfo var_info);
ResultType getBaseValue(FUNC);
ResultType getList(FUNC);
ResultType getDict(FUNC);
ResultType setDefault(FUNC);

ResultType ifBranch(FUNC);
ResultType whileBranch(FUNC);
ResultType forBranch(FUNC);
ResultType withBranch(FUNC);
ResultType tryBranch(FUNC);
ResultType breakCycle(FUNC);
ResultType continueCycle(FUNC);
ResultType regoIf(FUNC);
ResultType restartCode(FUNC);
ResultType returnCode(FUNC);
ResultType yieldCode(FUNC);
ResultType raiseCode(FUNC);
ResultType assertCode(FUNC);
ResultType gotoLabel(FUNC);
ResultType runLabel(FUNC);

ResultType includeFile(FUNC);
ResultType importFile(FUNC);
ResultType fromImportFile(FUNC);

ResultType pointAss(Statement *name, LinkValue *value, FUNC_NT);
ResultType listAss(Statement *name, LinkValue *value, FUNC_NT);
ResultType assCore(Statement *name, LinkValue *value, bool check_aut, bool setting, FUNC_NT);
ResultType downAss(Statement *name, LinkValue *value, FUNC_NT);
ResultType varAss(Statement *name, LinkValue *value, bool check_aut, bool setting, FUNC_NT);

ResultType delOperation(FUNC);
ResultType delCore(Statement *name, bool check_aut, FUNC_NT);
ResultType listDel(Statement *name, FUNC_NT);
ResultType varDel(Statement *name, bool check_aut, FUNC_NT);
ResultType pointDel(Statement *name, FUNC_NT);
ResultType downDel(Statement *name, FUNC_NT);
bool is_quitExc(LinkValue *value, Inter *inter);
#endif //VIRTUALMATH_RUN_H
