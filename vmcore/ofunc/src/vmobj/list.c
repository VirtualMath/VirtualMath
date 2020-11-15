#include "__ofunc.h"

static ResultType tuple_list_newCore(O_FUNC, enum ListType type){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=true},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_list;
    value->value->data.list.type = type;
    value->value->data.list.list = NULL;
    value->value->data.list.size = 0;

    for (Argument *at = ap[1].arg; at != NULL && at->type == value_arg; at = at->next) {
        value->value->data.list.size++;
        value->value->data.list.list = memRealloc(value->value->data.list.list, value->value->data.list.size * sizeof(LinkValue *));
        value->value->data.list.list[value->value->data.list.size - 1] = at->data.value;
    }

    run_init(value, NULL, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType tuple_new(O_FUNC) {
    return tuple_list_newCore(CO_FUNC(arg, var_list, result, belong), L_tuple);
}

static ResultType list_new(O_FUNC) {
    return tuple_list_newCore(CO_FUNC(arg, var_list, result, belong), L_list);
}

static ResultType list_slice(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint first;
    vint second;
    vint stride;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list) {
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = ap[0].value->value->data.list.size;

    first = 0;
    second = size;
    stride = 1;
    for (vint *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == V_int)
            *(list[i]) = ap[i + 1].value->value->data.int_.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != V_none) {
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, num or null), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CNEXT_NT))
        return result->type;

    {
        Argument *new_list = NULL;
        for (vint i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            LinkValue *element = ap[0].value->value->data.list.list[i];
            new_list = connectValueArgument(element, new_list);
        }
        makeListValue(new_list, LINEFILE, L_list, CNEXT_NT);
        freeArgument(new_list, true);
    }
    return result->type;
}

static ResultType list_slice_assignment(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint first;
    vint second;
    vint stride;
    LinkValue *iter_obj = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list) {
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = ap[0].value->value->data.list.size;
    getIter(ap[1].value, 1, LINEFILE, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    GET_RESULT(iter_obj, result);

    first = 0;
    second = size;
    stride = 1;
    for (vint *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 2].value != NULL && ap[i + 2].value->value->type == V_int)
            *(list[i]) = ap[i + 2].value->value->data.int_.num;
        else if (ap[i + 2].value != NULL && ap[i + 2].value->value->type != V_none) {
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, num or null), LINEFILE, true, CNEXT_NT);
            goto return_;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CNEXT_NT))
        goto return_;

    {
        for (vint i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            freeResult(result);
            getIter(iter_obj, 0, LINEFILE, CNEXT_NT);
            if (is_iterStop(result->value, inter)){
                setResultError(E_TypeException, L"iter Object Too Short", LINEFILE, true, CNEXT_NT);
                goto return_;
            }
            else if (!CHECK_RESULT(result))
                goto return_;
            ap[0].value->value->data.list.list[i] = result->value;
        }
        freeResult(result);
        getIter(iter_obj, 0, LINEFILE, CNEXT_NT);
        if (CHECK_RESULT(result)) {  // 若没有出现Exception
            setResultError(E_TypeException, L"iter Object Too Long", LINEFILE, true, CNEXT_NT);
            goto return_;
        } else if (!is_iterStop(result->value, inter))
            goto return_;
    }
    setResult(result, inter);
    return_:
    gc_freeTmpLink(&iter_obj->gc_status);
    return result->type;
}

static ResultType list_slice_del(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint first;
    vint second;
    vint stride;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list) {
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = ap[0].value->value->data.list.size;

    first = 0;
    second = size;
    stride = 1;
    for (vint *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == V_int)
            *(list[i]) = ap[i + 1].value->value->data.int_.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != V_none) {
            setResultError(E_TypeException, ONLY_ACC(first/second/stride, num or null), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CNEXT_NT))
        return result->type;

    {
        LinkValue **new = NULL;
        vint new_size = size;
        for (vint i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride) {
            ap[0].value->value->data.list.list[i] = NULL;
            new_size --;
        }
        new = memCalloc(new_size, sizeof(LinkValue *));
        for (vint i = 0, c = 0; i < size; i++) {
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

static ResultType list_down_assignment(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    if (ap[2].value->value->type != V_int){
        setResultError(E_TypeException, ONLY_ACC(list index, num), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    size = ap[0].value->value->data.list.size;
    index = ap[2].value->value->data.int_.num;
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(E_IndexException, L"index too max", LINEFILE, true, CNEXT_NT);
        return R_error;
    } else if (index < 0){
        setResultError(E_IndexException, L"index less than 0", LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    ap[0].value->value->data.list.list[index] = ap[1].value;
    setResultOperationBase(result, ap[1].value);
    return result->type;
}

static ResultType list_down_del(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    if (ap[1].value->value->type != V_int){
        setResultError(E_TypeException, ONLY_ACC(list index, V_int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    size = ap[0].value->value->data.list.size;
    index = ap[1].value->value->data.int_.num;
    if (!checkIndex(&index, &size, CNEXT_NT))
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
    setResult(result, inter);
    return result->type;
}

static ResultType list_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    LinkValue *element = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    if (ap[1].value->value->type != V_int){
        setResultError(E_TypeException, ONLY_ACC(list index, V_int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    size = ap[0].value->value->data.list.size;
    index = ap[1].value->value->data.int_.num;
    if (!checkIndex(&index, &size, CNEXT_NT))
        return result->type;
    element = ap[0].value->value->data.list.list[index];
    setResultOperationBase(result, COPY_LINKVALUE(element, inter));
    return result->type;
}

static ResultType list_iter(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    {
        Argument *list_iter_arg = makeValueArgument(ap[0].value);
        callBackCore(inter->data.base_obj[B_LISTITER], list_iter_arg, LINEFILE, 0, CNEXT_NT);
        freeArgument(list_iter_arg, true);
    }
    return result->type;
}

static ResultType listRepoStrCore(O_FUNC, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo = NULL;
    Value *value = NULL;
    LinkValue *again = NULL;
    enum ListType lt;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    if (value->type != V_list){
        setResultError(E_TypeException, INSTANCE_ERROR(list), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    lt = value->data.list.type;
    again = findAttributes(is_repo ? L"repo_again" : L"str_again", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (again != NULL){
        bool again_ = checkBool(again, LINEFILE, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        if (again_) {
            makeStringValue((lt == L_list ? L"[...]" : L"(...)"), LINEFILE, CNEXT_NT);
            return result->type;
        }
    }

    setBoolAttrible(true, is_repo ? L"repo_again" : L"str_again", LINEFILE, ap[0].value, CNEXT_NT);
    if (lt == L_list)
        repo = memWidecpy(L"[");
    else
        repo = memWidecpy(L"(");
    for (int i=0;i < value->data.list.size;i++){
        wchar_t *tmp;
        freeResult(result);
        if (i > 0)
            repo = memWidecat(repo, L", ", true, false);
        tmp = getRepoStr(value->data.list.list[i], is_repo, LINEFILE, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto return_;
        repo = memWidecat(repo, tmp, true, false);
    }
    if (lt == L_list)
        repo = memWidecat(repo, L"]", true, false);
    else
        repo = memWidecat(repo, L")", true, false);

    freeResult(result);
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

static ResultType list_repo(O_FUNC){
    return listRepoStrCore(CO_FUNC(arg, var_list, result, belong), true);
}

static ResultType list_str(O_FUNC){
    return listRepoStrCore(CO_FUNC(arg, var_list, result, belong), false);
}

void registeredList(R_FUNC){
    {
        LinkValue *object = inter->data.base_obj[B_TUPLE];
        NameFunc tmp[] = {{inter->data.mag_func[M_NEW], tuple_new, fp_class},
                          {inter->data.mag_func[M_DOWN], list_down, fp_obj},
                          {inter->data.mag_func[M_SLICE], list_slice, fp_obj},
                          {inter->data.mag_func[M_ITER], list_iter, fp_obj},
                          {inter->data.mag_func[M_REPO], list_repo, fp_obj},
                          {inter->data.mag_func[M_STR], list_str, fp_obj},
                          {inter->data.mag_func[M_DOWN_DEL], list_down_del, fp_obj},
                          {inter->data.mag_func[M_SLICE_DEL], list_slice_del, fp_obj},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar(L"tuple", object, belong, inter);
        iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }

    {
        LinkValue *object = inter->data.base_obj[B_LIST];
        NameFunc tmp[] = {{inter->data.mag_func[M_NEW], list_new, fp_class, .var=nfv_notpush},
                          {inter->data.mag_func[M_DOWN_ASSIGMENT], list_down_assignment, fp_obj, .var=nfv_notpush},
                          {inter->data.mag_func[M_SLICE_ASSIGMENT], list_slice_assignment, fp_obj, .var=nfv_notpush},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar(L"list", object, belong, inter);
        iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
}

void makeBaseList(Inter *inter){
    LinkValue *tuple = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    LinkValue *list = makeBaseChildClass(tuple, inter);
    gc_addStatementLink(&tuple->gc_status);
    gc_addStatementLink(&list->gc_status);
    inter->data.base_obj[B_TUPLE] = tuple;
    inter->data.base_obj[B_LIST] = list;
}
