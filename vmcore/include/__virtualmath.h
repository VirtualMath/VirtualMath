#ifndef VIRTUALMATH___VIRTUALMATH_H
#define VIRTUALMATH___VIRTUALMATH_H

#include "__macro.h"

#include "mem.h"
#include "handler.h"
#include "file.h"
#include "md5.h"

#include "gc.h"
#include "value.h"
#include "parameter.h"
#include "statement.h"
#include "var.h"
#include "inter.h"
#include "ofunc.h"
#include "clib.h"
#include "run.h"

#include "lexical.h"
#include "token.h"
#include "syntax.h"
#include "grammar.h"

#if DEBUG
/* DEBUG所使用的函数 */
void printGC(Inter *inter);
void printGCHS(Inter *inter);
void printLinkValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link);
void printValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link);
void printVarGC(char *tag, Inter *inter);
void printHashTableGC(char *tag, Inter *inter, long *tmp_link);
#endif

#endif //VIRTUALMATH___VIRTUALMATH_H
