#include "__ofunc.h"

ResultType object_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "object.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    run_init(value, arg, 0, "obj.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType objectRepoStrCore(OFFICAL_FUNCTIONSIG, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo;
    wchar_t *name;
    wchar_t *type;
    LinkValue *name_value;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    name_value = findAttributes(inter->data.object_name, false, 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (name_value != NULL){
        gc_addTmpLink(&name_value->gc_status);
        name = getRepoStr(name_value, is_repo, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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

    makeStringValue(repo, 0, "object.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    memFree(repo);
    return result->type;
}

ResultType object_repo(OFFICAL_FUNCTIONSIG){
    return objectRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), true);
}

ResultType object_str(OFFICAL_FUNCTIONSIG){
    return objectRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), false);
}

void registeredObject(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.object;
    NameFunc tmp[] = {{inter->data.object_new,  object_new,  class_free_},
                      {inter->data.object_repo, object_repo, all_free_},
                      {inter->data.object_str,  object_str,  all_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"object", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
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

    inter->data.object = makeLinkValue(object, g_belong, inter);
    gc_addStatementLink(&inter->data.object->gc_status);
    for (Inherit *ih=g_belong->value->object.inherit; ih != NULL; ih = ih->next) {
        if (ih->value->value == object)
            ih->value->belong = g_belong;
    }


    {
        Result result;
        Argument *arg = makeValueArgument(makeLinkValue(object, g_belong, inter));
        setResultCore(&result);
        object_new(CALL_OFFICAL_FUNCTION(arg, inter->var_list, &result, g_belong));

        result.value->value->type = V_none;
        inter->data.none = result.value;
        gc_addStatementLink(&inter->data.none->gc_status);

        freeArgument(arg, true);
        freeResult(&result);
    }
}
