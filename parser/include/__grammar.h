#ifndef VIRTUALMATH___GRAMMAR_H
#define VIRTUALMATH___GRAMMAR_H
#include "__virtualmath.h"

#define PASERSSIGNATURE ParserMessage *pm, Inter *inter /*pasers函数的统一签名*/
#define CALLPASERSSIGNATURE pm, inter /*pasers函数调用的统一实参*/

#define addStatementToken(type, st, pm) addBackToken(pm->tm->ts, makeStatementToken(type, st))
#define delToken(pm) (freeToken(popNewToken(pm->tm), false))
#define backToken_(pm, token) addBackToken(pm->tm->ts, (token))
#define addLexToken(pm, type) backToken_(pm, makeLexToken(type, NULL, NULL, 0))
#define addToken_ backToken_
#define call_success(pm) (pm->status == success)

typedef void (*PasersFunction)(PASERSSIGNATURE);
typedef int (*GetSymbolFunction)(PASERSSIGNATURE, int, Statement **);
typedef int (*ChecktLeftToken)(PASERSSIGNATURE, Statement *);
typedef Statement *(*MakeControlFunction)(Statement *, long int, char *);
typedef int (*TailFunction)(PASERSSIGNATURE, Token *, Statement **);

void parserCommand(PASERSSIGNATURE);
void parserControl(PASERSSIGNATURE, MakeControlFunction callBack, int type, bool must_operation, char *message);
void parserDef(PASERSSIGNATURE);
void parserDo(PASERSSIGNATURE);
void parserWith(PASERSSIGNATURE);
void parserIf(PASERSSIGNATURE);
void parserWhile(PASERSSIGNATURE);
void parserTry(PASERSSIGNATURE);
void parserCode(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void parserPolynomial(PASERSSIGNATURE);
void parserBaseValue(PASERSSIGNATURE);
void parserCallBack(PASERSSIGNATURE);
void parserPoint(PASERSSIGNATURE);
void parserFactor(PASERSSIGNATURE);
void parserAssignment(PASERSSIGNATURE);
void parserTuple(PASERSSIGNATURE);
void parserImport(PASERSSIGNATURE);
void parserLabel(PASERSSIGNATURE);
void parserGoto(PASERSSIGNATURE);
void parserDecoration(PASERSSIGNATURE);
void parserVarControl(PASERSSIGNATURE);

void syntaxError(ParserMessage *pm, int status,long int line , int num, ...);

int readBackToken(ParserMessage *pm);
bool checkToken(ParserMessage *pm, int type);

bool commandCallControl_(PASERSSIGNATURE, MakeControlFunction callBack, int type, Statement **st,
                         char *log_message, bool must_operation, char *error_message);
bool commandCallBack_(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message);

bool callParserCode(PASERSSIGNATURE, Statement **st, char *message, long int line);
bool callParserAs(PASERSSIGNATURE, Statement **st,char *message);
bool callChildStatement(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message);
bool callChildToken(PASERSSIGNATURE, PasersFunction callBack, int type, Token **tmp, char *message,
                    int error_type);
bool parserParameter(PASERSSIGNATURE, Parameter **pt, bool is_formal, bool is_list, bool is_dict,
                     int sep,int ass);

void twoOperation(PASERSSIGNATURE, PasersFunction callBack, GetSymbolFunction getSymbol, ChecktLeftToken checkleft,
                  int call_type, int self_type, char *call_name, char *self_name, bool is_right);
void tailOperation(PASERSSIGNATURE, PasersFunction callBack, TailFunction tailFunction, int call_type, int self_type,
                   char *call_name, char *self_name);

void lexEnter(ParserMessage *pm, bool lock);

#endif //VIRTUALMATH___GRAMMAR_H
