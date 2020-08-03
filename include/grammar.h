#ifndef VIRTUALMATH_GRAMMAR_H
#define VIRTUALMATH_GRAMMAR_H
#include "__macro.h"

struct ParserMessage{
    struct TokenMessage *tm;
    FILE *paser_debug;
    FILE *grammar_debug;
    int count;
    enum ParserMessageStatus{
        success = 1,
        syntax_error,
        command_list_error,
        lexical_error,
    } status;
    char *status_message;
};

typedef struct ParserMessage ParserMessage;

ParserMessage *makeParserMessage(char *file_dir, char *debug);
void freeParserMessage(ParserMessage *pm, bool self);
void parserCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st);

#endif //VIRTUALMATH_GRAMMAR_H
