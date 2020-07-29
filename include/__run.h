#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H

#include "__virtualmath.h"

#if OUT_INTER_LOG && OUT_INTER_LOG
#define printResult(result, first, last, debug) do{ \
switch (result.value->value->type){ \
    case number: \
        writeLog(debug, INFO, "%s%ld%s\n", first, result.value->value->data.num.num, last); \
        break; \
    case string: \
        writeLog(debug, INFO, "%s%s%s\n", first, result.value->value->data.str.str, last); \
        break; \
    case function: \
        writeLog(debug, INFO, "%sfunction on %x%s\n", first, (int)result.value->value, last); \
        break; \
    case none: \
        writeLog(debug, INFO, "%sNone%s\n", first, last); \
        break; \
    default: \
        writeLog(debug, INFO, "%sdefault on %x%s\n", first, (int)result.value->value, last); \
        break; \
} \
} while(0)
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define printResult(...)
#define writeLog_(...)
#endif

#endif //VIRTUALMATH___RUN_H