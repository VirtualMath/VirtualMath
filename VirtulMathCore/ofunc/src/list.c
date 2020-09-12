#include "__ofunc.h"

ResultType tuple_list_newCore(OFFICAL_FUNCTIONSIG, enum ListType type){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=true},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    value = make_new(inter, belong, ap[0].value);
    value->value->type = list;
    value->value->data.list.type = type;
    value->value->data.list.list = NULL;
    value->value->data.list.size = 0;

    for (Argument *at = ap[1].arg; at != NULL && at->type == value_arg; at = at->next) {
        value->value->data.list.size++;
        value->value->data.list.list = memRealloc(value->value->data.list.list, value->value->data.list.size * sizeof(LinkValue *));
        value->value->data.list.list[value->value->data.list.size - 1] = at->data.value;
    }

    switch (init_new(value, NULL, "list.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }  // TODO-szh 提取成函数
    return result->type;
}

ResultType tuple_new(OFFICAL_FUNCTIONSIG) {
    return tuple_list_newCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), value_tuple);
}

ResultType list_new(OFFICAL_FUNCTIONSIG) {
    return tuple_list_newCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), value_list);
}

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
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, number or none), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;

    {
        Argument *new_list = NULL;
        for (vnum i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            LinkValue *element = ap[0].value->value->data.list.list[i];
            new_list = connectValueArgument(element, new_list);
        }
        makeListValue(new_list, 0, "list.slice", value_list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeArgument(new_list, true);
    }
    return result->type;
}

ResultType list_slice_assignment(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum first;
    vnum second;
    vnum stride;
    LinkValue *iter_obj = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list) {
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = ap[0].value->value->data.list.size;
    getIter(ap[1].value, 1, 0, "list", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    iter_obj = result->value;
    result->value = NULL;
    freeResult(result);

    first = 0;
    second = size;
    stride = 1;
    for (vnum *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 2].value != NULL && ap[i + 2].value->value->type == number)
            *(list[i]) = ap[i + 2].value->value->data.num.num;
        else if (ap[i + 2].value != NULL && ap[i + 2].value->value->type != none) {
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, num or null), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            goto return_;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        goto return_;

    {
        for (vnum i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            freeResult(result);
            getIter(iter_obj, 0, 0, "list", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (is_iterStop(result->value, inter)){
                setResultError(E_TypeException, "Iter Object Too Short", 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                goto return_;
            }
            else if (!CHECK_RESULT(result))
                goto return_;
            ap[0].value->value->data.list.list[i] = result->value;
        }
        freeResult(result);
        getIter(iter_obj, 0, 0, "list", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (CHECK_RESULT(result)) {
            setResultError(E_TypeException, "Iter Object Too Long", 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            goto return_;
        } else if (!is_iterStop(result->value, inter))
            goto return_;
    }
    setResult(result, inter, belong);
    return_:
    gc_freeTmpLink(&iter_obj->gc_status);
    return result->type;
}

ResultType list_slice_del(OFFICAL_FUNCTIONSIG){
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
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, num or null), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;

    {
        LinkValue **new = NULL;
        vnum new_size = size;
        for (vnum i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            ap[0].value->value->data.list.list[i] = NULL;
            new_size --;
        }
        new = memCalloc(new_size, sizeof(LinkValue *));
        for (vnum i = 0, c = 0; i < size; i++) {
            if (ap[0].value->value->data.list.list[i] != NULL){
                new[c] = ap[0].value->value->data.list.list[i];
                c++;
            }
        }
        memFree(ap[0].value->value->data.list.list);
        ap[0].value->value->data.list.list = new;
        ap[0].value->value->data.list.size = new_size;
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

    if (ap[0].value->value->type != list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    if (ap[2].value->value->type != number){
        setResultError(E_TypeException, ONLY_ACC(list index, number), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    size = ap[0].value->value->data.list.size;
    index = ap[2].value->value->data.num.num;
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(E_IndexException, "Index too max", 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    } else if (index < 0){
        setResultError(E_IndexException, "Index less than 0", 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    ap[0].value->value->data.list.list[index] = ap[1].value;
    setResultOperationBase(result, ap[1].value);
    return result->type;
}

ResultType list_down_del(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    if (ap[1].value->value->type != number){
        setResultError(E_TypeException, ONLY_ACC(list index, number), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    size = ap[0].value->value->data.list.size;
    index = ap[1].value->value->data.num.num;
    if (!checkIndex(&index, &size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;
    {
        LinkValue **new = NULL;
        new = memCalloc(size - 1, sizeof(LinkValue *));
        memcpy(new, ap[0].value->value->data.list.list, sizeof(LinkValue *) * index);
        memcpy(new + index, ap[0].value->value->data.list.list + index + 1, sizeof(LinkValue *) * (size - index - 1));
        memFree(ap[0].value->value->data.list.list);
        ap[0].value->value->data.list.list = new;
        ap[0].value->value->data.list.size --;
    }
    setResult(result, inter, belong);
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

    if (ap[0].value->value->type != list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    if (ap[1].value->value->type != number){
        setResultError(E_TypeException, ONLY_ACC(list index, number), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "list", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        Argument *list_iter_arg = makeValueArgument(ap[0].value);
        callBackCore(inter->data.list_iter, list_iter_arg, 0, "list", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
        setResultError(E_TypeException, INSTANCE_ERROR(list), 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    lt = value->data.list.type;
    again = findAttributes(is_repo ? "repo_again" : "str_again", false, ap[0].value, inter);
    if (again != NULL){
        bool again_ = checkBool(again, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            makeStringValue(lt == value_list ? "[...]" : "(...)", 0, "list.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return result->type;
        }
    }

    setBoolAttrible(true, is_repo ? "repo_again" : "str_again", 0, "list.repo", ap[0].value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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

    freeResult(result);
    makeStringValue(repo, 0, "list.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return_:
    {
        Result tmp;
        setResultCore(&tmp);
        setBoolAttrible(false, is_repo ? "repo_again" : "str_again", 0, "list.repo", ap[0].value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &tmp, belong));
        if (!RUN_TYPE(tmp.type)) {
            freeResult(result);
            *result = tmp;
        } else
            freeResult(&tmp);
    }
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
        LinkValue *object = inter->data.tuple;
        NameFunc tmp[] = {{inter->data.object_new, tuple_new, class_free_},
                          {inter->data.object_down, list_down, object_free_},
                          {inter->data.object_slice, list_slice, object_free_},
                          {inter->data.object_iter, list_iter, object_free_},
                          {inter->data.object_repo, list_repo, object_free_},
                          {inter->data.object_str, list_str, object_free_},
                          {inter->data.object_down_del, list_down_del, object_free_},
                          {inter->data.object_slice_del, list_slice_del, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar("tuple", object, belong, inter);
        iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }

    {
        LinkValue *object = inter->data.list;
        NameFunc tmp[] = {{inter->data.object_new, list_new, class_free_},
                          {inter->data.object_down_assignment, list_down_assignment, object_free_},
                          {inter->data.object_slice_assignment, list_slice_assignment, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar("list", object, belong, inter);
        iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
}

void makeBaseList(Inter *inter){
    LinkValue *tuple = makeBaseChildClass4(inter->data.vobject, inter);
    LinkValue *list = makeBaseChildClass4(tuple, inter);
    gc_addStatementLink(&tuple->gc_status);
    gc_addStatementLink(&list->gc_status);
    inter->data.tuple = tuple;
    inter->data.list = list;
}
