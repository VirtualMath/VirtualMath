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
        setResultError(E_TypeException, "Don't get a list to listiter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    index = makeLinkValue(makeNumberValue(0, inter), ap[0].value, inter);
    addAttributes("__list", false, ap[1].value, ap[0].value, inter);
    addAttributes("__index", false, index, ap[0].value, inter);
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
    if (list_->value->type != list || index->value->type != number){
        setResultError(E_TypeException, "Don't get a list to listiter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    freeResult(result);
    elementDownOne(list_, index, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        setResultError(E_StopIterException, "Stop Iter", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else {
        index->value->data.num.num ++;
        addAttributes("__index", false, index, ap[0].value, inter);
    }
    return result->type;
}

void registeredListIter(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.list_iter, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__init__", listiter_init, object_free_},
                      {"__next__", listiter_next, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("listiter", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

    object_backup = object_var->next;
    object_var->next = inter->var_list;
    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseListIter(Inter *inter){
    Value *list_iter = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&list_iter->gc_status);
    inter->data.list_iter = list_iter;
}
