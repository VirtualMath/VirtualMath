#ifndef VIRTUALMATH_GRAMMAR_H
#define VIRTUALMATH_GRAMMAR_H
#include "run.h"

#define PASERSSIGNATURE parserMessage *pm, Inter *inter /*pasers函数的统一签名*/
#define CALLPASERSSIGNATURE pm, inter /*pasers函数调用的统一实参*/

typedef struct parserMessage{
    struct tokenMessage *tm;
    enum parserMessageStatus{
        success = 1,
        syntax_error,
        command_list_error,
    } status;
    char *status_message;
} parserMessage;

parserMessage *makeParserMessage(char *file_dir);
void freePasersMessage(parserMessage *pm, bool self);
void commandList(parserMessage *pm, Inter *inter, bool global, Statement *st);
#endif //VIRTUALMATH_GRAMMAR_H
