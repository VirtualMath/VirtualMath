#include "__ofunc.h"

ResultType listiter_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="list_", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *index = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[1].value->value->type != list){
        setResultError(E_TypeException, ONLY_ACC(listiter, list), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    index = makeLinkValue(makeNumberValue(0, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)), ap[0].value, inter);
    if (!CHECK_RESULT(result))
        return result->type;

    freeResult(result);
    if (addAttributes("__list", false, ap[1].value, 0, "listiter.init", ap[0].value, result, inter)) {
        freeResult(result);
        addAttributes("__index", false, index, 0, "listiter.init", ap[0].value, result, inter);
    }

    setResult(result, inter, belong);
    return result->type;
}

ResultType listiter_next(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *list_ = NULL;
    LinkValue *index = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    list_ = findAttributes("__list", false, ap[0].value, inter);
    index = findAttributes("__index", false, ap[0].value, inter);

    if (list_->value->type != list){
        setResultError(E_TypeException, VALUE_ERROR(listiter.__list, list), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    if (index->value->type != number){
        setResultError(E_TypeException, VALUE_ERROR(listiter.__index, number), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }


    freeResult(result);
    elementDownOne(list_, index, 0, "listiter", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        setResultError(E_StopIterException, "Stop Iter", 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else {
        index->value->data.num.num ++;
        if (addAttributes("__index", false, index, 0, "listiter.next", ap[0].value, result, inter))
            setResult(result, inter, belong);
    }
    return result->type;
}

void registeredListIter(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.list_iter, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_init, listiter_init, object_free_},
                      {inter->data.object_next, listiter_next, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar("listiter", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseListIter(Inter *inter){
    Value *list_iter = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&list_iter->gc_status);
    inter->data.list_iter = list_iter;
}
