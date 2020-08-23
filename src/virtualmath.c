#include "__virtualmath.h"

ResultType runCodeBlock(char *code_file, Inter *inter) {
    Statement *pst = NULL;
    ResultType type = error_return;
    if (checkFile(code_file) != 1)
        return error_return;
    runParser(code_file, inter, false, &pst);
    if (pst != NULL) {
        type = runCode(pst, inter);
        freeStatement(pst);
    }
    return type;
}

ResultType runCodeStdin(Inter *inter) {
    Statement *pst = NULL;
    ResultType type = error_return;
    fprintf(stdout, ">>> ");
    runParser(NULL, inter, true, &pst);
    if (pst != NULL) {
        type = runCode(pst, inter);
        freeStatement(pst);
    }
    return type;
}

void runParser(char *code_file, Inter *inter, bool is_one, Statement **st) {
    ParserMessage *pm = makeParserMessage(code_file);
    *st = makeStatement(0, (code_file == NULL) ? "stdin" : code_file);
    parserCommandList(pm, inter, true, is_one, *st);
    if (pm->status != success) {
        fprintf(stderr, "Syntax Error: %s\n", pm->status_message);
        freeStatement(*st);
        *st = NULL;
    }
    freeParserMessage(pm, true);
}

ResultType runCode(Statement *st, Inter *inter) {
    Result result;
    ResultType type;
    setResultCore(&result);
    type = globalIterStatement(&result, inter, st);
    if (type == error_return)
        printError(&result, inter, true);
    freeResult(&result);
    return type;
}