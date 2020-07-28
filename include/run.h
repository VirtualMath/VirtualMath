#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "__macro.h"
#include "value.h"
#include "var.h"

Result iterStatement(INTER_FUNCTIONSIG);
Result operationStatement(INTER_FUNCTIONSIG);
Result assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_CORE);
Result globalIterStatement(Inter *inter);
Result setFunction(INTER_FUNCTIONSIG);
Result callFunction(INTER_FUNCTIONSIG);
Result getBaseVar(INTER_FUNCTIONSIG);
Result getBaseValue(INTER_FUNCTIONSIG);

#endif //VIRTUALMATH_RUN_H
