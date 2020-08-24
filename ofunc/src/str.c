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

void registeredStr(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.str, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_init, str_init, object_free_},
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
