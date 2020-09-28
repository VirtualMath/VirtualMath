#include "__ofunc.h"

ResultType listiter_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"list_", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *index = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[1].value->value->type != V_list){
        setResultError(E_TypeException, ONLY_ACC(listiter, list), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    index = makeLinkValue(makeNumberValue(0, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)), ap[0].value, inter);
    if (!CHECK_RESULT(result))
        return result->type;

    freeResult(result);
    if (addAttributes(L"__list", false, ap[1].value, 0, "listiter.init", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, ap[0].value))) {
        freeResult(result);
        addAttributes(L"__index", false, index, 0, "listiter.init", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, ap[0].value));
    }

    setResult(result, inter);
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
    freeResult(result);

    list_ = findAttributes(L"__list", false, 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (list_ == NULL){
        setResultError(E_TypeException, VALUE_ERROR(__list, listiter), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    freeResult(result);
    index = findAttributes(L"__index", false, 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (index == NULL){
        setResultError(E_TypeException, VALUE_ERROR(__index, listiter), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    if (list_->value->type != V_list){
        setResultError(E_TypeException, VALUE_ERROR(listiter.__list, list), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }
    if (index->value->type != V_num){
        setResultError(E_TypeException, VALUE_ERROR(listiter.__index, V_num), 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }


    freeResult(result);
    getElement(list_, index, 0, "listiter", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        if (is_indexException(result->value, inter))
            setResultError(E_StopIterException, L"Stop Iter", 0, "listiter", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else
            return result->type;
    else {
        Result tmp_result;
        setResultCore(&tmp_result);
        index->value->data.num.num ++;
        if (addAttributes(L"__index", false, index, 0, "listiter.next", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &tmp_result, ap[0].value)))
            freeResult(&tmp_result);
        else {
            freeResult(result);
            *result = tmp_result;
        }
    }
    return result->type;
}

void registeredListIter(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.list_iter;
    NameFunc tmp[] = {{inter->data.object_init, listiter_init, object_free_},
                      {inter->data.object_next, listiter_next, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"listiter", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseListIter(Inter *inter){
    LinkValue *list_iter = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&list_iter->gc_status);
    inter->data.list_iter = list_iter;
}
