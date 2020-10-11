#include "__run.h"

bool importRunParser(ParserMessage *pm, fline line, char *file, Statement *run_st, FUNC_NT) {
    setResultCore(result);
    parserCommandList(pm, inter, true, run_st);
    if (pm->status == int_error)
        setResultError(E_KeyInterrupt, KEY_INTERRUPT, line, file, true, CNEXT_NT);
    else if (pm->status != success) {
        wchar_t *wcs_message = memStrToWcs(pm->status_message, false);
        setResultError(E_TypeException, wcs_message, line, file, true, CNEXT_NT);
        memFree(wcs_message);
    }
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

int checkFileDir(char **file_dir, FUNC) {
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
        char *getcwd(char *buf,size_t size);
        char arr_cwd[200];
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
    setResultErrorSt(E_ImportException, L"import/include file is not readable", true, st, CNEXT_NT);
    return 0;
}

ResultType includeFile(FUNC) {
    Statement *new_st = NULL;
    ParserMessage *pm = NULL;
    char *file_dir = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CFUNC(st->u.include_file.file, var_list, result, belong)))
        return result->type;

    if (!isType(result->value->value, V_str)){
        setResultErrorSt(E_TypeException, ONLY_ACC(include file dir, V_str), true, st, CNEXT_NT);
        goto return_;
    }

    file_dir = memWcsToStr(result->value->value->data.str.str, false);
    freeResult(result);
    if (checkFileDir(&file_dir, CNEXT) != 1)
        goto return_;

    new_st = makeStatement(0, file_dir);
    pm = makeParserMessageFile(file_dir, false);

    if (!importRunParser(pm, st->line, st->code_file, new_st, CNEXT_NT))
        goto return_;

    includeSafeInterStatement(CFUNC(new_st, var_list, result, belong));
    if (result->type == R_yield)
        setResult(result, inter);
    else if (!CHECK_RESULT(result))
        setResultErrorSt(E_BaseException, NULL, false, st, CNEXT_NT);

    return_:
    memFree(file_dir);
    freeStatement(new_st);
    freeParserMessage(pm, true);
    return result->type;
}

ResultType importVMFileCore(Inter *import_inter, char *path, fline line, char *code_file, FUNC_NT) {
    ParserMessage *pm = NULL;
    Statement *run_st = NULL;
    setResultCore(result);

    pm = makeParserMessageFile(path, false);
    run_st = makeStatement(0, path);

    if (!importRunParser(pm, line, code_file, run_st, CNEXT_NT))
        goto return_;

    globalIterStatement(result, import_inter, run_st);
    if (!CHECK_RESULT(result))
        setResultError(E_BaseException, NULL, line, code_file, false, CNEXT_NT);
    else
        setResult(result, inter);

    return_:
    freeStatement(run_st);
    freeParserMessage(pm, true);
    return result->type;
}


ResultType importFileCore(char **path, char **split, int *status, FUNC) {
    setResultCore(result);
    if (operationSafeInterStatement(CNEXT))
        return result->type;

    if (!isType(result->value->value, V_str)) {
        setResultErrorSt(E_ImportException, ONLY_ACC(include file dir, V_str), true, st, CNEXT_NT);
        return R_error;
    }

    *path = memWcsToStr(result->value->value->data.str.str, false);
    *split = splitDir(*path);  // 自动去除末尾路径分隔符
    freeResult(result);
    if ((*status = checkFileDir(path, CNEXT)) == 0)
        return result->type;

    return result->type;
}

ResultType runImportFile(Inter *import_inter, char **path, int status, FUNC) {
    setResultCore(result);
    if (status == 2)
        importClibCore(*path, belong, CFUNC_CORE(inter->var_list));
    else
        importVMFileCore(import_inter, *path, st->line, st->code_file, CNEXT_NT);

    import_inter->var_list = NULL;
    mergeInter(import_inter, inter);
    return result->type;
}

static bool getPackage(LinkValue **imp_value, char *md5_str, char *split, int status, char **path, int *is_new, bool is_lock, FUNC) {
    Value *pg;
    Inter *imp_inter;
    if (is_lock || (pg = checkPackage(inter->package, md5_str, split)) == NULL) {
        setResultCore(result);
        *is_new = true;
        imp_inter = deriveInter(belong, inter);
        pg = makeObject(inter, imp_inter->var_list, copyVarList(var_list, false, inter), NULL);
        if (!is_lock)
            inter->package = makePackage(pg, md5_str, split, inter->package);  // 只有当不是保护读入或私密读入的时才可以记录
        imp_inter->package = inter->package;

        freeResult(result);
        runImportFile(imp_inter, path, status, CNEXT);
        if (!CHECK_RESULT(result))
            return false;
    } else
        *is_new = false;
    *imp_value = makeLinkValue(pg, belong, inter);
    gc_addTmpLink(&(*imp_value)->gc_status);
    return true;
}

ResultType importFile(FUNC) {
    bool is_new = false;
    bool is_lock = st->u.import_file.is_lock;
    Statement *file = st->u.import_file.file;
    int status;
    char *split_path = NULL;
    char *path = NULL;
    LinkValue *imp_value = NULL;
    char md5_str[MD5_STRING];

    setResultCore(result);
    gc_freeze(inter, var_list, NULL, true);

    importFileCore(&path, &split_path, &status, CFUNC(file, var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    getFileMd5(path, md5_str);
    if (!getPackage(&imp_value, md5_str, split_path, status, &path, &is_new, is_lock, CFUNC(file, var_list, result, belong)))
        goto return_;
    freeResult(result);
    if (st->u.import_file.as == NULL) {
        wchar_t *name_ = memStrToWcs(split_path, false);
        addStrVar(name_, false, is_new, imp_value, LINEFILE, false, CNEXT_NT);
        memFree(name_);
    }
    else
        assCore(st->u.import_file.as, imp_value, false, is_new, CNEXT_NT);

    if (CHECK_RESULT(result))  // 若没有出现错误则设定none
        setResult(result, inter);
    gc_freeTmpLink(&imp_value->gc_status);

    return_:
    memFree(split_path);
    memFree(path);
    gc_freeze(inter, var_list, NULL, false);
    return result->type;
}

static ResultType importParameter(fline line, char *file, Parameter *call_pt, Parameter *func_pt, VarList *func_var, LinkValue *imp_belong, FUNC_NT) {
    Argument *call = NULL;
    setResultCore(result);
    call = getArgument(call_pt, false, CFUNC_NT(var_list, result, imp_belong));
    if (!CHECK_RESULT(result)) {
        freeArgument(call, false);
        return result->type;
    }

    freeResult(result);
    setParameterCore(line, file, call, func_pt, func_var, CNEXT_NT);
    freeArgument(call, false);
    return result->type;
}

ResultType fromImportFile(FUNC) {
    int status;
    bool is_new;
    bool is_lock = st->u.from_import_file.is_lock;
    Statement *file = st->u.from_import_file.file;
    char *split_path = NULL;
    char *path = NULL;
    char md5_str[MD5_STRING];  // TODO-szh 设置为空字符串
    VarList *imp_var = NULL;
    LinkValue *imp_value;
    Parameter *pt = st->u.from_import_file.pt;
    Parameter *as = st->u.from_import_file.as != NULL ? st->u.from_import_file.as : st->u.from_import_file.pt;

    setResultCore(result);
    gc_freeze(inter, var_list, NULL, true);
    importFileCore(&path, &split_path, &status, CFUNC(file, var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    getFileMd5(path, md5_str);
    if (!getPackage(&imp_value, md5_str, split_path, status, &path, &is_new, is_lock, CFUNC(file, var_list, result, belong)))
        goto return_;
    imp_var = imp_value->value->object.var;
    if (is_new) {
        wchar_t *wcs;
        LinkValue *string;
        freeResult(result);

        wcs = memStrToWcs(split_path, false);
        makeStringValue(wcs, st->line, st->code_file, CFUNC_NT(var_list, result, imp_value));
        memFree(wcs);

        string = result->value;
        result->value = NULL;
        freeResult(result);

        newObjectSetting(string, st->line, st->code_file, CFUNC_NT(var_list, result, imp_value));
        gc_freeTmpLink(&string->gc_status);
    }

    freeResult(result);
    if (pt != NULL) {
        importParameter(st->line, st->code_file, pt, as, var_list, imp_value, CFUNC_NT(imp_var, result, belong));
        if (!CHECK_RESULT(result)) {
            gc_freeTmpLink(&imp_value->gc_status);
            goto return_;
        }
    }
    else
        updateHashTable(var_list->hashtable, imp_var->hashtable, inter);
    setResult(result, inter);
    gc_freeTmpLink(&imp_value->gc_status);

    return_:
    memFree(path);
    memFree(split_path);
    gc_freeze(inter, var_list, NULL, false);
    return result->type;
}
