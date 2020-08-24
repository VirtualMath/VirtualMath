#include "__ofunc.h"

ResultType list_slice(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    long size;
    long first;
    long second;
    long stride;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list)
        goto type_error;
    size = ap[0].value->value->data.list.size;
    if (ap[1].value == NULL || ap[1].value->value->type == none)
        first = 0;
    else if (ap[1].value->value->type == number)
        first = ap[1].value->value->data.num.num;
    else
        goto type_error;

    if (ap[2].value == NULL || ap[2].value->value->type == none)
        second = size;
    else if (ap[2].value->value->type == number)
        second = ap[2].value->value->data.num.num;
    else
        goto type_error;

    if (ap[3].value == NULL || ap[3].value->value->type == none)
        stride = 1;
    else if (ap[3].value->value->type == number)
        stride = ap[3].value->value->data.num.num;
    else
        goto type_error;

    first = first < 0 ? first + size : first;
    second = second < 0 ? second + size : second;
    if (second > size || first >= size){
        setResultError(E_IndexException, "Index too max", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    } else if (first < 0 || second <= 0){
        setResultError(E_IndexException, "Index too small", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    if (stride < 0){
        long tmp = first;
        stride = -stride;
        first = second;
        second = tmp;
    }
    if (stride == 0 || first > second){
        setResultError(E_StrideException, "Stride Error", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

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

    type_error:
    setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return error_return;
}

ResultType list_down_assignment(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    long size;
    long index;
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
    long size;
    long index;
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
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(E_IndexException, "Index too max", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    } else if (index < 0){
        setResultError(E_IndexException, "Index too small", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
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

ResultType list_repo(OFFICAL_FUNCTIONSIG){
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
        setResultError(E_TypeException, "list.__repo__ gets unsupported data", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    lt = value->data.list.type;
    again = findAttributes("repo_again", false, ap[0].value, inter);
    if (again != NULL){
        bool again_ = checkBool(again, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            setResultOperation(result, makeLinkValue(makeStringValue(lt == value_list ? "[...]" : "(...)", inter), belong, inter));
            return result->type;
        }
    }

    addAttributes("repo_again", false, makeLinkValue(makeBoolValue(true, inter), belong, inter), ap[0].value, inter);
    if (lt == value_list)
        repo = memStrcpy("[");
    else
        repo = memStrcpy("(");
    for (int i=0;i < value->data.list.size;i++){
        char *tmp;
        freeResult(result);
        if (i > 0)
            repo = memStrcat(repo, ", ", true, false);
        tmp = getRepo(value->data.list.list[i], 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
    addAttributes("repo_again", false, makeLinkValue(makeBoolValue(false, inter), belong, inter), ap[0].value, inter);
    memFree(repo);
    return result->type;
}

void registeredList(REGISTERED_FUNCTIONSIG){
    {
        LinkValue *object = makeLinkValue(inter->data.tuple, inter->base_father, inter);
        NameFunc tmp[] = {{"__down__", list_down, object_free_},
                          {"__slice__", list_slice, object_free_},
                          {"__iter__", list_iter, object_free_},
                          {"__repo__", list_repo, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addStrVar("tuple", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }

    {
        LinkValue *object = makeLinkValue(inter->data.list, inter->base_father, inter);
        NameFunc tmp[] = {{"__down_assignment__", list_down_assignment, object_free_},
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
