#include "__ofunc.h"

ResultType vm_super(O_FUNC){
    Value *arg_father = NULL;
    Value *arg_child = NULL;
    LinkValue *next_father = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"class_", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"obj_", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    arg_father = ap[0].value->value;
    arg_child = ap[1].value->value;
    if (arg_child == arg_father) {
        if (arg_child->object.inherit != NULL){
            result->value = copyLinkValue(arg_child->object.inherit->value, inter);
            result->type = R_opt;
            gc_addTmpLink(&result->value->gc_status);
        } else
            setResultError(E_SuperException, L"Object has no next father", 0, "super", true, CFUNC_NT(var_list, result, belong));
        return result->type;
    }

    for (Inherit *self_father = arg_child->object.inherit; self_father != NULL; self_father = self_father->next) {
        if (self_father->value->value == arg_father) {
            if (self_father->next != NULL)
                next_father = copyLinkValue(self_father->next->value, inter);
            break;
        }
    }

    if (next_father != NULL){
        result->value = next_father;
        result->type = R_opt;
        gc_addTmpLink(&result->value->gc_status);
    }
    else
        setResultError(E_SuperException, L"Object has no next father", 0, "super", true, CFUNC_NT(var_list, result, belong));

    return result->type;
}

ResultType vm_setNowRunCore(O_FUNC, bool type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false}, {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    function_value->value->data.function.function_data.run = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

ResultType vm_setMethodCore(O_FUNC, enum FunctionPtType type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false}, {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    function_value->value->data.function.function_data.pt_type = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

ResultType vm_clsfreemethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), cls_free_);
}

ResultType vm_clsmethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), cls_static_);
}

ResultType vm_freemethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), free_);
}

ResultType vm_staticmethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), static_);
}

ResultType vm_classmethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), class_static_);
}

ResultType vm_objectmethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), object_static_);
}

ResultType vm_classfreemethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), class_free_);
}

ResultType vm_objectfreemethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), object_free_);
}

ResultType vm_allfreemethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), all_free_);
}

ResultType vm_allstaticmethod(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), all_static_);
}

ResultType vm_isnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType vm_disnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), false);
}

ResultType vm_quit(O_FUNC){
    if (arg != NULL)
        setResultError(E_ArgumentException, MANY_ARG, 0, "sys", true, CFUNC_NT(var_list, result, belong));
    else
        setResultError(E_QuitException, L"VirtualMath Quit", 0, "sys", true, CFUNC_NT(var_list, result, belong));
    return R_error;
}

void registeredSysFunction(R_FUNC){
    NameFunc tmp[] = {{L"super", vm_super, free_},
                      {L"freemethod", vm_freemethod, free_},
                      {L"staticmethod", vm_staticmethod, free_},
                      {L"staticclassmethod", vm_classmethod, free_},
                      {L"staticobjectmethod", vm_objectmethod, free_},
                      {L"classmethod", vm_classfreemethod, free_},
                      {L"objectmethod", vm_objectfreemethod, free_},
                      {L"simplemethod", vm_allfreemethod, free_},
                      {L"simplestaticmethod", vm_allstaticmethod, free_},
                      {L"clsmethod", vm_clsfreemethod, free_},
                      {L"clsstaticmethod", vm_clsmethod, free_},
                      {L"isnowrun", vm_isnowrun, free_},
                      {L"disnowrun", vm_disnowrun, free_},
                      {L"quit", vm_quit, free_},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}
