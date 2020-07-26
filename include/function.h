#ifndef VIRTUALMATH_FUNCTION_H
#define VIRTUALMATH_FUNCTION_H
#include "__macro.h"
#include "statement.h"
#include "run.h"

// 所有statement相关的function都在此处声明

// value的处理
Value *makeValue(Inter *inter);
void freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);

void setResult(Result *ru, bool link, Inter *inter);

// Inter的处理
Inter *makeInter();
void freeInter(Inter *inter, bool self);

// statement的处理
Statement *makeStatement();
void connectStatement(Statement *base, Statement *new);
void freeStatement(Statement *st);

// run的处理
Result iterStatement(INTER_FUNCTIONSIG);
Result operationStatement(INTER_FUNCTIONSIG);
Result globalIterStatement(Inter *inter);

// var的处理
VarList *makeVarList(Inter *inter);
VarList *freeVarList(VarList *vl, bool self);
LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times);
void addFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, LinkValue *value);
void freeHashTable(HashTable *ht, Inter *inter);

#endif //VIRTUALMATH_FUNCTION_H
