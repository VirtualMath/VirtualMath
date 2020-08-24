#include "__ofunc.h"

ResultType list_slice(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum first;
    vnum second;
    vnum stride;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list) {
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = ap[0].value->value->data.list.size;

    first = 0;
    second = size;
    stride = 1;
    for (vnum *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == number)
            *(list[i]) = ap[i + 1].value->value->data.num.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != none) {
            setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;

    {
        Argument *new_list = NULL;
        Argument *back_up = NULL;
        LinkValue *new = NULL;
        for (long i = first; i < second; i += stride) {
            LinkValue *element = ap[0].value->value->data.list.list[i];
            new_list = connectValueArgument(element, new_list);
        }
        back_up = new_list;
        new = makeLinkValue(makeListValue(&new_list, inter, ap[0].value->value->data.list.type), belong, inter);
        setResultOperationBase(result, new);
        freeArgument(back_up, true);
    }
    return result->type;
}

ResultType list_down_assignment(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list || ap[2].value->value->type != number){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = ap[0].value->value->data.list.size;
    index = ap[2].value->value->data.num.num;
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(E_IndexException, "Index too max", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    } else if (index < 0){
        setResultError(E_IndexException, "Index too small", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    ap[0].value->value->data.list.list[index] = ap[1].value;
    setResultOperationBase(result, ap[1].value);
    return result->type;
}

ResultType list_down(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum index;
    LinkValue *element = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list || ap[1].value->value->type != number){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = ap[0].value->value->data.list.size;
    index = ap[1].value->value->data.num.num;
    if (!checkIndex(&index, &size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;
    element = ap[0].value->value->data.list.list[index];
    setResultOperationBase(result, copyLinkValue(element, inter));
    return result->type;
}

ResultType list_iter(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list){
        setResultError(E_TypeException, "Don't get a list", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        Argument *list_iter_arg = makeValueArgument(ap[0].value);
        LinkValue *iter_list = makeLinkValue(inter->data.list_iter, inter->base_father, inter);
        gc_addTmpLink(&iter_list->gc_status);
        callBackCore(iter_list, list_iter_arg, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&iter_list->gc_status);
        freeArgument(list_iter_arg, true);
    }
    return result->type;
}

ResultType listRepoStrCore(OFFICAL_FUNCTIONSIG, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *repo = NULL;
    Value *value = NULL;
    LinkValue *again = NULL;
    enum ListType lt;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    if (value->type != list){
        setResultError(E_TypeException, "list.__repo__/__list__ gets unsupported data", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    lt = value->data.list.type;
    again = findAttributes(is_repo ? "repo_again" : "str_again", false, ap[0].value, inter);
    if (again != NULL){
        bool again_ = checkBool(again, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            setResultOperation(result, makeLinkValue(makeStringValue(lt == value_list ? "[...]" : "(...)", inter), belong, inter));
            return result->type;
        }
    }

    addAttributes(is_repo ? "repo_again" : "str_again", false, makeLinkValue(makeBoolValue(true, inter), belong, inter), ap[0].value, inter);
    if (lt == value_list)
        repo = memStrcpy("[");
    else
        repo = memStrcpy("(");
    for (int i=0;i < value->data.list.size;i++){
        char *tmp;
        freeResult(result);
        if (i > 0)
            repo = memStrcat(repo, ", ", true, false);
        tmp = getRepoStr(value->data.list.list[i], is_repo, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        repo = memStrcat(repo, tmp, true, false);
    }
    if (lt == value_list)
        repo = memStrcat(repo, "]", true, false);
    else
        repo = memStrcat(repo, ")", true, false);
    setResultOperation(result, makeLinkValue(makeStringValue(repo, inter), belong, inter));

    return_:
    addAttributes(is_repo ? "repo_again" : "str_again", false, makeLinkValue(makeBoolValue(false, inter), belong, inter), ap[0].value, inter);
    memFree(repo);
    return result->type;
}

ResultType list_repo(OFFICAL_FUNCTIONSIG){
    return listRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), true);
}

ResultType list_str(OFFICAL_FUNCTIONSIG){
    return listRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), false);
}

void registeredList(REGISTERED_FUNCTIONSIG){
    {
        LinkValue *object = makeLinkValue(inter->data.tuple, inter->base_father, inter);
        NameFunc tmp[] = {{inter->data.object_down, list_down, object_free_},
                          {inter->data.object_slice, list_slice, object_free_},
                          {inter->data.object_iter, list_iter, object_free_},
                          {inter->data.object_repo, list_repo, object_free_},
                          {inter->data.object_str, list_str, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addStrVar("tuple", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }

    {
        LinkValue *object = makeLinkValue(inter->data.list, inter->base_father, inter);
        NameFunc tmp[] = {{inter->data.object_down_assignment, list_down_assignment, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addStrVar("list", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
}

void makeBaseList(Inter *inter){
    Value *tuple = makeBaseChildClass(inter->data.vobject, inter);
    Value *list = makeBaseChildClass(tuple, inter);
    gc_addStatementLink(&tuple->gc_status);
    gc_addStatementLink(&list->gc_status);
    inter->data.tuple = tuple;
    inter->data.list = list;
}
