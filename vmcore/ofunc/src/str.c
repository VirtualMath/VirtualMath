#include "__ofunc.h"

LinkValue *strCore(LinkValue *belong, LinkValue *class, Inter *inter) {
    LinkValue *value;
    value = make_new(inter, belong, class);
    value->value->type = V_str;
    value->value->data.str.str = NULL;  // 设置为NULL, 因此strCore不能单独使用, strCore()后需要显式设定str的内容
    return value;
}

static ResultType str_new(O_FUNC){
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
    freeResult(result);

    value = make_new(inter, belong, ap[0].value);  // 保持与strCore的行为相同
    value->value->type = V_str;
    value->value->data.str.str = memWidecpy(L"");
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType str_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    wchar_t *str = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value != NULL){
        str = getRepoStr(ap[1].value, false, LINEFILE, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        memFree(base->value->data.str.str);
        base->value->data.str.str = memWidecpy(str);
    }
    setResult(result, inter);
    return result->type;
}

static ResultType str_slice(O_FUNC){
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

    if (ap[0].value->value->type != V_str) {
        setResultError(E_TypeException, INSTANCE_ERROR(str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = memWidelen(ap[0].value->value->data.str.str);

    first = 0;
    second = size;
    stride = 1;
    for (vint *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == V_int)  // 检查是否存在或是否为数字
            *(list[i]) = ap[i + 1].value->value->data.int_.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != V_none) {  // 若不是数字则报错
            setResultError(E_TypeException, VALUE_ERROR(first/second/stride, num or null), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CNEXT_NT))
        return result->type;

    {
        wchar_t *str = NULL;
        for (vint i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride)
            str = memWideCharcpy(str, 1, true, true, ap[0].value->value->data.str.str[i]);
        makeStringValue(str, LINEFILE, CNEXT_NT);
        memFree(str);
    }
    return result->type;
}

static ResultType str_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    wchar_t element[2] = { NUL };
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_str){
        setResultError(E_TypeException, INSTANCE_ERROR(str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    if (ap[1].value->value->type != V_int){
        setResultError(E_TypeException, ONLY_ACC(str index, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    size = memWidelen(ap[0].value->value->data.str.str);
    index = ap[1].value->value->data.int_.num;
    if (!checkIndex(&index, &size, CNEXT_NT))
        return result->type;
    *element = ap[0].value->value->data.str.str[index];
    makeStringValue(element, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType str_to_list(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_str){
        setResultError(E_TypeException, INSTANCE_ERROR(str), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = memWidelen(ap[0].value->value->data.str.str);

    {
        Argument *new_list = NULL;
        for (vint i = 0; i < size; i ++) {
            wchar_t str[2] = { NUL };
            str[0] = ap[0].value->value->data.str.str[i];
            makeStringValue(str, LINEFILE, CNEXT_NT);
            new_list = connectValueArgument(result->value, new_list);
            freeResult(result);
        }
        makeListValue(new_list, LINEFILE, L_list, CNEXT_NT);
        freeArgument(new_list, true);
    }
    return result->type;
}

static ResultType str_iter(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *to_list = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    to_list = findAttributes(L"to_list", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (to_list == NULL){
        setResultError(E_TypeException, L"string cannot be converted to list", LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    gc_addTmpLink(&to_list->gc_status);
    callBackCore(to_list, NULL, LINEFILE, 0, CNEXT_NT);
    gc_freeTmpLink(&to_list->gc_status);
    if (CHECK_RESULT(result)) {  // 若没有出现Exception
        LinkValue *str_list;
        GET_RESULT(str_list, result);
        getIter(str_list, 1, LINEFILE, CNEXT_NT);
        gc_freeTmpLink(&str_list->gc_status);
    }
    return result->type;
}

void registeredStr(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_STR];
    NameFunc tmp[] = {{L"to_list", str_to_list, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_NEW], str_new, fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], str_init, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SLICE], str_slice, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_ITER], str_iter, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DOWN], str_down, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SLICE], str_slice, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    addBaseClassVar(L"str", object, belong, inter);
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseStr(Inter *inter){
    LinkValue *str = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&str->gc_status);
    inter->data.base_obj[B_STR] = str;
}
