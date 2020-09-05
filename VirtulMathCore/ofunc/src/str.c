#include "__ofunc.h"

ResultType str_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "str.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    setResultCore(result);
    value = make_new(inter, belong, ap[0].value);
    value->value->type = string;
    value->value->data.str.str = memStrcpy("");
    switch (init_new(value, arg, "str.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}

ResultType str_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    char *str = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value != NULL){
        str = getRepoStr(ap[1].value, false, 0, "str.init", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        memFree(base->value->data.str.str);
        base->value->data.str.str = memStrcpy(str);
    }
    setResultBase(result, inter);
    return result->type;
}

ResultType str_slice(OFFICAL_FUNCTIONSIG){
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

    if (ap[0].value->value->type != string) {
        setResultError(E_TypeException, INSTANCE_ERROR(str), 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = memStrlen(ap[0].value->value->data.str.str);

    first = 0;
    second = size;
    stride = 1;
    for (vnum *list[]={&first, &second, &stride}, i=0; i < 3; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == number)
            *(list[i]) = ap[i + 1].value->value->data.num.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != none) {
            setResultError(E_TypeException, VALUE_ERROR(first/second/stride, num or null), 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;

    {
        char *str = NULL;
        for (vnum i = stride > 0 ? first : second; stride > 0 ? (i < second) : (i > first); i += stride)
            str = memStrCharcpy(str, 1, true, true, ap[0].value->value->data.str.str[i]);
        makeStringValue(str, 0, "str.slice", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        memFree(str);
    }
    return result->type;
}

ResultType str_down(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vnum size;
    vnum index;
    char element[2] = {};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != string){
        setResultError(E_TypeException, INSTANCE_ERROR(str), 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    if (ap[1].value->value->type != number){
        setResultError(E_TypeException, ONLY_ACC(str index, number), 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    size = memStrlen(ap[0].value->value->data.str.str);
    index = ap[1].value->value->data.num.num;
    if (!checkIndex(&index, &size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;
    *element = ap[0].value->value->data.str.str[index];
    makeStringValue(element, 0, "str.down", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType str_to_list(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vnum size;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != string){
        setResultError(E_TypeException, INSTANCE_ERROR(str), 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = memStrlen(ap[0].value->value->data.str.str);

    {
        Argument *new_list = NULL;
        for (vnum i = 0; i < size; i ++) {
            char str[2] = {};
            str[0] = ap[0].value->value->data.str.str[i];
            makeStringValue(str, 0, "str.to_list", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            new_list = connectValueArgument(result->value, new_list);
            freeResult(result);
        }
        makeListValue(new_list, 0, "str", value_list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeArgument(new_list, true);
    }
    return result->type;
}

ResultType str_iter(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *to_list = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    to_list = findAttributes("to_list", false, ap[0].value, inter);
    if (to_list == NULL){
        setResultError(E_TypeException, "String cannot be converted to list", 0, "str", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    gc_addTmpLink(&to_list->gc_status);
    callBackCore(to_list, NULL, 0, "str", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    gc_freeTmpLink(&to_list->gc_status);
    if (CHECK_RESULT(result)) {
        LinkValue *str_list = NULL;
        str_list = result->value;
        result->value = NULL;
        freeResult(result);
        getIter(str_list, 1, 0, "str", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&str_list->gc_status);
    }
    return result->type;
}

void registeredStr(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.str, inter->base_father, inter);
    NameFunc tmp[] = {{"to_list", str_to_list, object_free_},
                      {inter->data.object_iter, str_iter, object_free_},
                      {inter->data.object_down, str_down, object_free_},
                      {inter->data.object_slice, str_slice, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    addBaseClassVar("str", object, belong, inter);
    gc_freeTmpLink(&object->gc_status);
}

LinkValue *callClassOf(LinkValue *obj, Inter *inter, LinkValue *new_func, LinkValue *init_func) {
    Argument *arg = makeValueArgument(obj);
    Result result;
    LinkValue *new_name;
    setResultCore(&result);

    new_func->value->data.function.of(arg, obj, &result, inter, new_func->value->object.out_var);
    new_name = result.value;
    freeResult(&result);

    init_func->value->data.function.of(arg, new_name, &result, inter, init_func->value->object.out_var);
    freeResult(&result);
    freeArgument(arg, true);

    return new_name;
}

LinkValue *makeStrFromOf(LinkValue *str, LinkValue *new, LinkValue *init, char *str_, Inter *inter) {
    LinkValue *return_;
    return_ = callClassOf(str, inter, new, init);
    memFree(return_->value->data.str.str);
    return_->value->data.str.str = memStrcpy(str_);
    return return_;
}

LinkValue *makeFunctionFromValue(LinkValue *func, LinkValue *new, LinkValue *init, OfficialFunction of, LinkValue *belong, VarList *var_list, Inter *inter) {
    LinkValue *new_func;
    new_func = callClassOf(func, inter, new, init);
    new_func->value->data.function.type = c_function;
    new_func->value->data.function.of = of;
    new_func->value->data.function.function_data.pt_type = inter->data.default_pt_type;
    for (VarList *vl = new_func->value->object.out_var, *vl_next; vl != NULL; vl = vl_next) {
        vl_next = vl->next;
        freeVarList(vl);
    }
    new_func->value->object.out_var = copyVarList(var_list, false, inter);
    new_func->belong = belong;
    return new_func;
}

void strFunctionPresetting(LinkValue *func, LinkValue *func_new, LinkValue *func_init, Inter *inter) {
    Value *str = inter->data.str;
    LinkValue *obj = makeLinkValue(str, inter->base_father, inter);

    LinkValue *new_func = NULL;
    LinkValue *new_name = NULL;
    char *new_name_ = setStrVarName(inter->data.object_new, false, inter);

    LinkValue *init_func = NULL;
    LinkValue *init_name = NULL;
    char *init_name_ = setStrVarName(inter->data.object_init, false, inter);

    new_func = makeFunctionFromValue(func, func_new, func_init, str_new, obj, str->object.var, inter);
    new_func->value->data.function.function_data.pt_type = class_free_;
    init_func = makeFunctionFromValue(func, func_new, func_init, str_init, obj, str->object.var, inter);
    init_func->value->data.function.function_data.pt_type = object_free_;


    new_name = makeStrFromOf(obj, new_func, init_func, inter->data.object_new, inter);
    init_name = makeStrFromOf(obj, new_func, init_func, inter->data.object_init, inter);

    addFromVarList(new_name_, new_name, 0, new_func, CALL_INTER_FUNCTIONSIG_CORE(str->object.var));
    addFromVarList(init_name_, init_name, 0, init_func, CALL_INTER_FUNCTIONSIG_CORE(str->object.var));

    newObjectSettingPresetting(new_func, new_name, inter);
    newObjectSettingPresetting(init_func, init_name, inter);
    memFree(new_name_);
    memFree(init_name_);
}

void makeBaseStr(Inter *inter){
    Value *str = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&str->gc_status);
    inter->data.str = str;
}
