#include "__run.h"

Result setFunction(INTER_FUNCTIONSIG) {
    Result result, tmp;
    setResultOperation(&tmp, inter);

    tmp.value->value = makeFunctionValue(st->u.set_function.function, var_list, inter);
    result = assCore(st->u.set_function.name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return result;
}

Result callFunction(INTER_FUNCTIONSIG) {
    Result call_function, result;

    safeIterStatement(call_function, CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list));
    if (call_function.value->value->type != function){
        writeLog_(inter->debug, WARNING, "call not function[%d]\n", call_function.type);
        setResultError(&result, inter);
        result.type = error_return;
        goto return_;
    }

    var_list = pushVarList(var_list, inter);
    result = iterStatement(CALL_INTER_FUNCTIONSIG(call_function.value->value->data.function.function, var_list));
    var_list = popVarList(var_list, inter);

    // 降级操作
    if (result.type == function_return)
        result.type = operation_return;
    else if(result.type == error_return)
        PASS;
    else
        result.type = not_return;

    return_:
    return result;
}