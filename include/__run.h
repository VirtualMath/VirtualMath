#ifndef VIRTUALMATH___RUN_H
#define VIRTUALMATH___RUN_H

#include "__virtualmath.h"

#if OUT_INTER_LOG && OUT_INTER_LOG
#define printResult(result, first, last, debug) do{ \
switch (result.value->value->type){ \
    case number: \
        writeLog(debug, INFO, first"%ld"last"\n", result.value->value->data.num.num); \
        break; \
    case string: \
        writeLog(debug, INFO, first"%s"last"\n", result.value->value->data.str.str); \
        break; \
    case function: \
        writeLog(debug, INFO, "%sfunction on %lx%s\n", first, (unsigned long )result.value->value, last); \
        break; \
    case none: \
        writeLog(debug, INFO, "%sNone%s\n", first, last); \
        break; \
    default: \
        writeLog(debug, INFO, "%sdefault on %lx%s\n", first, (unsigned long )result.value->value, last); \
        break; \
} \
} while(0)
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define printResult(...)
#define writeLog_(...)
#endif

#endif //VIRTUALMATH___RUN_H
