#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H

#include "__virtualmath.h"

#if OUT_INTER_LOG && OUT_INTER_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define printResult(...)
#define writeLog_(...)
#endif

#endif //VIRTUALMATH___RUN_H
