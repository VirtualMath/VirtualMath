#include "__ofunc.h"

ResultType dictiter_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="dict_", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *list = NULL;
    LinkValue *list_iter = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (ap[1].value->value->type != dict){
        setResultError(E_TypeException, "Don't get a dict to dictiter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    {
        LinkValue *keys = findAttributes("keys", false, ap[1].value, inter);
        Argument *list_arg = NULL;
        LinkValue *listiter_class = NULL;

        if (keys == NULL){
            setResultError(E_TypeException, "Don't Find keys", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
        }

        gc_addTmpLink(&keys->gc_status);
        freeResult(result);
        callBackCore(keys, NULL, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&keys->gc_status);
        if (!CHECK_RESULT(result)) {
            return error_return;
        }
        list = result->value;
        result->value = NULL;

        listiter_class = makeLinkValue(inter->data.list_iter, inter->base_father, inter);
        gc_addTmpLink(&listiter_class->gc_status);

        list_arg = makeValueArgument(list);
        freeResult(result);
        callBackCore(listiter_class, list_arg, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeArgument(list_arg, true);

        if (!CHECK_RESULT(result)) {
            gc_freeTmpLink(&listiter_class->gc_status);
            gc_freeTmpLink(&list->gc_status);
            return error_return;
        }

        list_iter = result->value;
        result->value = NULL;
        freeResult(result);
        gc_freeTmpLink(&list->gc_status);
        gc_freeTmpLink(&listiter_class->gc_status);
    }
    addAttributes("__list", false, list_iter, ap[0].value, inter);
    addAttributes("__dict", false, ap[1].value, ap[0].value, inter);
    gc_freeTmpLink(&list_iter->gc_status);
    setResult(result, inter, belong);
    return result->type;
}

ResultType dictiter_next(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *list_ = NULL;
    LinkValue *list_next = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    list_ = findAttributes("__list", false, ap[0].value, inter);
    if (list_ == NULL){
        setResultError(E_TypeException, "Don't get a list to listiter from dictiter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    list_next = findAttributes(inter->data.object_next, false, list_, inter);
    if (list_next == NULL){
        setResultError(E_TypeException, "Don't find __next__", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    callBackCore(list_next, NULL, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType dictiter_down(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *dict_ = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    dict_ = findAttributes("__dict", false, ap[0].value, inter);
    if (dict_->value->type != dict){
        setResultError(E_TypeException, "Don't get a dict to dictiter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    freeResult(result);
    elementDownOne(dict_, ap[1].value, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

void registeredDictIter(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.dict_iter, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__init__", dictiter_init, object_free_},
                      {"__next__", dictiter_next, object_free_},
                      {"__down__", dictiter_down, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("dictiter", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

    object_backup = object_var->next;
    object_var->next = inter->var_list;
    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDictIter(Inter *inter){
    Value *dict_iter = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&dict_iter->gc_status);
    inter->data.dict_iter = dict_iter;
}
