#include "__virtualmath.h"

int runCodeBlock(char *code_file, Inter *inter) {
    Statement *pst = NULL;
    if (checkFile(code_file) != 1)
        return -1;
    runParser(code_file, inter, &pst);
    if (pst != NULL) {
        runCode(pst, inter);
        freeStatement(pst);
    }
    return 0;
}

void runParser(char *code_file, Inter *inter, Statement **st) {
    ParserMessage *pm = makeParserMessage(code_file);
    *st = makeStatement(0, code_file);
    parserCommandList(pm, inter, true, *st);
    if (pm->status != success) {
        fprintf(stderr, "Syntax Error: %s\n", pm->status_message);
        freeStatement(*st);
        *st = NULL;
    }
    freeParserMessage(pm, true);
}

void runCode(Statement *st, Inter *inter) {
    Result result;
    ResultType type;
    setResultCore(&result);
    type = globalIterStatement(&result, NULL, inter, st);
    if (type == error_return)
        printError(&result, inter, true);
    freeResult(&result);
}