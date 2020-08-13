#include "__run.h"

ResultType setClass(INTER_FUNCTIONSIG) {
    LinkValue *tmp = NULL;
    setResultCore(result);
    tmp = makeLinkValue(makeClassValue(var_list, inter), father, inter);
    gcAddTmp(&tmp->gc_status);

    tmp->value->object.var->next = var_list;
    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.set_class.st, tmp->value->object.var, result, tmp));
    tmp->value->object.var->next = NULL;

    if (!run_continue(result)) {
        setResultError(result, inter, NULL, NULL, st, father, false);
        goto return_;
    }
    else
        freeResult(result);

    assCore(st->u.set_class.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (run_continue(result))
        setResult(result, inter, father);
    return_:
    gcFreeTmpLink(&tmp->gc_status);
    return result->type;
}

ResultType setFunction(INTER_FUNCTIONSIG) {
    LinkValue *tmp = NULL;
    setResultCore(result);

    tmp = makeLinkValue(makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, var_list, inter), father, inter);
    assCore(st->u.set_function.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (run_continue(result))
        setResult(result, inter, father);

    return result->type;
}

ResultType callFunction(INTER_FUNCTIONSIG) {
    LinkValue *function_value = NULL;
    VarList *function_var = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list, result, father)))
        goto return_;
    if (result->value->value->type != function){
        setResultError(result, inter, "TypeException", "Object is not callable", st, father, true);
        goto return_;
    }

    function_value = result->value;
    function_var = pushVarList(function_value->value->data.function.out_var, inter);
    gcAddTmp(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, true);

    freeResult(result);
    setParameter(st->u.call_function.parameter, function_value->value->data.function.pt, function_var,
                 st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (!run_continue(result)) {
        gcAddTmp(&function_var->hashtable->gc_status);
        runFREEZE(inter, var_list, function_var, false);
        popVarList(function_var);
        goto return_;
    }
    else
        freeResult(result);

    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(function_value->value->data.function.function, function_var, result, father));

    gcFreeTmpLink(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, false);
    popVarList(function_var);

    setResultError(result, inter, NULL, NULL, st, father, false);

    return_:
    return result->type;
}