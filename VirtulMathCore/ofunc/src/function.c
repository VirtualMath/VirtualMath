#include "__ofunc.h"

static void setFunctionData(Value *value, LinkValue *cls, Inter *inter) {
    value->data.function.function_data.pt_type = inter->data.default_pt_type;
    value->data.function.function_data.cls = cls;
    value->data.function.function_data.run = false;
}

ResultType function_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
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

    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType function_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *func;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if ((func = ap[0].value)->value->type != V_func) {
        setResultError(E_TypeException, INSTANCE_ERROR(func), LINEFILE, true,
                       CNEXT_NT);
        return R_error;
    }

    if (ap[1].value != NULL) {
        Statement *return_ = makeBaseLinkValueStatement(ap[1].value, LINEFILE);
        func->value->data.function.function = makeReturnStatement(return_, LINEFILE);
        func->value->data.function.function_data.pt_type = free_;
        func->value->data.function.type = vm_func;
    }

    setResult(result, inter);
    return result->type;
}

void registeredFunction(R_FUNC){
    LinkValue *object = inter->data.function;
    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"func", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
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

    addStrVar(inter->data.object_new, false, true, func_new, LINEFILE, false, CFUNC_NT(object_var, &result, func));
    freeResult(&result);
    addStrVar(inter->data.object_init, false, true, func_init, LINEFILE, false, CFUNC_NT(object_var, &result, func));
    freeResult(&result);
}
