#include "__run.h"

Result includeFile(INTER_FUNCTIONSIG) {
    Statement *new_st = NULL;
    ParserMessage *pm = NULL;
    char *file_dir = NULL;
    Result result;
    Result file;
    setResultCore(&result);
    setResultCore(&file);

    if (operationSafeInterStatement(&file, CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list)))
        return file;

    if (!isType(file.value->value, string)){
        freeResult(&file);
        setResultError(&result, inter, "TypeException", "Don't get a string value", st, true);
        goto return_;
    }

    file_dir = file.value->value->data.str.str;
    freeResult(&file);

    if (checkFile(file_dir) != 1){
        setResultError(&result, inter, "IncludeFileException", "File is not readable", st, true);
        goto return_;
    }

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir, NULL);
    parserCommandList(pm, inter, true, new_st);
    if (pm->status != success){
        setResultError(&result, inter, "IncludeSyntaxException", pm->status_message, st, true);
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