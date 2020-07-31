#ifndef VIRTUALMATH___VIRTUALMATH_H
#define VIRTUALMATH___VIRTUALMATH_H

#include "__macro.h"
#include "mem.h"
#include "statement.h"
#include "run.h"
#include "syntax.h"
#include "lexical.h"
#include "token.h"
#include "grammar.h"
#include "var.h"
#include "value.h"
#include "inter.h"
#include "log.h"
#include "parameter.h"

struct Args{
    char *file;
    char *log_file;
    int level;
    bool stdout_inter;
} args;

#endif //VIRTUALMATH___VIRTUALMATH_H
