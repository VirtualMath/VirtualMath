#include "__ofunc.h"

ResultType str_new(O_FUNC){
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

    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_str;
    value->value->data.str.str = memWidecpy(L"");
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType str_init(O_FUNC){
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
    setResultBase(result, inter);
    return result->type;
}

ResultType str_slice(O_FUNC){
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
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == V_int)
            *(list[i]) = ap[i + 1].value->value->data.int_.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != V_none) {
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

ResultType str_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    wchar_t element[2];  // TODO-szh 设置为空
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
        setResultError(E_TypeException, ONLY_ACC(str index, V_int), LINEFILE, true, CNEXT_NT);
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

ResultType str_to_list(O_FUNC){
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
            wchar_t str[2] = { NUL };  // TODO-szh 设置为空
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

ResultType str_iter(O_FUNC){
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
        LinkValue *str_list = NULL;
        str_list = result->value;
        result->value = NULL;
        freeResult(result);
        getIter(str_list, 1, LINEFILE, CNEXT_NT);
        gc_freeTmpLink(&str_list->gc_status);
    }
    return result->type;
}

void registeredStr(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_STR];
    NameFunc tmp[] = {{L"to_list", str_to_list, object_free_},
                      {inter->data.mag_func[M_ITER], str_iter, object_free_},
                      {inter->data.mag_func[M_DOWN], str_down, object_free_},
                      {inter->data.mag_func[M_SLICE], str_slice, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    addBaseClassVar(L"str", object, belong, inter);
    gc_freeTmpLink(&object->gc_status);
}

LinkValue *callClassOf(LinkValue *obj, Inter *inter, LinkValue *new_func, LinkValue *init_func) {
    Argument *arg;
    Argument *init_arg;
    Result result;
    LinkValue *new_name;
    setResultCore(&result);

    arg = makeValueArgument(obj);
    new_func->value->data.function.of(arg, obj, &result, inter, new_func->value->object.out_var);
    new_name = result.value;
    freeResult(&result);
    freeArgument(arg, true);

    init_arg = makeValueArgument(new_name);
    init_func->value->data.function.of(init_arg, new_name, &result, inter, init_func->value->object.out_var);
    freeResult(&result);
    freeArgument(init_arg, true);

    return new_name;
}

LinkValue *makeStrFromOf(LinkValue *str, LinkValue *new, LinkValue *init, wchar_t *str_, Inter *inter) {
    LinkValue *return_;
    return_ = callClassOf(str, inter, new, init);
    memFree(return_->value->data.str.str);
    return_->value->data.str.str = memWidecpy(str_);
    return return_;
}

LinkValue *makeFunctionFromValue(LinkValue *func, LinkValue *new, LinkValue *init, OfficialFunction of, LinkValue *belong, VarList *var_list, Inter *inter) {
    LinkValue *new_func;
    new_func = callClassOf(func, inter, new, init);
    new_func->value->data.function.type = c_func;
    new_func->value->data.function.of = of;
    new_func->value->data.function.function_data.pt_type = inter->data.default_pt_type;
    for (VarList *vl = new_func->value->object.out_var; vl != NULL; vl = freeVarList(vl))
        PASS;
    new_func->value->object.out_var = copyVarList(var_list, false, inter);
    new_func->belong = belong;
    return new_func;
}

void strFunctionPresetting(LinkValue *func, LinkValue *func_new, LinkValue *func_init, Inter *inter) {
    LinkValue *obj = inter->data.base_obj[B_STR];

    LinkValue *new_func = NULL;
    LinkValue *new_name = NULL;
    wchar_t *new_name_ = setStrVarName(inter->data.mag_func[M_NEW], false, inter);

    LinkValue *init_func = NULL;
    LinkValue *init_name = NULL;
    wchar_t *init_name_ = setStrVarName(inter->data.mag_func[M_INIT], false, inter);

    new_func = makeFunctionFromValue(func, func_new, func_init, str_new, obj, obj->value->object.var, inter);
    new_func->value->data.function.function_data.pt_type = class_free_;
    init_func = makeFunctionFromValue(func, func_new, func_init, str_init, obj, obj->value->object.var, inter);
    init_func->value->data.function.function_data.pt_type = object_free_;


    new_name = makeStrFromOf(obj, new_func, init_func, inter->data.mag_func[M_NEW], inter);
    init_name = makeStrFromOf(obj, new_func, init_func, inter->data.mag_func[M_INIT], inter);

    addFromVarList(new_name_, new_name, 0, new_func, CFUNC_CORE(obj->value->object.var));
    addFromVarList(init_name_, init_name, 0, init_func, CFUNC_CORE(obj->value->object.var));

    newObjectSettingPresetting(new_func, new_name, inter);
    newObjectSettingPresetting(init_func, init_name, inter);
    memFree(new_name_);
    memFree(init_name_);
}

void makeBaseStr(Inter *inter){
    LinkValue *str = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&str->gc_status);
    inter->data.base_obj[B_STR] = str;
}
