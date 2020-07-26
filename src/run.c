#include "__run.h"

Result getBaseVar(INTER_FUNCTIONSIG) {
    Result times, result;
    int int_times;
    if (st->u.base_var.times == NULL){
        int_times = 0;
        goto not_times;
    }
    times = iterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list));
    int_times = (int)times.value->value->data.num.num;

    not_times:
    result.value = findFromVarList(st->u.base_var.name, var_list, int_times);
    if (result.value == NULL){
        writeLog_(inter->debug, WARNING, "not found[%s]\n", st->u.base_var.name);
    }
    return result;
}

/**
 * 运行单个statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
Result runStatement(INTER_FUNCTIONSIG) {
    Result result;
    setResult(&result, true, inter);
    switch (st->type) {
        case base_value:
            result.value->value = st->u.base_value.value;
            break;
        case base_var:
            result = getBaseVar(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case operation:
            result = operationStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
            printResult(result, "operation result = ", "", inter->debug);
            break;
        default:
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
    VarList *new_var_list = var_list;
    while(base_st != NULL){
        result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, new_var_list));
        base_st = base_st->next;
    }
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
    VarList *new_var_list = inter->var_list;
    while(base_st != NULL){
        result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, new_var_list));
        base_st = base_st->next;
    }
    return result;
}
