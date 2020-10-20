#ifndef VIRTUALMATH_MACRO_H
#define VIRTUALMATH_MACRO_H

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <getopt.h>
#include <unistd.h>
#include <dlfcn.h>
#include "ffi.h"



// 布尔逻辑的定义
#define bool int
#define true 1
#define false 0
#define NUL ((char)0)
#define WNUL ((wchar_t)0)

#ifdef NDEBUG
#define errasert(e) ((void)0)
#else
#define errasert(e) __assert_fail (#e, __FILE__, __LINE__, __ASSERT_FUNCTION)
#endif

#endif //VIRTUALMATH_MACRO_H
