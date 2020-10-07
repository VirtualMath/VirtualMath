#ifndef VIRTUALMATH_GRAMMAR_H
#define VIRTUALMATH_GRAMMAR_H
#include "__macro.h"

struct ParserMessage{
    struct TokenMessage *tm;
    enum ParserMessageStatus{
        success = 1,
        syntax_error,
        command_list_error,
        lexical_error,
        int_error,
    } status;
    char *status_message;
    char *file;
};

typedef struct ParserMessage ParserMessage;

ParserMessage *makeParserMessageFile(char *file_dir);
ParserMessage *makeParserMessageStr(wchar_t *str);
void freeParserMessage(ParserMessage *pm, bool self);
void parserCommandList(ParserMessage *pm, Inter *inter, bool global, bool is_one, Statement *st);

#endif //VIRTUALMATH_GRAMMAR_H
