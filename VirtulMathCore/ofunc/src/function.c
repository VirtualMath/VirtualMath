#include "__ofunc.h"

static void setFunctionData(Value *value, Inter *inter) {
    value->data.function.function_data.pt_type = inter->data.default_pt_type;
}

ResultType function_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "function.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    {
        Inherit *object_father = getInheritFromValue(inter->data.function, inter);
        VarList *new_var = copyVarList(var_list, false, inter);
        Value *new_object = makeObject(inter, NULL, new_var, object_father);
        value = makeLinkValue(new_object, belong, inter);
    }

    value->value->type = function;
    value->value->data.function.type = vm_function;
    value->value->data.function.function = NULL;
    value->value->data.function.pt = NULL;
    value->value->data.function.of = NULL;
    setFunctionData(value->value, inter);

    switch (init_new(value, arg, "function.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}


// TODO-szh 函数执行的时候，function的st为NULL的时候, 直接返回null
ResultType function_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    // TODO-szh 添加操作行为
    setResult(result, inter, belong);
    return result->type;
}

void registeredFunction(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.function, inter->base_father, inter);
    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar("function", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFunction(Inter *inter){
    Value *function = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&function->gc_status);
    inter->data.function = function;
}

void functionPresetting(LinkValue *func, LinkValue **func_new, LinkValue **func_init, Inter *inter) {
    *func_new = makeCFunctionFromOf(function_new, func, function_new, function_init, inter->base_father, inter->var_list, inter);
    *func_init = makeCFunctionFromOf(function_init, func, function_new, function_init, inter->base_father, inter->var_list, inter);
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
