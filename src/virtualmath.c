#include "hellovm.h"

void runCodeFile(Inter *inter, char *file[]) {
    Statement *pst = NULL;
    Result result;
    bool should_break = false;
    setResultCore(&result);
    for (PASS; !should_break && *file != NULL; file++) {
        int status;
        if ((status = checkFileReadble((*file))) == 3)
            continue;
        else if (status == 2) {
            *file = memStrcat(*file, ((*file)[memStrlen(*file) - 1] != SEP_CH ? SEP"__main__.vm" : "__main__.vm"), false, false);
            if (checkFileReadble(*file) != 1)
                continue;
        }
        if (runParser(*file, inter, false, &pst)) {
            globalIterStatement(&result, inter, pst);
            if (result.type == R_error) {
                printError(&result, inter, true);
                should_break = true;
            }
            freeStatement(pst);
            freeResult(&result);
        }
    }
}

void runCodeStdin(Inter *inter, char *hello_string) {
    Statement *pst = NULL;
    Result result;
    bool should_break = false;
    setResultCore(&result);
    if (hello_string != NULL)
        printf("%s", hello_string);

    while (!should_break){
        if (ferror(stdin) || feof(stdin))
            clearerr(stdin);
        fprintf(stdout, ">>> ");
        if (runParser(NULL, inter, true, &pst)) {
            globalIterStatement(&result, inter, pst);
            if (result.type == R_error && !(should_break = is_quitExc(result.value, inter)))
                printError(&result, inter, true);
            freeStatement(pst);
            freeResult(&result);
        }
    }
}

bool runParser(char *code_file, Inter *inter, bool is_one, Statement **st) {
    ParserMessage *pm = makeParserMessageFile(code_file, is_one);
    *st = makeStatement(0, (code_file == NULL) ? "stdin" : code_file);
    parserCommandList(pm, inter, true, *st);
    if (pm->status == int_error) {
        fprintf(stderr, "KeyInterrupt\n");
    } else if (pm->status != success)
        fprintf(stderr, "Syntax Error: %s\n", pm->status_message);
    else {
        freeParserMessage(pm, true);
        return true;
    }

    freeStatement(*st);
    freeParserMessage(pm, true);
    *st = NULL;
    return false;
}