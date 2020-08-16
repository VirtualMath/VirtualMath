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
    } status;
    char *status_message;
    char *file;
};

typedef struct ParserMessage ParserMessage;

ParserMessage *makeParserMessage(char *file_dir);
void freeParserMessage(ParserMessage *pm, bool self);
void parserCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st);
// PASERSSIGNATURE 位于__grammar

#endif //VIRTUALMATH_GRAMMAR_H
