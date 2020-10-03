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
    if (!CHECK_RESULT(result)) {
        return result->type;
    }
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value == NULL) {
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
        setResultError(E_TypeException, L"File is not readable", LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    file->value->data.file.path = path;
    file->value->data.file.mode = mode;
    file->value->data.file.is_std = false;
    file->value->data.file.file = fopen(path, mode);

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
    if (!CHECK_RESULT(result)) {
        return result->type;
    }
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value != NULL) {
        size_t n;
        wint_t ch;
        if (ap[1].value->value->type != V_int)
            n = ap[1].value->value->data.int_.num;
        else if (ap[1].value->value->type != V_dou)
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
        for (int count=1; (ch = getwc(file->value->data.file.file)) != WEOF; count++) {
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

ResultType file_write(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .must=1, .name=L"str", .long_arg=false},
                           {.must=-1}};
    LinkValue *file;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result)) {
        return result->type;
    }
    freeResult(result);

    if ((file = ap[0].value)->value->type != V_file || file->value->data.file.file == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(file), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(str, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    fprintf(file->value->data.file.file, "%ls", ap[1].value->value->data.str.str);
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
    LinkValue *object = inter->data.file;
    NameFunc tmp[] = {{L"read", file_read, object_free_},
                      {L"write", file_write, object_free_},
                      {L"close", file_close, object_free_},
                      {inter->data.object_enter, file_enter, object_free_},
                      {inter->data.object_del, file_close, object_free_},
                      {inter->data.object_exit, file_close, object_free_},
                      {inter->data.object_new, file_new, class_free_},
                      {inter->data.object_init, file_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"file", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFile(Inter *inter){
    LinkValue *file = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&file->gc_status);
    inter->data.file = file;
}
