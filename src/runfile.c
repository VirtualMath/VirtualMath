#include "__run.h"

Result includeFile(INTER_FUNCTIONSIG) {
    Result result;
    Result file;
    char *file_dir = NULL;
    ParserMessage *pm = NULL;
    Statement *new_st = NULL;

    if (operationSafeInterStatement(&file, CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list)))
        return file;

    // TODO-szh 类型检查
    file_dir = file.value->value->data.str.str;
    if (access(file_dir, R_OK) != 0){
        setResultError(&result, inter, "IncludeException", "File is not readable", st, true);
        printf("tag 1\n");
        goto return_;
    }
    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir, NULL);
    parserCommandList(pm, inter, true, new_st);
    if (pm->status != success){
        setResultError(&result, inter, "IncludeException", pm->status_message, st, true);
        goto return_;
    }

    functionSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(new_st, var_list));
    if (!run_continue(result))
        setResultError(&result, inter, NULL, NULL, st, false);

    return_:
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result;
}