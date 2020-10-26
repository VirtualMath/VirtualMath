#include "__ofunc.h"

ResultType lib_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_lib;
    value->value->data.lib.handle = NULL;

    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType lib_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"path", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *clib;
    char *path;
    void *handle;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((clib = ap[0].value)->value->type != V_lib) {
        setResultError(E_TypeException, INSTANCE_ERROR(clib), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(path, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    path = memWcsToStr(ap[1].value->value->data.str.str, false);
    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) {
        wchar_t *tmp = memWidecat(L"load lib error: ", memStrToWcs(dlerror(), false), false, true);
        setResultError(E_ImportException, tmp, LINEFILE, true, CNEXT_NT);
        memFree(tmp);
        goto return_;
    }
    clib->value->data.lib.handle = handle;

    {
        void (*test)(void) = dlsym(handle, "init");  // 运行init函数
        if (test != NULL)
            test();
    }

    setResult(result, inter);
    return_:
    memFree(path);
    return result->type;
}

ResultType lib_close(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *clib;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((clib = ap[0].value)->value->type != V_lib) {
        setResultError(E_TypeException, INSTANCE_ERROR(clib), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (clib->value->data.lib.handle != NULL) {
        dlclose(clib->value->data.lib.handle);
        clib->value->data.lib.handle = NULL;
    }

    setResult(result, inter);
    return result->type;
}

ResultType lib_addCore(wchar_t *name_, LinkValue *clib, FUNC_NT) {
    char *name = memWcsToStr(name_, false);
    void (*func)() = dlsym(clib->value->data.lib.handle, name);
    memFree(name);
    if (func == NULL) {
        wchar_t *tmp = memWidecat(L"function not found: ", memStrToWcs(dlerror(), false), false, true);
        setResultError(E_NameExceptiom, tmp, LINEFILE, true, CNEXT_NT);
        memFree(tmp);
    } else {
        makeFFunctionValue(FFI_FN(func), LINEFILE, CFUNC_NT(var_list, result, clib));
        if (CHECK_RESULT(result)) {
            LinkValue *func_value;
            GET_RESULT(func_value, result);
            addAttributes(name_, false, func_value, LINEFILE, false, CFUNC_NT(var_list, result, clib));
            gc_freeTmpLink(&func_value->gc_status);
            if (CHECK_RESULT(result))
                setResultOperation(result, func_value);
        }
    }
    return result->type;
}

ResultType lib_add(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"func", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *clib;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((clib = ap[0].value)->value->type != V_lib || clib->value->data.lib.handle == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(clib), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (ap[1].value->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(func, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    lib_addCore(ap[1].value->value->data.str.str, clib, CNEXT_NT);
    return result->type;
}

ResultType lib_attr(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"error", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *clib;
    LinkValue *error;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if ((clib = ap[0].value)->value->type != V_lib || clib->value->data.lib.handle == NULL) {
        setResultError(E_TypeException, INSTANCE_ERROR(clib), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    error = findAttributes(inter->data.mag_func[M_VAL], false, LINEFILE, true, CFUNC_NT(var_list, result, ap[1].value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (error == NULL) {
        setResultError(E_TypeException, ONLY_ACC(error, NameException with val), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (error->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(error.val, str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    lib_addCore(error->value->data.str.str, clib, CNEXT_NT);
    return result->type;
}

void registeredLib(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_LIB];
    NameFunc tmp[] = {{L"close", lib_close, fp_obj, .var=nfv_notpush},
                      {L"add", lib_add, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_NEW], lib_new, fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], lib_init, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DEL], lib_close, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_ATTR], lib_attr, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"clib", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseLib(Inter *inter){
    LinkValue *lib_ = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&lib_->gc_status);
    inter->data.base_obj[B_LIB] = lib_;
}
