#ifndef VIRTUALMATH___VIRTUALMATH_H
#define VIRTUALMATH___VIRTUALMATH_H

#include "__macro.h"
#include "mem.h"
#include "gc.h"
#include "inter.h"
#include "value.h"
#include "var.h"
#include "parameter.h"
#include "statement.h"
#include "run.h"
#include "lexical.h"
#include "token.h"
#include "syntax.h"
#include "grammar.h"
#include "arguement.h"
#include "file.h"

/* DEBUG */
void printLinkValueGC(char *tag, Inter *inter);
void printValueGC(char *tag, Inter *inter);
void printVarGC(char *tag, Inter *inter);
void printHashTableGC(char *tag, Inter *inter);
#endif //VIRTUALMATH___VIRTUALMATH_H
