#include "__run.h"

ResultType includeFile(INTER_FUNCTIONSIG) {
    Statement *new_st = NULL;
    ParserMessage *pm = NULL;
    char *file_dir = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list, result, belong)))
        return result->type;

    if (!isType(result->value->value, string)){
        setResultErrorSt(result, inter, "TypeException", "Don't get a string value", st, belong, true);
        goto return_;
    }

    file_dir = result->value->value->data.str.str;
    freeResult(result);

    if (checkFile(file_dir) != 1){
        setResultErrorSt(result, inter, "IncludeFileException", "File is not readable", st, belong, true);
        goto return_;
    }

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir);
    parserCommandList(pm, inter, true, new_st);
    if (pm->status != success){
        setResultErrorSt(result, inter, "IncludeSyntaxException", pm->status_message, st, belong, true);
        goto return_;
    }

    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(new_st, var_list, result, belong));
    if (!run_continue(result))
        setResultErrorSt(result, inter, NULL, NULL, st, belong, false);

    return_:
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result->type;
}

ResultType importFileCore(VarList **new_object, char **file_dir, INTER_FUNCTIONSIG) {
    Inter *import_inter = NULL;
    ParserMessage *pm = NULL;
    Statement *run_st = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        goto return_;

    if (!isType(result->value->value, string)) {
        setResultErrorSt(result, inter, "TypeException", "Don't get a string value", st, belong, true);
        goto return_;
    }

    *file_dir = result->value->value->data.str.str;
    freeResult(result);
    if (checkFile(*file_dir) != 1) {
        setResultErrorSt(result, inter, "ImportFileException", "File is not readable", st, belong, true);
        goto return_;
    }


    import_inter = makeInter(NULL, belong);
    pm = makeParserMessage(*file_dir);
    run_st = makeStatement(0, *file_dir);
    parserCommandList(pm, import_inter, true, run_st);
    if (pm->status != success) {
        freeInter(import_inter, false);
        setResultErrorSt(result, inter, "ImportSyntaxException", pm->status_message, st, belong, true);
        goto return_;
    }

    globalIterStatement(result, import_inter, run_st);
    if (!run_continue(result)) {
        freeInter(import_inter, false);
        result->value = makeLinkValue(inter->base, belong, inter);  // 重新设定none值
        setResultErrorSt(result, inter, NULL, NULL, st, belong, false);
        goto return_;
    }

    *new_object = import_inter->var_list;
    import_inter->var_list = NULL;
    mergeInter(import_inter, inter);
    setResult(result, inter, belong);

    return_:
    freeStatement(run_st);
    freeParserMessage(pm, true);
    return result->type;
}

ResultType importFile(INTER_FUNCTIONSIG) {
    char *file_dir = NULL;
    VarList *new_object = NULL;
    LinkValue *import_value = NULL;
    setResultCore(result);
    importFileCore(&new_object, &file_dir, CALL_INTER_FUNCTIONSIG(st->u.import_file.file, var_list, result, belong));
    if (!run_continue(result))
        goto return_;

    freeResult(result);
    {
        VarList *import_var = copyVarList(var_list, false, inter);
        Value *import_obj = makeObject(inter, new_object, import_var, NULL);
        import_value = makeLinkValue(import_obj, belong, inter);
    }
    if (st->u.import_file.as != NULL)
        assCore(st->u.import_file.as, import_value, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else
        addStrVar(splitDir(file_dir), true, import_value, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    setResult(result, inter, belong);

    return_:
    return result->type;
}


ResultType fromImportFile(INTER_FUNCTIONSIG) {
    char *file_dir = NULL;
    VarList *new_object = NULL;
    Parameter *pt = st->u.from_import_file.pt;
    Parameter *as = st->u.from_import_file.as != NULL ? st->u.from_import_file.as : st->u.from_import_file.pt;
    setResultCore(result);
    importFileCore(&new_object, &file_dir, CALL_INTER_FUNCTIONSIG(st->u.from_import_file.file, var_list, result, belong));
    if (!run_continue(result))
        goto return_;

    freeResult(result);
    if (pt != NULL) {
        setParameter(st->line, st->code_file, pt, as, var_list, belong, CALL_INTER_FUNCTIONSIG_NOT_ST(new_object, result, belong));
        if (!run_continue(result))
            goto return_;
    }
    else
        updateHashTable(var_list->hashtable, new_object->hashtable, inter);
    setResult(result, inter, belong);

    return_:
    freeVarList(new_object);
    return result->type;
}
