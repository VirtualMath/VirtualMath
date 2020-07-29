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

// TODO-szh 优化token操作, 减少内存操作
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
}while(0)

#define checkToken(pm, type, error_) do{ \
int token = readBackToken(pm); \
if (token != type){ \
goto error_; \
} \
delToken(pm); \
}while(0)

// pasersCommand专属macro
#define commandCallBack(pm, st, call, type, return_) do{ \
writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: call "#call"\n", NULL); \
Token *tmp_token = NULL; \
call(CALLPASERSSIGNATURE); \
if (!call_success(pm) || readBackToken(pm) != type) \
goto return_; \
popAheadToken(tmp_token, pm); \
st = tmp_token->data.st; \
freeToken(tmp_token, true, false); \
} while(0)

#define commandCallControl(pm, st, call, type, return_) do{ \
writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: call pasers"#call"\n", NULL); \
Token *tmp_token = NULL; \
parserControl(CALLPASERSSIGNATURE, call, type); \
if (!call_success(pm) || readBackToken(pm) != type) \
goto return_; \
popAheadToken(tmp_token, pm); \
st = tmp_token->data.st; \
freeToken(tmp_token, true, false); \
} while(0)

void parserCommand(PASERSSIGNATURE);
void parserControl(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type);
void parserDef(PASERSSIGNATURE);
void parserIf(PASERSSIGNATURE);
void parserWhile(PASERSSIGNATURE);
void parserCode(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void parserPolynomial(PASERSSIGNATURE);
void parserBaseValue(PASERSSIGNATURE);
void parserCallBack(PASERSSIGNATURE);
void parserFactor(PASERSSIGNATURE);
void parserAssignment(PASERSSIGNATURE);
void twoOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*getSymbol)(PASERSSIGNATURE, int symbol, Statement **st), int, int, char *, char *);
void tailOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*tailFunction)(PASERSSIGNATURE, Token *left_token,  Statement **st), int , int , char *, char *);

void syntaxError(ParserMessage *pm, char *message, int status);
int readBackToken(ParserMessage *pm);

#endif //VIRTUALMATH___GRAMMAR_H
