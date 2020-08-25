#include "__run.h"

void runCodeFile(Inter *inter, char *file[]) {
    Statement *pst = NULL;
    Result result;
    bool should_break = false;
    setResultCore(&result);
    for (PASS; !should_break && *file != NULL; file++) {
        if (checkFile((*file)) != 1)
            continue;
        if (runParser(*file, inter, false, &pst)) {
            globalIterStatement(&result, inter, pst);
            if (result.type == error_return) {
                printError(&result, inter, true);
                should_break = true;
            }
            freeStatement(pst);
            freeResult(&result);
        }
    }
}

void runCodeStdin(Inter *inter) {
    Statement *pst = NULL;
    Result result;
    bool should_break = false;
    setResultCore(&result);
    printf("%s", HelloString);
    while (!should_break){
        if (ferror(stdin) || feof(stdin))
            clearerr(stdin);
        fprintf(stdout, ">>> ");
        if (runParser(NULL, inter, true, &pst)) {
            globalIterStatement(&result, inter, pst);
            if (result.type == error_return && !(should_break = is_quitExc(result.value, inter)))
                printError(&result, inter, true);
            freeStatement(pst);
            freeResult(&result);
        }
    }
}

bool runParser(char *code_file, Inter *inter, bool is_one, Statement **st) {
    ParserMessage *pm = makeParserMessage(code_file);
    *st = makeStatement(0, (code_file == NULL) ? "stdin" : code_file);
    parserCommandList(pm, inter, true, is_one, *st);

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