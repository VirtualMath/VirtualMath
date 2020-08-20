#include "__ofunc.h"

ResultType vm_super(OfficialFunctionSig){
    Value *arg_father = NULL;
    Value *arg_child = NULL;
    LinkValue *next_father = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name="class_", .must=1, .long_arg=false},
                           {.type=name_value, .name="obj_", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!run_continue(result))
            return result->type;
        freeResult(result);
    }

    arg_father = ap[0].value->value;
    arg_child = ap[1].value->value;
    if (arg_child == arg_father) {
        if (arg_child->object.inherit != NULL){
            result->value = copyLinkValue(arg_child->object.inherit->value, inter);
            result->type = operation_return;
            gc_addTmpLink(&result->value->gc_status);
        } else
            setResultError(result, inter, "SuperException", "Don't get next father", 0, "sys", belong, true);
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
        result->type = operation_return;
        gc_addTmpLink(&result->value->gc_status);
    }
    else
        setResultError(result, inter, "SuperException", "Don't get next father", 0, "sys", belong, true);

    return result->type;
}

ResultType vm_setMethodCore(OfficialFunctionSig, enum FunctionPtType type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name="func", .must=1, .long_arg=false}, {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!run_continue(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    function_value->value->data.function.function_data.pt_type = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = operation_return;
    return operation_return;
}

ResultType vm_freemethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), free_);
}

ResultType vm_staticmethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), static_);
}

ResultType vm_classmethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), class_static_);
}

ResultType vm_objectmethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), object_static_);
}

ResultType vm_classfreemethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), class_free_);
}

ResultType vm_objectfreemethod(OfficialFunctionSig){
    return vm_setMethodCore(CALL_OfficialFunction(arg, var_list, result, belong), object_free_);
}

void registeredSysFunction(RegisteredFunctionSig){
    NameFunc tmp[] = {{"super", vm_super, free_},
                      {"freemethod", vm_freemethod, free_},
                      {"staticmethod", vm_staticmethod, free_},
                      {"staticclassmethod", vm_classmethod, free_},
                      {"staticobjectmethod", vm_objectmethod, free_},
                      {"classmethod", vm_classfreemethod, free_},
                      {"objectmethod", vm_objectfreemethod, free_},
                      {NULL, NULL}};
    iterNameFunc(tmp, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
