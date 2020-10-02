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
        goto return_;
    }
    clib->value->data.lib.handle = handle;

    {
        void (*test)(void) = dlsym(handle, "init");
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

void registeredLib(R_FUNC){
    LinkValue *object = inter->data.lib_;
    NameFunc tmp[] = {{L"close", lib_close, object_free_},
                      {inter->data.object_new, lib_new, class_free_},
                      {inter->data.object_init, lib_init, object_free_},
                      {inter->data.object_del, lib_close, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"clib", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseLib(Inter *inter){
    LinkValue *lib_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&lib_->gc_status);
    inter->data.lib_ = lib_;
}
