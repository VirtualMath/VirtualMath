#ifndef VIRTUALMATH___GRAMMAR_H
#define VIRTUALMATH___GRAMMAR_H
#include "__virtualmath.h"

#if OUT_LOG && OUT_PASERS_LOG
#define doubleLog(pm, grammar_level, pasers_level, message, ...) do{ \
writeLog(pm->grammar_debug, grammar_level, message, __VA_ARGS__); \
writeLog(pm->paser_debug, pasers_level, "\n"message, __VA_ARGS__); \
} while(0)
#else
#define doubleLog(...) PASS
#endif

#if OUT_LOG && OUT_GRAMMER_LOG
#define writeLog_(...) writeLog(__VA_ARGS__)
#else
#define writeLog_(...) PASS
#endif

#define readBackToken(status, pm) do{ \
doubleLog(pm, GRAMMAR_DEBUG, DEBUG, "token operation number : %d\n", pm->count); \
pm->count ++; \
status = safeGetToken(pm->tm, pm->paser_debug); \
if (status == -2){ \
syntaxError(pm, "lexical make some error", lexical_error); \
} \
backToken(pm->tm->ts, pm->paser_debug); \
} while(0) /*预读token*/

#define popAheadToken(token_var, pm) do{ \
doubleLog(pm, GRAMMAR_DEBUG, DEBUG, "token operation number : %d\n", pm->count); \
pm->count ++; \
safeGetToken(pm->tm, pm->paser_debug); \
/* 执行popheanToken之前执行readBackToken因此不必再检查status */ \
token_var = popToken(pm->tm->ts, pm->paser_debug); \
} while(0) /*弹出预读的token*/

#define addStatementToken(type, st, pm) do{\
Token *tmp_new_token; \
tmp_new_token = makeStatementToken(type, st); \
addToken(pm->tm->ts, tmp_new_token, pm->paser_debug); \
backToken(pm->tm->ts, pm->paser_debug); \
} while(0)

#define backToken_(pm, token) do{ \
addToken(pm->tm->ts, (token), pm->paser_debug); \
backToken(pm->tm->ts, pm->paser_debug); \
}while(0)

#define addToken_ backToken_

#define call_success(pm) (pm->status == success)

#define delToken(pm) do{ \
Token *tmp_token; \
popAheadToken(tmp_token, pm); \
freeToken(tmp_token, true, false); \
tmp_token = NULL; \
}while(0)

#define callChild(pm, status, token, call, type, error_) do{ \
call(CALLPASERSSIGNATURE); \
if (!call_success(pm)){ \
goto error_; \
} \
readBackToken(status, pm); \
if (status != type){ \
goto error_; \
} \
popAheadToken(token, pm); \
}while(0)

void parserCommand(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void parserPolynomial(PASERSSIGNATURE);
void parserBaseValue(PASERSSIGNATURE);
void parserFactor(PASERSSIGNATURE);
void parserAssignment(PASERSSIGNATURE);
void syntaxError(ParserMessage *pm, char *message, int status);
void twoOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*getSymbol)(PASERSSIGNATURE, int symbol, Statement **st), int, int, char *, char *);

#endif //VIRTUALMATH___GRAMMAR_H
