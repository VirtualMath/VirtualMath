#ifndef VIRTUALMATH___GRAMMAR_H
#define VIRTUALMATH___GRAMMAR_H
#include "__virtualmath.h"

#define PASERSSIGNATURE ParserMessage *pm, Inter *inter /*pasers函数的统一签名*/
#define CALLPASERSSIGNATURE pm, inter /*pasers函数调用的统一实参*/

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

typedef void (*PasersFunction)(PASERSSIGNATURE);
typedef int (*GetSymbolFunction)(PASERSSIGNATURE, int, Statement **);
typedef Statement *(*MakeControlFunction)(Statement *);
typedef int (*TailFunction)(PASERSSIGNATURE, Token *, Statement **);

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
void twoOperation(ParserMessage *pm, Inter *inter, PasersFunction callBack, GetSymbolFunction getSymbol,
                  int type, int self_type, char *call_name, char *self_name, bool is_right);
void tailOperation(PASERSSIGNATURE, PasersFunction callBack, TailFunction tailFunction, int type, int self_type,
                   char *call_name, char *self_name);
void syntaxError(ParserMessage *pm, int status, int num, ...);
int readBackToken(ParserMessage *pm);
Token *popAheadToken(ParserMessage *pm);
bool checkToken_(ParserMessage *pm, int type);
bool commandCallControl_(PASERSSIGNATURE, MakeControlFunction callBack, int type, Statement **st, char *message);
bool commandCallBack_(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message);
bool callParserCode(PASERSSIGNATURE, Statement **st,char *message);
bool callParserAs(PASERSSIGNATURE, Statement **st,char *message);
bool callChildStatement(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message);
bool callChildToken(ParserMessage *pm, Inter *inter, PasersFunction callBack, int type, Token **tmp, char *message,
                    int error_type);

#endif //VIRTUALMATH___GRAMMAR_H