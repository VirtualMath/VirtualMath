#include "__ofunc.h"

ResultType object_new(O_FUNC){
    LinkValue *value = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType objectRepoStrCore(O_FUNC, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo;
    wchar_t *name;
    wchar_t *type;
    LinkValue *name_value;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    name_value = findAttributes(inter->data.mag_func[M_NAME], false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (name_value != NULL){
        gc_addTmpLink(&name_value->gc_status);
        name = getRepoStr(name_value, is_repo, LINEFILE, CNEXT_NT);
        gc_freeTmpLink(&name_value->gc_status);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    } else
        name = L"unknown";

    if (ap[0].value->value->type == V_none) {
        repo = memWidecpy(L"null");
    } else {
        size_t len;
        if (ap[0].value->value->type == V_class)
            type = L"V_class";
        else
            type = L"object";
        len = memWidelen(name) + 30;
        repo = memCalloc(len, sizeof(char ));
        swprintf(repo, len, L"(%ls: %ls on %p)", type, name, ap[0].value->value);
    }

    makeStringValue(repo, LINEFILE, CNEXT_NT);
    memFree(repo);
    return result->type;
}

ResultType object_repo(O_FUNC){
    return objectRepoStrCore(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType object_str(O_FUNC){
    return objectRepoStrCore(CO_FUNC(arg, var_list, result, belong), false);
}

void registeredObject(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_OBJECT];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW],  object_new,  class_free_},
                      {inter->data.mag_func[M_REPO], object_repo, all_free_},
                      {inter->data.mag_func[M_STR],  object_str,  all_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"object", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter, LinkValue *belong){
    LinkValue *g_belong;
    Value *object = makeClassValue(inter->var_list, inter, NULL);

    {
        Value *global_belong = makeObject(inter, copyVarList(inter->var_list, false, inter), NULL, NULL);
        g_belong = makeLinkValue(global_belong, belong, inter);
        inter->base_belong = g_belong;
        gc_addStatementLink(&inter->base_belong->gc_status);
    }

    inter->data.base_obj[B_OBJECT] = makeLinkValue(object, g_belong, inter);
    gc_addStatementLink(&inter->data.base_obj[B_OBJECT]->gc_status);
    for (Inherit *ih=g_belong->value->object.inherit; ih != NULL; ih = ih->next) {
        if (ih->value->value == object)
            ih->value->belong = g_belong;
    }


    {
        Result result;
        Argument *arg = makeValueArgument(makeLinkValue(object, g_belong, inter));
        setResultCore(&result);
        object_new(CO_FUNC(arg, inter->var_list, &result, g_belong));

        result.value->value->type = V_none;
        inter->data.base_obj[B_NONE] = result.value;
        gc_addStatementLink(&inter->data.base_obj[B_NONE]->gc_status);

        freeArgument(arg, true);
        freeResult(&result);
    }
}
