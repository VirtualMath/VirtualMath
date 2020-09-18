#include "__run.h"

bool importRunParser(ParserMessage *pm, fline line, char *file, Statement *run_st, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);
    parserCommandList(pm, inter, true, false, run_st);
    if (pm->status == int_error)
        setResultError(E_KeyInterrupt, KEY_INTERRUPT, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else if (pm->status != success)
        setResultError(E_TypeException, pm->status_message, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return CHECK_RESULT(result);
}

int isAbsolutePath(const char *path) {
    switch (*path) {
        case ':':
            return 1;
        case '@':
            return 2;
        case '$':
            return 3;
        default:
            return 0;
    }
}

static bool isExist(char **path, bool is_ab, char *file) {
    char *backup = is_ab ? memStrcpy((*path) + 1) : memStrcpy(*path);
    int status;
    if ((status = checkFileReadble(backup)) != 3 || (status = checkFileReadble(backup = memStrcat(backup, ".vm", true, false))) != 3) {
        memFree(*path);
        *path = backup;
        if (status == 2) {
            if (file == NULL)
                return false;
#if __linux__
            if ((*path)[memStrlen(*path) - 1] != '/')
                *path = memStrcat(*path, "/", true, false);
            *path = memStrcat(*path, file, true, false);
#else
            if ((*path)[memStrlen(*path) - 1] != '\\')
                *path = memStrcat(*path, "\\", true, false);
            *path = memStrcat(*path, file, true, false);
#endif
            return isExist(path, false, NULL);
        } else
            return true;
    }
    memFree(backup);
    return false;
}

int checkFileDir(char **file_dir, INTER_FUNCTIONSIG) {
    switch (isAbsolutePath(*file_dir)) {
        case 1:
            if (isExist(file_dir, true, "__init__.vm"))
                return 1;
            goto error_;
        case 2:
            goto clib;
        case 3:
            goto path;
        default:
            break;
    }

    if (isExist(file_dir, false, "__init__.vm"))
        return 1;

    {
        char arr_cwd[200] = {};
        char *p_cwd = NULL;
        getcwd(arr_cwd, 200);
#ifdef __linux__
        p_cwd = memStrcatIter(arr_cwd, false, "/", *file_dir, NULL);
#else
        p_cwd = memStrcatIter(arr_cwd, false, "\\", *file_dir);
#endif
        if (isExist(&p_cwd, false, "__init__.vm")) {
            memFree(*file_dir);
            *file_dir = p_cwd;
            return 1;
        }
        memFree(p_cwd);
    }

    path: {
        char *path = memStrcpy(getenv("VIRTUALMATHPATH"));
        for (char *tmp = strtok(path, ";"), *new_dir; tmp != NULL; tmp = strtok(NULL, ";")) {
#ifdef __linux__
            if (*(tmp + (memStrlen(tmp) - 1)) != '/')
                new_dir = memStrcatIter(tmp, false, "/", *file_dir, NULL);
#else
            if (*(tmp + (memStrlen(tmp) - 1)) != '\\')
                new_dir = memStrcatIter(tmp, false, "\\", *file_dir);
#endif
            else
                new_dir = memStrcat(tmp, *file_dir, false, false);

            if (isExist(&new_dir, false, "__init__.vm")) {
                memFree(*file_dir);
                *file_dir = new_dir;
                return 1;
            }
            memFree(new_dir);

        }
        memFree(path);
    }

    clib:
    if (checkCLib(*file_dir))
        return 2;

    error_:
    setResultErrorSt(E_ImportException, "import/include file is not readable", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
    if (checkFileDir(&file_dir, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)) != 1)
        goto return_;

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessage(file_dir);

    if (!importRunParser(pm, st->line, st->code_file, new_st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        goto return_;

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

    if (!importRunParser(pm, line, code_file, run_st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        freeInter(import_inter, false);
        goto return_;
    }

    globalIterStatement(result, import_inter, run_st);
    if (!CHECK_RESULT(result)) {
        freeInter(import_inter, false);
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

    if (status == 2)
        importCLibFileCore(new_object, *file_dir, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else
        importVMFileCore(new_object, *file_dir, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

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

    {
        VarList *import_var = copyVarList(var_list, false, inter);
        Value *import_obj = makeObject(inter, new_object, import_var, NULL);
        import_value = makeLinkValue(import_obj, belong, inter);
    }

    freeResult(result);
    if (st->u.import_file.as != NULL)
        assCore(st->u.import_file.as, import_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else {
        addStrVar(splitDir(file_dir), true, true, import_value, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
    }
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
