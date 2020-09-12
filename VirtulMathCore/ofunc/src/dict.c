#include "__ofunc.h"

ResultType dict_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    VarList *hash = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "bool.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    if (arg != NULL && arg->type == value_arg) {
        setResultError(E_ArgumentException, "Too many argument", 0, "dict.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }


    value = make_new(inter, belong, ap[0].value);
    hash = pushVarList(var_list, inter);
    value->value->type = dict;
    value->value->data.dict.size = 0;
    value->value->data.dict.dict = hash->hashtable;

    gc_addTmpLink(&value->gc_status);
    argumentToVar(&arg, &value->value->data.dict.size, CALL_INTER_FUNCTIONSIG_NOT_ST(hash, result, belong));
    gc_freeTmpLink(&value->gc_status);
    popVarList(hash);

    freeResult(result);
    switch (init_new(value, NULL, "dict.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    freeResult(result);
    setResultOperation(result, value);
    return result->type;
}

ResultType dict_down(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        LinkValue *element = NULL;
        char *name = getNameFromValue(ap[1].value->value, inter);
        element = findVar(name, get_var, inter, ap[0].value->value->data.dict.dict);
        if (element != NULL)
            setResultOperationBase(result, copyLinkValue(element, inter));
        else {
            char *message = memStrcat("Dict could not find key value: ", name, false, false);
            setResultError(E_KeyException, message, 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            memFree(message);
        }
        memFree(name);
    }
    return result->type;
}

ResultType dict_down_del(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        LinkValue *element = NULL;
        char *name = getNameFromValue(ap[1].value->value, inter);
        element = findVar(name, del_var, inter, ap[0].value->value->data.dict.dict);
        if (element != NULL)
            setResult(result, inter, belong);
        else{
            char *message = memStrcat("Cannot delete non-existent keys: ", name, false, false);
            setResultError(E_KeyException, message, 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            memFree(message);
        }
        memFree(name);
    }
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
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[0].value->value->type != dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    for (int index=0; index < MAX_SIZE; index++){
        Var *tmp = ap[0].value->value->data.dict.dict->hashtable[index];
        for (PASS; tmp != NULL; tmp = tmp->next)
            list = connectValueArgument(copyLinkValue(tmp->name_, inter), list);
    }
    makeListValue(list, 0, "dict", value_list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    freeArgument(list, true);
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
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        Argument *dict_iter_arg = makeValueArgument(ap[0].value);
        callBackCore(inter->data.dict_iter, dict_iter_arg, 0, "dict", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeArgument(dict_iter_arg, true);
    }
    return result->type;
}

ResultType dictRepoStrCore(OFFICAL_FUNCTIONSIG, bool is_repo){
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
        setResultError(E_TypeException, INSTANCE_ERROR(dict), 0, "dict", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    again = findAttributes(is_repo ? "repo_again" : "str_again", false, ap[0].value, inter);
    if (again != NULL){
        bool again_ = checkBool(again, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            makeStringValue("{...}", 0, "dict.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return result->type;
        }
    }

    setBoolAttrible(true, is_repo ? "repo_again" : "str_again", 0, "dict.repo", ap[0].value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    repo = memStrcpy("{");
    for (int i = 0, count = 0; i < MAX_SIZE; i++) {
        for (Var *var = value->data.dict.dict->hashtable[i]; var != NULL; var = var->next, count++) {
            char *name_tmp;
            char *value_tmp;
            if (count > 0)
                repo = memStrcat(repo, ", ", true, false);

            freeResult(result);
            name_tmp = getRepoStr(var->name_, is_repo, 0, "dict", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memStrcat(repo, name_tmp, true, false);
            repo = memStrcat(repo, ": ", true, false);

            freeResult(result);
            value_tmp = getRepoStr(var->value, is_repo, 0, "dict", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memStrcat(repo, value_tmp, true, false);
        }
    }

    freeResult(result);
    repo = memStrcat(repo, "}", true, false);
    makeStringValue(repo, 0, "dict.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    return_:
    {
        Result tmp;
        setResultCore(&tmp);
        setBoolAttrible(false, is_repo ? "repo_again" : "str_again", 0, "dict.repo", ap[0].value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &tmp, belong));
        if (!RUN_TYPE(tmp.type)) {
            freeResult(result);
            *result = tmp;
        } else
            freeResult(&tmp);
    }
    memFree(repo);
    return result->type;
}

ResultType dict_repo(OFFICAL_FUNCTIONSIG){
    return dictRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), true);
}

ResultType dict_str(OFFICAL_FUNCTIONSIG){
    return dictRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), false);
}

void registeredDict(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.dict;
    NameFunc tmp[] = {{"keys", dict_keys, object_free_},
                      {inter->data.object_new, dict_new, class_free_},
                      {inter->data.object_down, dict_down, object_free_},
                      {inter->data.object_iter, dict_iter, object_free_},
                      {inter->data.object_repo, dict_repo, object_free_},
                      {inter->data.object_str, dict_str, object_free_},
                      {inter->data.object_down_assignment, dict_down_assignment, object_free_},
                      {inter->data.object_down_del, dict_down_del, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar("dict", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDict(Inter *inter){
    LinkValue *dict = makeBaseChildClass4(inter->data.vobject, inter);
    gc_addStatementLink(&dict->gc_status);
    inter->data.dict = dict;
}
