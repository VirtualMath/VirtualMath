#include "__run.h"

enum ResultType  setFunction(INTER_FUNCTIONSIG) {
    LinkValue *tmp = makeLinkValue(NULL, NULL, inter);
    setResultCore(result);

    tmp->value = makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, var_list, inter);
    assCore(st->u.set_function.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result));
    if (run_continue(result))
        setResult(result, inter);

    return result->type;
}

enum ResultType callFunction(INTER_FUNCTIONSIG) {
    LinkValue *function_value = NULL;
    VarList *function_var = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list, result)))
        goto return_;
    if (result->value->value->type != function){
        setResultError(result, inter, "TypeException", "Object is not callable", st, true);
        goto return_;
    }

    function_value = result->value;
    function_var = pushVarList(function_value->value->data.function.var, inter);
    gcAddTmp(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, true);

    freeResult(result);
    setParameter(st->u.call_function.parameter, function_value->value->data.function.pt, function_var,
                 st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result));
    if (!run_continue(result)) {
        gcAddTmp(&function_var->hashtable->gc_status);
        runFREEZE(inter, var_list, function_var, false);
        popVarList(function_var);
        goto return_;
    }
    else
        freeResult(result);

    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(function_value->value->data.function.function, function_var, result));

    gcFreeTmpLink(&function_var->hashtable->gc_status);
    runFREEZE(inter, var_list, function_var, false);
    popVarList(function_var);

    setResultError(result, inter, NULL, NULL, st, false);  // 自带检查

    return_:
    return result->type;
}