#include "__ofunc.h"

ResultType list_slice(OfficialFunctionSig){
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
    if (!run_continue(result))
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
        setResultError(result, inter, "IndexException", "Index too max", 0, "sys", belong, true);
        return error_return;
    } else if (first < 0 || second <= 0){
        setResultError(result, inter, "IndexException", "Index too small", 0, "sys", belong, true);
        return error_return;
    }

    if (stride < 0){
        long tmp = first;
        stride = -stride;
        first = second;
        second = tmp;
    }
    if (stride == 0 || first > second){
        setResultError(result, inter, "StrideException", "Stride Error", 0, "sys", belong, true);
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
    setResultError(result, inter, "TypeException", "Get Not Support Type", 0, "sys", belong, true);
    return error_return;
}

ResultType list_down_assignment(OfficialFunctionSig){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    long size;
    long index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!run_continue(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list || ap[2].value->value->type != number){
        setResultError(result, inter, "TypeException", "Get Not Support Type", 0, "sys", belong, true);
        return error_return;
    }
    size = ap[0].value->value->data.list.size;
    index = ap[2].value->value->data.num.num;
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(result, inter, "IndexException", "Index too max", 0, "sys", belong, true);
        return error_return;
    } else if (index < 0){
        setResultError(result, inter, "IndexException", "Index too small", 0, "sys", belong, true);
        return error_return;
    }
    ap[0].value->value->data.list.list[index] = ap[1].value;
    setResultOperationBase(result, ap[1].value);
    return result->type;
}

ResultType list_down(OfficialFunctionSig){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    long size;
    long index;
    LinkValue *element = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!run_continue(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != list || ap[1].value->value->type != number){
        setResultError(result, inter, "TypeException", "Get Not Support Type", 0, "sys", belong, true);
        return error_return;
    }
    size = ap[0].value->value->data.list.size;
    index = ap[1].value->value->data.num.num;
    if (index < 0)
        index = size + index;
    if (index >= size){
        setResultError(result, inter, "IndexException", "Index too max", 0, "sys", belong, true);
        return error_return;
    } else if (index < 0){
        setResultError(result, inter, "IndexException", "Index too small", 0, "sys", belong, true);
        return error_return;
    }
    element = ap[0].value->value->data.list.list[index];
    setResultOperationBase(result, copyLinkValue(element, inter));
    return result->type;
}

void registeredList(RegisteredFunctionSig){
    LinkValue *object = makeLinkValue(inter->data.list, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__down__", list_down, object_free_},
                      {"__slice__", list_slice, object_free_},
                      {"__down_assignment__", list_down_assignment, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("list", false, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    object_backup = object_var->next;
    object_var->next = inter->var_list;
    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseList(Inter *inter){
    Value *list = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&list->gc_status);
    inter->data.list = list;
}
