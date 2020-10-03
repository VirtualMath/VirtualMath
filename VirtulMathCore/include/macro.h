#ifndef VIRTUALMATH_MACRO_H
#define VIRTUALMATH_MACRO_H

#include <stdio.h>
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
#include <ffi.h>

#ifdef __linux__
#include <dlfcn.h>
#else
#include <windows.h>
#include "win/dlfcn_win32.h"
#endif

// 布尔逻辑的定义
#define bool int
#define true 1
#define false 0
#define NUL ((char)0)

#endif //VIRTUALMATH_MACRO_H
