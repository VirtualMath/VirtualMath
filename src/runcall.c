#include "__run.h"

ResultType setClass(INTER_FUNCTIONSIG) {
    Argument *call = NULL;
    LinkValue *tmp = NULL;
    FatherValue *class_father = NULL;
    VarList *father_var = NULL;
    setResultCore(result);

    call = getArgument(st->u.set_class.father, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (!run_continue(result))
        goto error_;

    freeResult(result);
    class_father = setFather(call, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    freeArgument(call, true);
    if (!run_continue(result))
        goto error_;

    tmp = makeLinkValue(makeClassValue(copyVarList(var_list, false, inter), inter, class_father), father, inter);
    gcAddTmp(&tmp->gc_status);

    father_var = tmp->value->object.var->next;
    tmp->value->object.var->next = var_list;
    freeResult(result);
    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.set_class.st, tmp->value->object.var, result, tmp));
    tmp->value->object.var->next = father_var;
    if (!run_continue(result))
        goto error_;
    freeResult(result);

    assCore(st->u.set_class.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (run_continue(result))
        setResult(result, inter, father);

    gcFreeTmpLink(&tmp->gc_status);
    return result->type;

    error_:
    gcFreeTmpLink(&tmp->gc_status);
    setResultError(result, inter, NULL, NULL, st, father, false);
    return result->type;
}

ResultType setFunction(INTER_FUNCTIONSIG) {
    LinkValue *tmp = NULL;
    setResultCore(result);

    tmp = makeLinkValue(makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, copyVarList(var_list, false, inter), inter), father, inter);
    assCore(st->u.set_function.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (run_continue(result))
        setResult(result, inter, father);

    return result->type;
}

ResultType callBack(INTER_FUNCTIONSIG) {
    LinkValue *function_value = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list, result, father)))
        goto return_;

    function_value = result->value;
    freeResult(result);
    if (function_value->value->type == function)
        callFunction(function_value, st->u.call_function.parameter, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    else if (function_value->value->type == class)
        callClass(function_value, st->u.call_function.parameter, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    else{
        setResultError(result, inter, "TypeException", "Object is not callable", st, father, true);
        return result->type;
    }


    setResultError(result, inter, NULL, NULL, st, father, false);
    return_:
    return result->type;
}

ResultType callClass(LinkValue *class_value, Parameter *parameter, INTER_FUNCTIONSIG_NOT_ST) {
    VarList *function_var = NULL;
    LinkValue *value = NULL;
    LinkValue *__init__ = NULL;
    setResultCore(result);

    value = makeLinkValue(makeObject(inter, NULL,copyVarList(class_value->value->object.out_var, false, inter),
                          setFatherCore(makeFatherValue(class_value))), father, inter);
    setResultOperation(result, value, inter);

    char *init_name = setStrVarName(inter->data.object_init, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    __init__ = findFromVarList(init_name, value->value->object.var, 0, false);
    memFree(init_name);

    if (__init__ != NULL && __init__->value->type == function){
        Result __init__result;
        setResultCore(&__init__result);
        __init__ = makeLinkValue(__init__->value, value, inter);

        gcAddTmp(&__init__->gc_status);
        callFunction(__init__, parameter, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &__init__result, value));
        gcFreeTmpLink(&__init__->gc_status);

        if (!run_continue_type(__init__result.type)){
            freeResult(result);
            *result = __init__result;
            goto return_;
        }
        freeResult(&__init__result);
    }

    return_:
    return result->type;
}

ResultType callFunction(LinkValue *function_value, Parameter *parameter, INTER_FUNCTIONSIG_NOT_ST) {
    VarList *function_var = NULL;
    setResultCore(result);
    gcAddTmp(&function_value->gc_status);

    function_var = pushVarList(function_value->value->object.out_var, inter);
    gcAddTmp(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, true);

    setParameter(parameter, function_value->value->data.function.pt, function_var, function_value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (!run_continue(result)) {
        gcAddTmp(&function_var->hashtable->gc_status);
        runFREEZE(inter, var_list, function_var, false);
        popVarList(function_var);
        goto return_;
    }

    freeResult(result);
    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(function_value->value->data.function.function, function_var, result, function_value));

    gcFreeTmpLink(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, false);
    popVarList(function_var);

    return_:
    gcFreeTmpLink(&function_value->gc_status);
    return result->type;
}