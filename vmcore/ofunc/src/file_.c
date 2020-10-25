#include "__ofunc.h"

static void setFunctionData(Value *value, LinkValue *cls, Inter *inter) {
    value->data.function.function_data.pt_type = inter->data.default_pt_type;
    value->data.function.function_data.cls = cls;
    value->data.function.function_data.run = false;
}

ResultType file_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1) {
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_file;
    value->value->data.file.is_std = true;
    value->value->data.file.path = NULL;
    value->value->data.file.mode = NULL;
    value->value->data.file.file = NULL;

    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType file_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .must=0, .name=L"file", .long_arg=false},
                           {.type=name_value, .must=0, .name=L"mode", .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    char *path;
    char *mode;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value == NULL) {
        setResult(result, inter);
        return result->type;
    } else if (ap[1].value->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(file, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    } else
        path = memWcsToStr(ap[1].value->value->data.str.str, false);

    if (ap[2].value != NULL && ap[2].value->value->type == V_str)
        mode = memWcsToStr(ap[2].value->value->data.str.str, false);
    else
        mode = memStrcpy("r");

    if (checkFileReadble(path) != 1) {
        setResultError(E_TypeException, L"file is not readable", LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    file->value->data.file.path = path;
    file->value->data.file.mode = mode;
    file->value->data.file.is_std = false;
    file->value->data.file.file = fopen(path, mode);
    if (file->value->data.file.file == NULL)
        setResultFromERR(E_TypeException, CNEXT_NT);
    else
        setResult(result, inter);
    return result->type;
}

ResultType file_read(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .must=0, .name=L"n", .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    wchar_t *tmp = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    fseek(file->value->data.file.file, 0L, SEEK_CUR);  // 改变文件状态(什么都没做)
    if (ap[1].value != NULL) {  // 指定数量读取
        size_t n;
        wint_t ch;
        if (ap[1].value->value->type == V_int)
            n = ap[1].value->value->data.int_.num;
        else if (ap[1].value->value->type == V_dou)
            n = (vint)ap[1].value->value->data.dou.num;
        else {
            setResultError(E_TypeException, ONLY_ACC(n, num), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
        tmp = memWide(n);
        for (int count=0; count < n && (ch = getwc(file->value->data.file.file)) != WEOF; count++)
            tmp[count] = ch;
    } else {
        size_t n = 0;
        size_t step = 50;
        wint_t ch;
        tmp = NULL;
        for (int count=1; (ch = fgetwc(file->value->data.file.file)) != WEOF; count++) {  // 全部读取
            if (count > n) {
                n += step;
                tmp = memWideExpansion(tmp, n, true);
            }
            tmp[count - 1] = ch;
        }
    }

    makeStringValue(tmp, LINEFILE, CNEXT_NT);
    memFree(tmp);
    return result->type;
}

ResultType file_readline(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    wchar_t *tmp = NULL;
    size_t n = 0;
    size_t step = 50;
    wint_t ch;

    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    fseek(file->value->data.file.file, 0L, SEEK_CUR);  // 改变文件状态(什么都没做)
    tmp = NULL;
    for (int count=1; (ch = fgetwc(file->value->data.file.file)) != WEOF && ch != '\n'; count++) {  // 读取到行末
        if (count > n) {
            n += step;
            tmp = memWideExpansion(tmp, n, true);
        }
        tmp[count - 1] = ch;
    }

    makeStringValue(tmp, LINEFILE, CNEXT_NT);
    memFree(tmp);
    return result->type;
}

ResultType file_write(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .must=1, .name=L"str", .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(str, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    fseek(file->value->data.file.file, 0L, 1);  // 改变文件状态(什么都没做)
    fprintf(file->value->data.file.file, "%ls", ap[1].value->value->data.str.str);
    setResult(result, inter);
    return result->type;
}

ResultType file_get_seek(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    long seek;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    seek = ftell(file->value->data.file.file);
    makeIntValue(seek, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType file_err_core(O_FUNC, int type){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    switch (type) {
        case 1:
            makeBoolValue(feof(file->value->data.file.file), LINEFILE, CNEXT_NT);
            break;
        case 2:
            makeBoolValue(ferror(file->value->data.file.file), LINEFILE, CNEXT_NT);
            break;
        case 3:
            clearerr(file->value->data.file.file);
        default:
            setResult(result, inter);
            break;
    }
    return result->type;
}

ResultType file_isend(O_FUNC){
    return file_err_core(CO_FUNC(arg, var_list, result, belong), 1);
}

ResultType file_iserr(O_FUNC){
    return file_err_core(CO_FUNC(arg, var_list, result, belong), 2);
}

ResultType file_clean_err(O_FUNC){
    return file_err_core(CO_FUNC(arg, var_list, result, belong), 3);
}

ResultType file_seek(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .must=1, .name=L"seek", .long_arg=false},
                           {.type=name_value, .must=0, .name=L"where", .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    int where;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value->type != V_int) {
        setResultError(E_TypeException, ONLY_ACC(seek, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[2].value != NULL) {
        if (ap[2].value->value->type != V_int) {
            setResultError(E_TypeException, ONLY_ACC(where, int), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
        switch (ap[2].value->value->data.int_.num) {
            case 0:
                where = SEEK_SET;
                break;
            case 1:
                where = SEEK_CUR;
                break;
            case 2:
                where = SEEK_END;
                break;
            default:
                setResultError(E_TypeException, ONLY_ACC(where, (int)0/1/2), LINEFILE, true, CNEXT_NT);
                return R_error;
        }
    } else
        where = SEEK_SET;

    fgetwc(file->value->data.file.file);  // 必须先读取一个字节才可以调用seek
    fseek(file->value->data.file.file, ap[1].value->value->data.int_.num, where);
    setResult(result, inter);
    return result->type;
}

ResultType file_close(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result)) {
        return result->type;
    }
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (file->value->data.file.file != NULL && !file->value->data.file.is_std) {
        fclose(file->value->data.file.file);
        file->value->data.file.file = NULL;
        file->value->data.file.is_std = true;
        memFree(file->value->data.file.path);
        memFree(file->value->data.file.mode);
    }

    setResult(result, inter);
    return result->type;
}

ResultType file_enter(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result)) {
        return result->type;
    }
    freeResult(result);

    if (ap[0].value->value->type != V_file || ap[0].value->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    setResultOperation(result, ap[0].value);
    return result->type;
}

void registeredFile(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_FILE];
    NameFunc tmp[] = {{L"read", file_read, object_free_, .var=nfv_notpush},
                      {L"write", file_write, object_free_, .var=nfv_notpush},
                      {L"close", file_close, object_free_, .var=nfv_notpush},
                      {L"get_seek", file_get_seek, object_free_, .var=nfv_notpush},
                      {L"seek", file_seek, object_free_, .var=nfv_notpush},
                      {L"readline", file_readline, object_free_, .var=nfv_notpush},
                      {L"end", file_isend, object_free_, .var=nfv_notpush},
                      {L"err", file_iserr, object_free_, .var=nfv_notpush},
                      {L"clean", file_clean_err, object_free_, .var=nfv_notpush},
                      {inter->data.mag_func[M_ENTER], file_enter, object_free_, .var=nfv_notpush},
//                      {inter->data.mag_func[M_DEL], file_close, object_free_, .var=nfv_notpush},
                      {inter->data.mag_func[M_EXIT], file_close, object_free_, .var=nfv_notpush},
                      {inter->data.mag_func[M_NEW], file_new, class_free_, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], file_init, object_free_, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"file", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFile(Inter *inter){
    LinkValue *file = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&file->gc_status);
    inter->data.base_obj[B_FILE] = file;
}
