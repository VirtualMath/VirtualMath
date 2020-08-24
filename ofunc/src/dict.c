#include "__ofunc.h"

ResultType dict_down(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *name = NULL;
    LinkValue *element = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    name = getNameFromValue(ap[1].value->value, inter);
    element = findVar(name, 0, inter, ap[0].value->value->data.dict.dict);
    memFree(name);
    if (element != NULL)
        setResultOperationBase(result, copyLinkValue(element, inter));
    else
        setResultError(E_KeyException, "Key Not Found", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType dict_down_assignment(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *name = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    name = getNameFromValue(ap[2].value->value, inter);
    addVar(name, ap[1].value, ap[2].value, inter, ap[0].value->value->data.dict.dict);
    memFree(name);
    return result->type;
}

ResultType dict_keys(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    Argument *list = NULL;
    Argument *backup = NULL;
    LinkValue *element = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    for (int index=0; index < MAX_SIZE; index++){
        Var *tmp = ap[0].value->value->data.dict.dict->hashtable[index];
        for (PASS; tmp != NULL; tmp = tmp->next)
            list = connectValueArgument(copyLinkValue(tmp->name_, inter), list);
    }
    backup = list;
    element = makeLinkValue(makeListValue(&list, inter, value_list), belong, inter);
    setResultOperationBase(result, element);
    freeArgument(backup, true);
    return result->type;
}

ResultType dict_iter(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, "Don't get a dict", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        Argument *dict_iter_arg = makeValueArgument(ap[0].value);
        LinkValue *iter_dict = makeLinkValue(inter->data.dict_iter, inter->base_father, inter);
        gc_addTmpLink(&iter_dict->gc_status);
        callBackCore(iter_dict, dict_iter_arg, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&iter_dict->gc_status);
        freeArgument(dict_iter_arg, true);
    }
    return result->type;
}

ResultType dict_repo(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *repo = NULL;
    Value *value = NULL;
    LinkValue *again = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    if (value->type != dict){
        setResultError(E_TypeException, "dict.__repo__ gets unsupported data", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    again = findAttributes("repo_again", false, ap[0].value, inter);
    if (again != NULL){
        bool again_ = checkBool(again, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            setResultOperation(result, makeLinkValue(makeStringValue("{...}", inter), belong, inter));
            return result->type;
        }
    }

    addAttributes("repo_again", false, makeLinkValue(makeBoolValue(true, inter), belong, inter), ap[0].value, inter);
    repo = memStrcpy("{");
    for (int i = 0, count = 0; i < MAX_SIZE; i++) {
        for (Var *var = value->data.dict.dict->hashtable[i]; var != NULL; var = var->next, count++) {
            char *name_tmp;
            char *value_tmp;
            if (count > 0)
                repo = memStrcat(repo, ", ", true, false);

            freeResult(result);
            name_tmp = getRepo(var->name_, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memStrcat(repo, name_tmp, true, false);
            repo = memStrcat(repo, ": ", true, false);

            freeResult(result);
            value_tmp = getRepo(var->value, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memStrcat(repo, value_tmp, true, false);
        }
    }

    repo = memStrcat(repo, "}", true, false);
    setResultOperation(result, makeLinkValue(makeStringValue(repo, inter), belong, inter));

    return_:
    addAttributes("repo_again", false, makeLinkValue(makeBoolValue(false, inter), belong, inter), ap[0].value, inter);
    memFree(repo);
    return result->type;
}

void registeredDict(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.dict, inter->base_father, inter);
    NameFunc tmp[] = {{"keys", dict_keys, object_free_},
                      {"__down__", dict_down, object_free_},
                      {"__iter__", dict_iter, object_free_},
                      {"__repo__", dict_repo, object_free_},
                      {"__down_assignment__", dict_down_assignment, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("dict", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDict(Inter *inter){
    Value *dict = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&dict->gc_status);
    inter->data.dict = dict;
}
