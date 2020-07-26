#ifndef VIRTUALMATH___VIRTUALMATH_H
#define VIRTUALMATH___VIRTUALMATH_H

#define PRIVATE_INCLUDE false
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
#undef PRIVATE_INCLUDE

struct Args{
    char *file;
    char *log_file;
    int level;
} args;

#endif //VIRTUALMATH___VIRTUALMATH_H
