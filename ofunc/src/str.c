#include "__ofunc.h"

ResultType str_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    char *str = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    str = getRepoStr(ap[1].value, false, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    base->value->type = string;
    base->value->data.str.str = memStrcpy(str);
    setResult(result, inter, belong);
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
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
            setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }
    }

    if (!checkSlice(&first, &second, &stride, size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;

    {
        char *str = NULL;
        for (vnum i = first; i < second; i += stride)
            str = memStrCharcpy(str, 1, true, true, ap[0].value->value->data.str.str[i]);
        setResultOperationBase(result, makeLinkValue(makeStringValue(str, inter), belong, inter));
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

    if (ap[0].value->value->type != string || ap[1].value->value->type != number){
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = memStrlen(ap[0].value->value->data.str.str);
    index = ap[1].value->value->data.num.num;
    if (!checkIndex(&index, &size, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        return result->type;
    *element = ap[0].value->value->data.str.str[index];
    setResultOperationBase(result, makeLinkValue(makeStringValue(element, inter), belong, inter));
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
        setResultError(E_TypeException, "Get Not Support Type", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    size = memStrlen(ap[0].value->value->data.str.str);

    {
        Argument *new_list = NULL;
        Argument *back_up = NULL;
        LinkValue *new = NULL;
        for (vnum i = 0; i < size; i ++) {
            char str[2] = {};
            str[0] = ap[0].value->value->data.str.str[i];
            new_list = connectValueArgument(makeLinkValue(makeStringValue(str, inter), belong, inter), new_list);
        }
        back_up = new_list;
        new = makeLinkValue(makeListValue(&new_list, inter, ap[0].value->value->data.list.type), belong, inter);
        setResultOperationBase(result, new);
        freeArgument(back_up, true);
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
        setResultError(E_TypeException, "String cannot be converted to list", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    gc_addTmpLink(&to_list->gc_status);
    callBackCore(to_list, NULL, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    gc_freeTmpLink(&to_list->gc_status);
    if (CHECK_RESULT(result)) {
        LinkValue *str_list = NULL;
        str_list = result->value;
        result->value = NULL;
        freeResult(result);
        getIter(str_list, 1, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&str_list->gc_status);
    }
    return result->type;
}

void registeredStr(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.str, inter->base_father, inter);
    NameFunc tmp[] = {{"to_list", str_to_list, object_free_},
                      {inter->data.object_init, str_init, object_free_},
                      {inter->data.object_iter, str_iter, object_free_},
                      {inter->data.object_down, str_down, object_free_},
                      {inter->data.object_slice, str_slice, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("str", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseStr(Inter *inter){
    Value *str = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&str->gc_status);
    inter->data.str = str;
}
