#include "__run.h"

Result setFunction(INTER_FUNCTIONSIG) {
    Result result, tmp;
    setResultOperation(&tmp, inter);

    tmp.value->value = makeFunctionValue(st->u.set_function.function, st->u.set_function.parameter, var_list, inter);
    result = assCore(st->u.set_function.name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return result;
}

Result callFunction(INTER_FUNCTIONSIG) {
    Result call_function, result;

    if (operationSafeInterStatement(&call_function, CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list)))
        return call_function;

    if (call_function.value->value->type != function){
        writeLog_(inter->debug, WARNING, "call not function[%d]\n", call_function.type);
        setResultError(&result, inter);
        result.type = error_return;
        goto return_;
    }
    VarList *function_var = call_function.value->value->data.function.var;
    Result set_tmp;
    set_tmp = setParameter(st->u.call_function.parameter, call_function.value->value->data.function.pt, function_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (set_tmp.type == error_return)
        return set_tmp;
    function_var = pushVarList(function_var, inter);
    functionSafeInterStatement(&result,
                               CALL_INTER_FUNCTIONSIG(call_function.value->value->data.function.function, function_var));
    popVarList(function_var);

    return_:
    return result;
}