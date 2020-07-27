#ifndef VIRTUALMATH_LOG_H
#define VIRTUALMATH_LOG_H
#include "__macro.h"

#define DEEP_DEBUG -3
#define LEXICAL_CHECKOUT_DEBUG -3
#define LEXICAL_DEBUG -2
#define GRAMMAR_DEBUG -1
#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3

#ifndef OUT_INTER_LOG
#define OUT_INTER_LOG true
#endif

#ifndef OUT_TOKEN_LOG
#define OUT_TOKEN_LOG true
#endif

#ifndef OUT_PASERS_LOG
#define OUT_PASERS_LOG true
#endif

#ifndef OUT_GRAMMER_LOG
#define OUT_GRAMMER_LOG true
#endif

#ifndef OUT_LOG
#define OUT_LOG true
#endif

#if OUT_LOG
#define writeLog(file, info_level, message, ...) do{ \
if (file == NULL || info_level < args.level){ \
break; \
} \
else{ \
fprintf(file, message, __VA_ARGS__); \
} \
} while(0)

#else
/* 不输出日志 */
#define writeLog(...) PASS
#define doubleLog(...) PASS
#endif

#ifdef __unix__
#define GRAMMAR_LOG "/grammar.log"
#define PASERS_LOG "/pasers.log"
#define LEXICAL_LOG "/lexical.log"
#define INTER_LOG "/inter.log"
#else
#define GRAMMAR_LOG "\grammar.log"
#define PASERS_LOG "\pasers.log"
#define LEXICAL_LOG "\lexical.log"
#define INTER_LOG "\inter.log"
#endif
#endif //VIRTUALMATH_LOG_H
