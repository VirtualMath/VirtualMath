#include "__run.h"

Result setFunction(INTER_FUNCTIONSIG) {
    Result result;
    Result tmp;
    setResultOperation(&tmp, inter);

    tmp.value->value = makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, var_list, inter);
    result = assCore(st->u.set_function.name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return result;
}

Result callFunction(INTER_FUNCTIONSIG) {
    Result result;
    Result call_function;

    if (operationSafeInterStatement(&call_function, CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list)))
        return call_function;

    if (call_function.value->value->type != function){
        setResultError(&result, inter, "TypeException", "Object is not callable", st, true);
        result.type = error_return;
        goto return_;
    }
    VarList *function_var = call_function.value->value->data.function.var;
    Result set_tmp;
    set_tmp = setParameter(st->u.call_function.parameter, call_function.value->value->data.function.pt, function_var,
                           st, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (set_tmp.type == error_return)
        return set_tmp;
    function_var = pushVarList(function_var, inter);
    functionSafeInterStatement(&result,
                               CALL_INTER_FUNCTIONSIG(call_function.value->value->data.function.function, function_var));
    setResultError(&result, inter, NULL, NULL, st, false);
    popVarList(function_var);

    return_:
    return result;
}