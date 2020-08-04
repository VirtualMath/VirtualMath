#include "__run.h"

Result includeFile(INTER_FUNCTIONSIG) {
    Result result;
    Result file;
    char *file_dir = NULL;
    ParserMessage *pm = NULL;
    Statement *new_st = makeStatement();

    if (operationSafeInterStatement(&file, CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list)))
        return file;

    // TODO-szh 类型检查
    file_dir = file.value->value->data.str.str;
    if (access(file_dir, R_OK) != 0){
        setResultError(&result, inter);
        printf("tag 1\n");
        goto return_;
    }

    pm = makeParserMessage(file_dir, NULL);
    parserCommandList(pm, inter, true, new_st);
    if (pm->status != success){
        writeLog(pm->paser_debug, ERROR, "Syntax Error: %s\n", pm->status_message);
        setResultError(&result, inter);
        goto return_;
    }

    functionSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(new_st, var_list));

    return_:
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result;
}