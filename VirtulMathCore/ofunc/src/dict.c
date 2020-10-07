#include "__ofunc.h"

ResultType dict_new(O_FUNC){
    LinkValue *value = NULL;
    VarList *hash = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (arg != NULL && arg->type == value_arg) {
        setResultError(E_ArgumentException, L"Too many argument", LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    hash = pushVarList(var_list, inter);
    value->value->type = V_dict;
    value->value->data.dict.size = 0;
    value->value->data.dict.dict = hash->hashtable;

    gc_addTmpLink(&value->gc_status);
    argumentToVar(&arg, &value->value->data.dict.size, CFUNC_NT(hash, result, belong));
    gc_freeTmpLink(&value->gc_status);
    popVarList(hash);

    freeResult(result);
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType dict_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    {
        LinkValue *element = NULL;
        wchar_t *name = getNameFromValue(ap[1].value->value, inter->data.var_deep, inter);
        element = findVar(name, get_var, inter, ap[0].value->value->data.dict.dict);
        if (element != NULL)
            setResultOperationBase(result, copyLinkValue(element, inter));
        else {
            wchar_t *message = memWidecat(L"Dict could not find key value: ", name, false, false);
            setResultError(E_KeyException, message, LINEFILE, true, CNEXT_NT);
            memFree(message);
        }
        memFree(name);
    }
    return result->type;
}

ResultType dict_down_del(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    {
        LinkValue *element = NULL;
        wchar_t *name = getNameFromValue(ap[1].value->value, inter->data.var_deep, inter);
        element = findVar(name, del_var, inter, ap[0].value->value->data.dict.dict);
        if (element != NULL)
            setResult(result, inter);
        else{
            wchar_t *message = memWidecat(L"Cannot delete non-existent keys: ", name, false, false);
            setResultError(E_KeyException, message, LINEFILE, true, CNEXT_NT);
            memFree(message);
        }
        memFree(name);
    }
    return result->type;
}

ResultType dict_down_assignment(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *name = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    name = getNameFromValue(ap[2].value->value, inter->data.var_deep, inter);
    addVar(name, ap[1].value, ap[2].value, inter, ap[0].value->value->data.dict.dict);
    memFree(name);
    return result->type;
}

ResultType dict_keys(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    Argument *list = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[0].value->value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    for (int index=0; index < MAX_SIZE; index++){
        Var *tmp = ap[0].value->value->data.dict.dict->hashtable[index];
        for (PASS; tmp != NULL; tmp = tmp->next)
            list = connectValueArgument(copyLinkValue(tmp->name_, inter), list);
    }
    makeListValue(list, LINEFILE, L_list, CNEXT_NT);
    freeArgument(list, true);
    return result->type;
}

ResultType dict_iter(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    {
        Argument *dict_iter_arg = makeValueArgument(ap[0].value);
        callBackCore(inter->data.base_obj[B_DICTITER], dict_iter_arg, LINEFILE, 0,
                     CNEXT_NT);
        freeArgument(dict_iter_arg, true);
    }
    return result->type;
}

ResultType dictRepoStrCore(O_FUNC, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo = NULL;
    Value *value = NULL;
    LinkValue *again = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    if (value->type != V_dict){
        setResultError(E_TypeException, INSTANCE_ERROR(dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    again = findAttributes(is_repo ? L"repo_again" : L"str_again", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (again != NULL){
        bool again_ = checkBool(again, LINEFILE, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            makeStringValue(L"{...}", LINEFILE, CNEXT_NT);
            return result->type;
        }
    }

    setBoolAttrible(true, is_repo ? L"repo_again" : L"str_again", LINEFILE, ap[0].value, CNEXT_NT);
    repo = memWidecpy(L"{");
    for (int i = 0, count = 0; i < MAX_SIZE; i++) {
        for (Var *var = value->data.dict.dict->hashtable[i]; var != NULL; var = var->next, count++) {
            wchar_t *name_tmp;
            wchar_t *value_tmp;
            if (count > 0)
                repo = memWidecat(repo, L", ", true, false);

            freeResult(result);
            name_tmp = getRepoStr(var->name_, is_repo, LINEFILE, CNEXT_NT);
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memWidecat(repo, name_tmp, true, false);
            repo = memWidecat(repo, L": ", true, false);

            freeResult(result);
            value_tmp = getRepoStr(var->value, is_repo, LINEFILE, CNEXT_NT);
            if (!CHECK_RESULT(result))
                goto return_;
            repo = memWidecat(repo, value_tmp, true, false);
        }
    }

    freeResult(result);
    repo = memWidecat(repo, L"}", true, false);
    makeStringValue(repo, LINEFILE, CNEXT_NT);

    return_:
    {
        Result tmp;
        setResultCore(&tmp);
        setBoolAttrible(false, is_repo ? L"repo_again" : L"str_again", LINEFILE, ap[0].value, CFUNC_NT(var_list, &tmp, belong));
        if (!RUN_TYPE(tmp.type)) {
            freeResult(result);
            *result = tmp;
        } else
            freeResult(&tmp);
    }
    memFree(repo);
    return result->type;
}

ResultType dict_repo(O_FUNC){
    return dictRepoStrCore(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType dict_str(O_FUNC){
    return dictRepoStrCore(CO_FUNC(arg, var_list, result, belong), false);
}

void registeredDict(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_DICT];
    NameFunc tmp[] = {{L"keys", dict_keys, object_free_},
                      {inter->data.mag_func[M_NEW], dict_new, class_free_},
                      {inter->data.mag_func[M_DOWN], dict_down, object_free_},
                      {inter->data.mag_func[M_ITER], dict_iter, object_free_},
                      {inter->data.mag_func[M_REPO], dict_repo, object_free_},
                      {inter->data.mag_func[M_STR], dict_str, object_free_},
                      {inter->data.mag_func[M_DOWN_ASSIGMENT], dict_down_assignment, object_free_},
                      {inter->data.mag_func[M_DOWN_DEL], dict_down_del, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"dict", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDict(Inter *inter){
    LinkValue *dict = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&dict->gc_status);
    inter->data.base_obj[B_DICT] = dict;
}
