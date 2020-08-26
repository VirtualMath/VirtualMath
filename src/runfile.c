#include "__run.h"

int checkFileDir(char **file_dir, INTER_FUNCTIONSIG) {
    if (checkFile(*file_dir) == 1)
        return 1;

    {
        char cwd[200] = {};
        char *cwd_tmp = NULL;
        getcwd(cwd, 200);
        cwd_tmp = memStrcat(cwd, "/", false, false);
        cwd_tmp = memStrcat(cwd_tmp, *file_dir, true, false);
        if (checkFile(cwd_tmp) == 1 || checkFile(cwd_tmp = memStrcat(cwd_tmp, ".vm", true, false)) == 1) {
            memFree(*file_dir);
            *file_dir = cwd_tmp;
            return 1;
        } else
            memFree(cwd_tmp);
    }

    {
        char *path = memStrcpy(getenv("VIRTUALMATHPATH"));
        for (char *tmp = strtok(path, ";"), *new_dir, *new_tmp; tmp != NULL; tmp = strtok(NULL, ";")) {
#ifdef __linux__
            if (*(tmp + (memStrlen(tmp) - 1)) != '/')
                new_tmp = memStrcat(tmp, "/", false, false);
#else
                if (*(tmp + (memStrlen(tmp) - 1)) != '\\')
                    new_tmp = memStrcat(tmp, "\\", false, false);
#endif
            else
                new_tmp = memStrcpy(tmp);
            new_dir = memStrcat(new_tmp, *file_dir, false, false);
            memFree(new_tmp);

            if (checkFile(new_dir) == 1 || checkFile(new_dir = memStrcat(new_dir, ".vm", true, false)) == 1) {
                memFree(*file_dir);
                *file_dir = new_dir;
                return 1;
            } else
                memFree(new_dir);
        }
        memFree(path);
    }

    if (checkCLib(*file_dir))
        return 2;

    setResultErrorSt(E_ImportException, "import file is not readable", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return 0;
}

ResultType includeFile(INTER_FUNCTIONSIG) {
    Statement *new_st = NULL;
    ParserMessage *pm = NULL;
    char *file_dir = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.include_file.file, var_list, result, belong)))
        return result->type;

    if (!isType(result->value->value, string)){
        setResultErrorSt(E_TypeException, ONLY_ACC(include file dir, string), true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }

    file_dir = result->value->value->data.str.str;
    freeResult(result);
    freeResult(result);
    if (checkFileDir(&file_dir, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)) != 1)
        goto return_;

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir);
    parserCommandList(pm, inter, true, false, new_st);
    if (pm->status == int_error) {
        setResultErrorSt(E_KeyInterrupt, KEY_INTERRUPT, true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }
    else if (pm->status != success){
        setResultErrorSt(E_IncludeException, pm->status_message, true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }

    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(new_st, var_list, result, belong));
    if (result->type == yield_return)
        setResult(result, inter, belong);
    else if (!CHECK_RESULT(result))
        setResultErrorSt(E_BaseException, NULL, false, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    return_:
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result->type;
}

ResultType importVMFileCore(VarList **new_object, char *file_dir, fline line, char *code_file, INTER_FUNCTIONSIG_NOT_ST) {
    Inter *import_inter = NULL;
    ParserMessage *pm = NULL;
    Statement *run_st = NULL;
    setResultCore(result);

    import_inter = deriveInter(belong, inter);
    pm = makeParserMessage(file_dir);
    run_st = makeStatement(0, file_dir);

    parserCommandList(pm, import_inter, true, false, run_st);
    if (pm->status == int_error) {
        setResultError(E_KeyInterrupt, KEY_INTERRUPT, line, code_file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeInter(import_inter, false);
        goto return_;
    }
    else if (pm->status != success) {
        setResultError(E_TypeException, pm->status_message, line, code_file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeInter(import_inter, false);
        goto return_;
    }

    globalIterStatement(result, import_inter, run_st);
    if (!CHECK_RESULT(result)) {
        freeInter(import_inter, false);
        result->value = makeLinkValue(makeNoneValue(inter), belong, inter);
        setResultError(E_BaseException, NULL, line, code_file, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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

ResultType importShareFileCore(VarList **new_object, char *file_dir, INTER_FUNCTIONSIG_NOT_ST) {
    Inter *import_inter;
    setResultCore(result);
    Registered reg;
    void* handle = dlopen(file_dir, RTLD_LAZY);

    reg = dlsym(handle, "registered");
    import_inter = deriveInter(belong, inter);

    reg(belong, import_inter, import_inter->var_list);
    *new_object = import_inter->var_list;
    import_inter->var_list = NULL;

    mergeInter(import_inter, inter);
    setResult(result, inter, belong);
    return result->type;
}

ResultType importCLibFileCore(VarList **new_object, char *file_dir, INTER_FUNCTIONSIG_NOT_ST) {
    Inter *import_inter;
    setResultCore(result);
    import_inter = deriveInter(belong, inter);

    runClib(file_dir, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

    *new_object = import_inter->var_list;
    import_inter->var_list = NULL;
    mergeInter(import_inter, inter);
    setResult(result, inter, belong);
    return result->type;
}

ResultType importFileCore(VarList **new_object, char **file_dir, INTER_FUNCTIONSIG) {
    int status;
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;

    if (!isType(result->value->value, string)) {
        setResultErrorSt(E_ImportException, ONLY_ACC(include file dir, string), true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    *file_dir = memStrcpy(result->value->value->data.str.str);
    freeResult(result);
    if ((status = checkFileDir(file_dir, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong))) == 0)
        return result->type;

    {
        char *file = strrchr(*file_dir, '.');
        if (status == 2)
            importCLibFileCore(new_object, *file_dir, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else if (file != NULL && eqString(file, ".vm"))
            importVMFileCore(new_object, *file_dir, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else
            importShareFileCore(new_object, *file_dir, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    }

    return result->type;
}

ResultType importFile(INTER_FUNCTIONSIG) {
    char *file_dir = NULL;
    VarList *new_object = NULL;
    LinkValue *import_value = NULL;
    setResultCore(result);
    gc_freeze(inter, var_list, NULL, true);

    importFileCore(&new_object, &file_dir, CALL_INTER_FUNCTIONSIG(st->u.import_file.file, var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    {
        VarList *import_var = copyVarList(var_list, false, inter);
        Value *import_obj = makeObject(inter, new_object, import_var, NULL);
        import_value = makeLinkValue(import_obj, belong, inter);
    }
    if (st->u.import_file.as != NULL)
        assCore(st->u.import_file.as, import_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else
        addStrVar(splitDir(file_dir), true, true, import_value, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    setResult(result, inter, belong);

    return_:
    memFree(file_dir);
    gc_freeze(inter, var_list, NULL, false);
    return result->type;
}


ResultType fromImportFile(INTER_FUNCTIONSIG) {
    char *file_dir = NULL;
    VarList *new_object = NULL;
    Parameter *pt = st->u.from_import_file.pt;
    Parameter *as = st->u.from_import_file.as != NULL ? st->u.from_import_file.as : st->u.from_import_file.pt;
    setResultCore(result);
    importFileCore(&new_object, &file_dir, CALL_INTER_FUNCTIONSIG(st->u.from_import_file.file, var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    if (pt != NULL) {
        setParameter(st->line, st->code_file, pt, as, var_list, belong, CALL_INTER_FUNCTIONSIG_NOT_ST(new_object, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
    }
    else
        updateHashTable(var_list->hashtable, new_object->hashtable, inter);
    setResult(result, inter, belong);

    return_:
    freeVarList(new_object);
    return result->type;
}
