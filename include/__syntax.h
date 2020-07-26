#ifndef VIRTUALMATH___SYNTAX_H
#define VIRTUALMATH___SYNTAX_H
#include "__virtualmath.h"

#if OUT_LOG && OUT_TOKEN_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define writeLog_(...) PASS
#undef printTokenEnter
#define printTokenEnter(...) PASS
#endif
#endif //VIRTUALMATH___SYNTAX_H
