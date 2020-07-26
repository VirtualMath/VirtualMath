#ifndef VIRTUALMATH___TOKEN_H
#define VIRTUALMATH___TOKEN_H

#include "__virtualmath.h"

#if OUT_LOG && OUT_TOKEN_LOG
#define MACRO_printTokenStream(...) printTokenStream(__VA_ARGS__)
#else
#define MACRO_printTokenStream(...) PASS
#undef printTokenEnter
#define printTokenEnter(...) PASS
#endif

#if OUT_LOG && OUT_PASERS_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define writeLog_(...) PASS
#endif

#endif //VIRTUALMATH___TOKEN_H
