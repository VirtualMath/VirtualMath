#include "__ofunc.h"

ResultType dictiter_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"dict_", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *list = NULL;
    LinkValue *list_iter = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[1].value->value->type != V_dict){
        setResultError(E_TypeException, ONLY_ACC(dictiter, dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    {
        LinkValue *keys = NULL;
        Argument *list_arg = NULL;
        LinkValue *listiter_class = NULL;

        freeResult(result);
        keys = findAttributes(L"keys", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[1].value));
        if (!CHECK_RESULT(result))
            return result->type;
        if (keys == NULL){
            setResultError(E_TypeException, L"object non-key-value pairs (there is no keys method)", LINEFILE, true, CNEXT_NT);
            return R_error;
        }

        gc_addTmpLink(&keys->gc_status);
        freeResult(result);
        callBackCore(keys, NULL, LINEFILE, 0, CNEXT_NT);
        gc_freeTmpLink(&keys->gc_status);
        if (!CHECK_RESULT(result))
            return R_error;
        GET_RESULT(list, result);

        listiter_class = inter->data.base_obj[B_LISTITER];
        gc_addTmpLink(&listiter_class->gc_status);

        list_arg = makeValueArgument(list);
        callBackCore(listiter_class, list_arg, LINEFILE, 0,
                     CNEXT_NT);
        freeArgument(list_arg, true);

        if (!CHECK_RESULT(result)) {
            gc_freeTmpLink(&listiter_class->gc_status);
            gc_freeTmpLink(&list->gc_status);
            return R_error;
        }

        GET_RESULT(list_iter, result);
        gc_freeTmpLink(&list->gc_status);
        gc_freeTmpLink(&listiter_class->gc_status);
    }
    freeResult(result);
    if (addAttributes(L"__list", false, list_iter, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value))) {
        freeResult(result);
        addAttributes(L"__dict", false, ap[1].value, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    }
    gc_freeTmpLink(&list_iter->gc_status);
    setResult(result, inter);
    return result->type;
}

ResultType dictiter_next(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *list_ = NULL;
    LinkValue *list_next = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    list_ = findAttributes(L"__list", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (list_ == NULL){
        setResultError(E_TypeException, VALUE_ERROR(__list, listiter), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    freeResult(result);
    list_next = findAttributes(inter->data.mag_func[M_NEXT], false, LINEFILE, true, CFUNC_NT(var_list, result, list_));
    if (!CHECK_RESULT(result))
        return result->type;
    if (list_next == NULL){
        setResultError(E_TypeException, OBJ_NOTSUPPORT(iter), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    freeResult(result);
    callBackCore(list_next, NULL, LINEFILE, 0, CNEXT_NT);
    return result->type;
}

ResultType dictiter_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *dict_ = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    dict_ = findAttributes(L"__dict", false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    if (dict_ == NULL || dict_->value->type != V_dict){
        setResultError(E_TypeException, VALUE_ERROR(__dict, dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    freeResult(result);
    getElement(dict_, ap[1].value, LINEFILE, CNEXT_NT);
    return result->type;
}

void registeredDictIter(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_DICTITER];
    NameFunc tmp[] = {{inter->data.mag_func[M_INIT], dictiter_init, object_free_},
                      {inter->data.mag_func[M_NEXT], dictiter_next, object_free_},
                      {inter->data.mag_func[M_DOWN], dictiter_down, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"dictiter", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDictIter(Inter *inter){
    LinkValue *dict_iter = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&dict_iter->gc_status);
    inter->data.base_obj[B_DICTITER] = dict_iter;
}
