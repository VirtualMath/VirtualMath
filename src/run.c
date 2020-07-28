#include "__run.h"

/**
 * 运行单个statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
Result runStatement(INTER_FUNCTIONSIG) {
    Result result;
    switch (st->type) {
        case base_value:
            result = getBaseValue(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case base_var:
            result = getBaseVar(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case operation:
            result = operationStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
            printResult(result, "operation result = ", "", inter->debug);
            break;
        case set_function:
            result = setFunction(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case call_function:
            result = callFunction(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        default:
            setResult(&result, true, inter);
            break;
    }
    return result;
}

/**
 * 局部程序运行statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
Result iterStatement(INTER_FUNCTIONSIG) {
    Result result;
    Statement *base_st = st;
    while(base_st != NULL){
        result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list));
        if (!runContinue(result))
            break;
        base_st = base_st->next;
    }
    if (result.type == not_return)
        setResult(&result, true, inter);
    return result;
}

/**
 * 全局程序运行statement
 * @param inter
 * @return
 */
Result globalIterStatement(Inter *inter) {
    Result result;
    Statement *base_st = inter->statement;
    VarList *var_list = inter->var_list;
    while(base_st != NULL){
        result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list));
        if (!runContinue(result))
            break;
        base_st = base_st->next;
    }
    if (result.type != error_return || result.type != function_return)
        setResult(&result, true, inter);
    return result;
}
