#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "__macro.h"
#include "value.h"
#include "var.h"

Result iterStatement(INTER_FUNCTIONSIG);
Result operationStatement(INTER_FUNCTIONSIG);
Result globalIterStatement(Inter *inter);

#endif //VIRTUALMATH_RUN_H
