#include "__run.h"

Result setFunction(INTER_FUNCTIONSIG) {
    Result result;
    LinkValue *tmp = makeLinkValue(NULL, NULL, inter);
    setResultCore(&result);

    tmp->value = makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, var_list, inter);
    result = assCore(st->u.set_function.name, tmp, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (!run_continue(result))
        return result;
    setResult(&result, inter);
    return result;
}

Result callFunction(INTER_FUNCTIONSIG) {
    Result result;
    Result function_value;
    Result set_tmp;
    setResultCore(&result);
    setResultCore(&function_value);
    setResultCore(&set_tmp);

    if (operationSafeInterStatement(&function_value, CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list)))
        return function_value;
    if (function_value.value->value->type != function){
        freeResult(&function_value);
        setResultError(&result, inter, "TypeException", "Object is not callable", st, true);
        result.type = error_return;
        goto return_;
    }
    VarList *function_var = function_value.value->value->data.function.var;
    set_tmp = setParameter(st->u.call_function.parameter, function_value.value->value->data.function.pt, function_var,
                           st, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (set_tmp.type == error_return)
        return set_tmp;
    else
        freeResult(&set_tmp);
    function_var = pushVarList(function_var, inter);
    functionSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(function_value.value->value->data.function.function, function_var));
    popVarList(function_var);

    freeResult(&function_value);
    setResultError(&result, inter, NULL, NULL, st, false);  // 自带检查

    return_:
    return result;
}