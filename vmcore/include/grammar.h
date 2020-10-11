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
    bool short_cm;  // 短匹配
};

typedef struct ParserMessage ParserMessage;

ParserMessage *makeParserMessageFile(char *file_dir, bool short_cm);
ParserMessage *makeParserMessageStr(wchar_t *str, bool short_cm);
void freeParserMessage(ParserMessage *pm, bool self);
void parserCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st);

#endif //VIRTUALMATH_GRAMMAR_H
