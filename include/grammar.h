#ifndef VIRTUALMATH_GRAMMAR_H
#define VIRTUALMATH_GRAMMAR_H
#include "__macro.h"

#define PASERSSIGNATURE ParserMessage *pm, Inter *inter /*pasers函数的统一签名*/
#define CALLPASERSSIGNATURE pm, inter /*pasers函数调用的统一实参*/

typedef struct ParserMessage{
    struct TokenMessage *tm;
    enum {
        success = 1,
        syntax_error,
        command_list_error,
    } status;
    char *status_message;
} ParserMessage;

ParserMessage *makeParserMessage(char *file_dir);
void freePasersMessage(ParserMessage *pm, bool self);
void pasersCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st);
#endif //VIRTUALMATH_GRAMMAR_H
