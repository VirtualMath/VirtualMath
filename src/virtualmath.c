#include "hellovm.h"

void runCodeFile(Inter *inter, char *file[]) {
    Statement *pst = NULL;
    Result result;
    bool should_break = false;
    setResultCore(&result);
    for (PASS; !should_break && *file != NULL; file++) {
        int status;
        if ((status = checkFileReadable((*file))) == 3)
            continue;
        else if (status == 2) {
            *file = memStrcat(*file, ((*file)[memStrlen(*file) - 1] != SEP_CH ? SEP"__main__.vm" : "__main__.vm"), false, false);
            if (checkFileReadable(*file) != 1)
                continue;
        }
        if (runParser(*file, inter, false, &pst)) {
            changeInterEnv(*file, true, inter);  // 设置运行环境
            globalIterStatement(&result, inter, pst, true);
            if (result.type == R_error) {
                printError(&result, inter, args.p_clock);
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
            globalIterStatement(&result, inter, pst, false);
            if (result.type == R_error && !(should_break = is_quitExc(result.value, inter)))
                printError(&result, inter, true);
            freeStatement(pst);
            freeResult(&result);
        }
    }
}

void safe_sleep(double ms) {
    time_t start = clock();
    time_t now;
    double d_time;
    do {
        now = clock();
        d_time = (double)(now - start) / CLOCKS_PER_SEC;
        if (d_time >= ms)
            break;
    } while (true);
}

bool checkSignalPm() {  // 检查信号
    if (signal_tag.status == signal_appear && PARSER_STOP_SIGNAL(signal_tag.signum)) {
        signal_tag.status = signal_reset;
        return true;
    }
    return false;
}

bool runParser(char *code_file, Inter *inter, bool is_one, Statement **st) {
    ParserMessage *pm = makeParserMessageFile(code_file, is_one);
    *st = makeStatement(0, (code_file == NULL) ? "stdin" : code_file);
    parserCommandList(pm, inter, true, *st);
    safe_sleep(0.005);  // 等待 0.005s 捕捉信号 (若信号捕捉不到可能要适当调高此处的等待时间)
    if (checkSignalPm())
        fprintf(stderr, "Signal: KeyInterrupt\n");
    else if (pm->status == int_error)
        fprintf(stderr, "Signal Error: %s\n", pm->status_message);
    else if (pm->status != success)
        fprintf(stderr, "Syntax Error: %s\n", pm->status_message);
    else if (!checkParserMessageIO(pm))
        fprintf(stderr, "IO Error\n");
    else {
        freeParserMessage(pm, true);
        return true;
    }

    freeStatement(*st);
    freeParserMessage(pm, true);
    *st = NULL;
    return false;
}