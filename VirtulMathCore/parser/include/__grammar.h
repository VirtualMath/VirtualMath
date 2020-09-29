#ifndef VIRTUALMATH___GRAMMAR_H
#define VIRTUALMATH___GRAMMAR_H
#include "__virtualmath.h"

#define P_FUNC ParserMessage *pm, Inter *inter /*pasers函数的统一签名*/
#define CP_FUNC pm, inter /*pasers函数调用的统一实参*/

#define addStatementToken(type, st, pm) addBackToken(pm->tm->ts, makeStatementToken(type, st))
#define delToken(pm) (freeToken(popNewToken(pm->tm), false))
#define backToken_(pm, token) addBackToken(pm->tm->ts, (token))
#define addLexToken(pm, type) backToken_(pm, makeLexToken(type, NULL, NULL, 0))
#define addToken_ backToken_
#define call_success(pm) (pm->status == success)

typedef void (*PasersFunction)(P_FUNC);
typedef int (*GetSymbolFunction)(P_FUNC, int, Statement **);
typedef int (*ChecktLeftToken)(P_FUNC, Statement *);
typedef Statement *(*MakeControlFunction)(Statement *, fline, char *);

void parserCommand(P_FUNC);
void parserControl(P_FUNC, MakeControlFunction callBack, int type, bool must_operation, char *message);
void parserDef(P_FUNC);
void parserDo(P_FUNC);
void parserFor(P_FUNC);
void parserWith(P_FUNC);
void parserIf(P_FUNC);
void parserWhile(P_FUNC);
void parserTry(P_FUNC);
void parserCode(P_FUNC);
void parserOperation(P_FUNC);
void parserPolynomial(P_FUNC);
void parserBaseValue(P_FUNC);
void parserCallBack(P_FUNC);
void parserFactor(P_FUNC);
void parserAssignment(P_FUNC);
void parserTuple(P_FUNC);
void parserImport(P_FUNC);
void parserLabel(P_FUNC);
void parserGoto(P_FUNC);
void parserDecoration(P_FUNC);
void parserVarControl(P_FUNC);

void syntaxError(ParserMessage *pm, int status,long int line , int num, ...);

int readBackToken(ParserMessage *pm);
bool checkToken(ParserMessage *pm, int type);

bool commandCallControl_(P_FUNC, MakeControlFunction callBack, int type, Statement **st, bool must_operation, char *error_message);

bool callParserCode(P_FUNC, Statement **st, char *message, long int line);
bool callParserAs(P_FUNC, Statement **st, char *message);
bool callChildStatement(P_FUNC, PasersFunction callBack, int type, Statement **st, char *message);
bool callChildToken(P_FUNC, PasersFunction callBack, int type, Token **tmp, char *message,
                    int error_type);

bool parserParameter(P_FUNC, Parameter **pt, bool enter, bool is_formal, bool is_list, bool is_dict,
                     int sep, int ass, int n_sep);

void twoOperation(P_FUNC, PasersFunction callBack, GetSymbolFunction getSymbol, ChecktLeftToken checkleft,
                  int call_type, int self_type, char *call_name, char *self_name, bool is_right);

void lexEnter(ParserMessage *pm, bool lock);

#endif //VIRTUALMATH___GRAMMAR_H
