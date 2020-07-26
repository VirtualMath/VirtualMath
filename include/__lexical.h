#ifndef VIRTUALMATH___LEXICAL_H
#define VIRTUALMATH___LEXICAL_H
#include "__virtualmath.h"

#if OUT_LOG && OUT_TOKEN_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define writeLog_(...) PASS
#endif

#endif //VIRTUALMATH___LEXICAL_H
