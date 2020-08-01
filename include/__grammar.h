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

#define addStatementToken(type, st, pm) addBackToken(pm->tm->ts, makeStatementToken(type, st), pm->paser_debug)
#define delToken(pm) freeToken(popAheadToken(pm), true, false)
#define backToken_(pm, token) addBackToken(pm->tm->ts, (token), pm->paser_debug)
#define addLexToken(pm, type) backToken_(pm, makeLexToken(type, NULL, NULL))
#define addToken_ backToken_
#define call_success(pm) (pm->status == success)

void parserCommand(PASERSSIGNATURE);
void parserControl(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type);
void parserDef(PASERSSIGNATURE);
void parserIf(PASERSSIGNATURE);
void parserWhile(PASERSSIGNATURE);
void parserTry(PASERSSIGNATURE);
void parserCode(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void parserPolynomial(PASERSSIGNATURE);
void parserBaseValue(PASERSSIGNATURE);
void parserCallBack(PASERSSIGNATURE);
void parserFactor(PASERSSIGNATURE);
void parserAssignment(PASERSSIGNATURE);
void parserTuple(PASERSSIGNATURE);
void twoOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*getSymbol)(PASERSSIGNATURE, int symbol, Statement **st), int, int, char *, char *);
void tailOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*tailFunction)(PASERSSIGNATURE, Token *left_token,  Statement **st), int , int , char *, char *);

void syntaxError(ParserMessage *pm, int status, int num, ...);
int readBackToken(ParserMessage *pm);
Token *popAheadToken(ParserMessage *pm);
bool checkToken_(ParserMessage *pm, int type);
bool commandCallControl_(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type, Statement **st, char *message);
bool commandCallBack_(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int type, Statement **st, char *message);
bool callParserCode(PASERSSIGNATURE, Statement **st,char *message);
bool callParserAs(PASERSSIGNATURE, Statement **st,char *message);
bool callChildStatement(PASERSSIGNATURE, void (*call)(PASERSSIGNATURE), int type, Statement **st, char *message);

bool callChildToken(ParserMessage *pm, Inter *inter, void (*call)(ParserMessage *, Inter *), int type, Token **tmp,
                    char *message, int error_type);

#endif //VIRTUALMATH___GRAMMAR_H
