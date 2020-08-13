#include "__run.h"

ResultType includeFile(INTER_FUNCTIONSIG) {
    Statement *new_st = NULL;
    ParserMessage *pm = NULL;
    char *file_dir = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list, result, father)))
        return result->type;

    if (!isType(result->value->value, string)){
        setResultError(result, inter, "TypeException", "Don't get a string value", st, father, true);
        goto return_;
    }

    file_dir = result->value->value->data.str.str;
    freeResult(result);

    if (checkFile(file_dir) != 1){
        setResultError(result, inter, "IncludeFileException", "File is not readable", st, father, true);
        goto return_;
    }

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir, NULL);
    parserCommandList(pm, inter, true, new_st);
    if (pm->status != success){
        setResultError(result, inter, "IncludeSyntaxException", pm->status_message, st, father, true);
        goto return_;
    }

    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(new_st, var_list, result, father));
    if (!run_continue(result))
        setResultError(result, inter, NULL, NULL, st, father, false);

    return_:
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result->type;
}