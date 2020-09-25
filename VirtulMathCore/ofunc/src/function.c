#include "__ofunc.h"

static void setFunctionData(Value *value, LinkValue *cls, Inter *inter) {
    value->data.function.function_data.pt_type = inter->data.default_pt_type;
    value->data.function.function_data.cls = cls;
}

ResultType function_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "V_func.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    {
        Inherit *object_father = getInheritFromValueCore(inter->data.function);
        VarList *new_var = copyVarList(var_list, false, inter);
        Value *new_object = makeObject(inter, NULL, new_var, object_father);
        value = makeLinkValue(new_object, belong, inter);
    }

    value->value->type = V_func;
    value->value->data.function.type = vm_func;
    value->value->data.function.function = NULL;
    value->value->data.function.pt = NULL;
    value->value->data.function.of = NULL;
    setFunctionData(value->value, ap->value, inter);

    switch (init_new(value, arg, "V_func.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}

ResultType function_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *func;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if ((func = ap[0].value)->value->type != V_func) {
        setResultError(E_TypeException, INSTANCE_ERROR(V_func), 0, "V_func", true,
                       CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    if (ap[1].value != NULL) {
        Statement *return_ = makeBaseLinkValueStatement(ap[1].value, 0, "sys");
        func->value->data.function.function = makeReturnStatement(return_, 0, "sys");
        func->value->data.function.function_data.pt_type = free_;
        func->value->data.function.type = vm_func;
    }

    setResult(result, inter);
    return result->type;
}

void registeredFunction(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.function;
    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"V_func", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFunction(Inter *inter){
    LinkValue *function = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&function->gc_status);
    inter->data.function = function;
}

void functionPresetting(LinkValue *func, LinkValue **func_new, LinkValue **func_init, Inter *inter) {
    *func_new = makeCFunctionFromOf(function_new, func, function_new, function_init, func, inter->var_list, inter);
    *func_init = makeCFunctionFromOf(function_init, func, function_new, function_init, func, inter->var_list, inter);
    (*func_new)->value->data.function.function_data.pt_type = class_free_;
    (*func_init)->value->data.function.function_data.pt_type = object_free_;
}

void functionPresettingLast(LinkValue *func, LinkValue *func_new, LinkValue *func_init, Inter *inter) {
    Result result;
    VarList *object_var = func->value->object.var;
    setResultCore(&result);

    addStrVar(inter->data.object_new, false, true, func_new, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(object_var, &result, func));
    freeResult(&result);
    addStrVar(inter->data.object_init, false, true, func_init, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(object_var, &result, func));
    freeResult(&result);
}
