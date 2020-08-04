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
            result = getVar(CALL_INTER_FUNCTIONSIG(st, var_list), getBaseVarInfo);
            break;
        case base_svar:
            result = getVar(CALL_INTER_FUNCTIONSIG(st, var_list), getBaseSVarInfo);
            break;
        case base_list:
            result = getList(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case base_dict:
            result = getDict(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case operation:
            result = operationStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
            printLinkValue(result.value, "operation result = ", "\n", inter->data.debug);
            break;
        case set_function:
            result = setFunction(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case call_function:
            result = callFunction(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case if_branch:
            result = ifBranch(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case while_branch:
            result = whileBranch(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case try_branch:
            result = tryBranch(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case break_cycle:
            result = breakCycle(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case continue_cycle:
            result = continueCycle(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case rego_if:
            result = regoIf(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case restart:
            result = restartCode(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case return_code:
            result = returnCode(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case raise_code:
            result = raiseCode(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case include_file:
            result = includeFile(CALL_INTER_FUNCTIONSIG(st, var_list));
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
    if (st == NULL){
        setResult(&result, true, inter);
        return result;
    }
    Statement *base_st = NULL;

    while (true) {
        base_st = st;
        while (base_st != NULL) {
            result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list));
            if (!run_continue(result))
                break;
            base_st = base_st->next;
        }

        if (result.type != restart_return || result.times > 0)
            break;
    }

    if (result.type == not_return)
        setResultOperation(&result, inter);
    return result;
}

/**
 * 全局程序运行statement
 * @param inter
 * @return
 */
Result globalIterStatement(Inter *inter) {
    Result result;
    Statement *base_st = NULL;
    VarList *var_list = NULL;

    while (true) {
        base_st = inter->statement;
        var_list = inter->var_list;
        while (base_st != NULL) {
            result = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list));
            if (!run_continue(result))
                break;
            base_st = base_st->next;
        }
        if (result.type != restart_return || result.times > 0)
            break;
    }

    if (result.type != error_return && result.type != function_return)
        setResultOperation(&result, inter);
    return result;
}

// 若需要中断执行, 则返回true
bool operationSafeInterStatement(Result *result, INTER_FUNCTIONSIG){
    *result = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
    if (run_continue_(result))
        return false;
    return true;
}

bool ifBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG){
    *result = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
    if (run_continue_(result)){
        return false;
    }
    if (result->type == rego_return){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (result->type == restart_return)
        result->times--;
    return true;
}

bool cycleBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG){
    *result = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
    if (run_continue_(result)){
        return false;
    }
    if (result->type == break_return || result->type == continue_return){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (result->type == restart_return)
        result->times--;
    return true;
}

bool tryBranchSafeInterStatement(Result *result, INTER_FUNCTIONSIG){
    *result = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
    if (run_continue_(result)){
        return false;
    }
    if (result->type == restart_return)
        result->times--;
    return true;
}

bool functionSafeInterStatement(Result *result, INTER_FUNCTIONSIG){
    *result = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list));
    if (result->type == error_return){
        return true;
    }
    else if (result->type == function_return){
        result->type = operation_return;
        return true;
    }
    result->type = not_return;
    return false;
}