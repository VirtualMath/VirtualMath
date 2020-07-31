#include "__run.h"

bool checkBool(Value *value){
    switch (value->type) {
        case number:
            return value->data.num.num != 0;
        case string:
            return memStrlen(value->data.str.str) > 0;
        case none:
            return false;
        default:
            return true;
    }
}

// TODO-szh 检查rego对else的支持
Result ifBranch(INTER_FUNCTIONSIG) {
    Result result, else_tmp, finally_tmp;
    StatementList *if_list = st->u.if_branch.if_list;
    bool set_result = true, is_rego = false;

    var_list = pushVarList(var_list, inter);
    while (if_list != NULL){
        if (if_list->type == if_b){
            Result tmp;
            if (operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(if_list->condition, var_list))){
                result = tmp;
                set_result = false;
                goto not_else;
            }
            if (if_list->var != NULL)
                assCore(if_list->var, tmp.value, inter, var_list);
            if (is_rego || checkBool(tmp.value->value)){
                Result code_tmp;
                is_rego = false;
                if (ifBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(if_list->code, var_list))){
                    result = code_tmp;
                    set_result = false;
                    goto not_else;
                }
                if (code_tmp.type == rego_return)
                    is_rego = true;
                else
                    goto not_else;
            }
        }
        else{
            Result code_tmp;
            if (ifBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(if_list->code, var_list))){
                result = code_tmp;
                set_result = false;
                goto not_else;
            }
            if (code_tmp.type == rego_return)
                is_rego = true;
        }
        if_list = if_list->next;
    }
    if (st->u.if_branch.else_list != NULL && ifBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.if_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }
    not_else:

    if (st->u.if_branch.finally != NULL && ifBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.if_branch.finally, var_list))){
        set_result = false;
        result = finally_tmp;
    }

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, true, inter);
    return result;
}

Result whileBranch(INTER_FUNCTIONSIG) {
    Result result, else_tmp, finally_tmp;
    StatementList *while_list = st->u.while_branch.while_list;
    bool set_result = true, is_break = false;

    var_list = pushVarList(var_list, inter);
    while (!is_break){
        Result tmp, do_tmp;
        if (operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(while_list->condition, var_list))){
            result = tmp;
            set_result = false;
            goto not_else;
        }
        if (while_list->var != NULL)
            assCore(while_list->var, tmp.value, inter, var_list);
        if (checkBool(tmp.value->value)){
            Result code_tmp;
            if (cycleBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(while_list->code, var_list))){
                result = code_tmp;
                set_result = false;
                goto not_else;
            }
            if (code_tmp.type == break_return)
                is_break = true;
            if (code_tmp.type == continue_return)
                PASS;
        }
        else{
            break;
        }
        if (st->u.while_branch.after == NULL)
            goto not_after_do;
        if (cycleBranchSafeInterStatement(&do_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.after, var_list))){
            result = do_tmp;
            set_result = false;
            goto not_else;
        }
        if (do_tmp.type == break_return)
            goto not_else;  // 直接跳转到not_else
        if (do_tmp.type == continue_return)
            PASS;
        not_after_do: PASS;
    }
    if (!is_break && st->u.while_branch.else_list != NULL && cycleBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }
    not_else:

    if (st->u.while_branch.finally != NULL && cycleBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.finally, var_list))){
        set_result = false;
        result = finally_tmp;
    }

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, true, inter);
    return result;
}

Result tryBranch(INTER_FUNCTIONSIG) {
    Result result, try_result, except_result, else_tmp, finally_tmp;
    StatementList *except_list = st->u.try_branch.except_list;
    bool set_result = true;

    var_list = pushVarList(var_list, inter);
    if (!tryBranchSafeInterStatement(&try_result, CALL_INTER_FUNCTIONSIG(st->u.try_branch.try, var_list))){
        goto not_except;
    }
    if (except_list == NULL) {
        result = try_result;
        set_result = false;
        goto not_else;
    }
    if (except_list->var != NULL)
        assCore(except_list->var, try_result.value, inter, var_list);
    if (tryBranchSafeInterStatement(&except_result, CALL_INTER_FUNCTIONSIG(except_list->code, var_list))){
        result = except_result;
        set_result = false;
    }
    goto not_else;

    not_except:
    if (st->u.try_branch.else_list != NULL && tryBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.try_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }

    not_else:
    if (st->u.try_branch.finally != NULL && tryBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.try_branch.finally, var_list))){
        set_result = false;
        result = finally_tmp;
    }

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, true, inter);
    return result;
}

Result breakCycle(INTER_FUNCTIONSIG){
    Result result, times;
    int times_int = 0;
    if (st->u.break_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.break_cycle.times, var_list)))
        return times;
    // TODO-szh 类型检查处理
    times_int = (int)times.value->value->data.num.num;
    not_times:
    setResult(&result, true, inter);
    if (times_int >= 0) {
        result.type = break_return;
        result.times = times_int;
    }
    return result;
}

Result continueCycle(INTER_FUNCTIONSIG){
    Result result, times;
    int times_int = 0;
    if (st->u.continue_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.continue_cycle.times, var_list)))
        return times;
    times_int = (int)times.value->value->data.num.num;
    not_times:
    setResult(&result, true, inter);
    if (times_int >= 0) {
        result.type = continue_return;
        result.times = times_int;
    }
    return result;
}

Result regoIf(INTER_FUNCTIONSIG){
    Result result, times;
    int times_int = 0;
    if (st->u.rego_if.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.rego_if.times, var_list)))
        return times;
    times_int = (int)times.value->value->data.num.num;
    not_times:
    setResult(&result, true, inter);
    if (times_int >= 0) {
        result.type = rego_return;
        result.times = times_int;
    }
    return result;
}

Result restartCode(INTER_FUNCTIONSIG){
    Result result, times;
    int times_int = 0;
    if (st->u.restart.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.restart.times, var_list)))
        return times;
    times_int = (int)times.value->value->data.num.num;
    not_times:
    setResult(&result, true, inter);
    if (times_int >= 0) {
        result.type = restart_return;
        result.times = times_int;
    }
    return result;
}

Result returnCode(INTER_FUNCTIONSIG){
    Result result;
    if (st->u.return_code.value == NULL) {
        setResult(&result, true, inter);
        goto set_result;
    }
    if (operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(st->u.return_code.value, var_list)))
        return result;

    set_result:
    result.type = function_return;
    return result;
}

Result raiseCode(INTER_FUNCTIONSIG){
    Result result;
    if (st->u.raise_code.value == NULL) {
        setResult(&result, true, inter);
        goto set_result;
    }
    if (operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list)))
        return result;

    set_result:
    result.type = error_return;
    return result;
}
