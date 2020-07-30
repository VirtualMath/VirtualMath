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

Result ifBranch(INTER_FUNCTIONSIG) {
    Result result, else_tmp, finally_tmp;
    StatementList *if_list = st->u.if_branch.if_list;
    bool set_result = true, is_rego = false;

    var_list = pushVarList(var_list, inter);
    while (if_list != NULL){
        if (if_list->type == if_b){
            Result tmp;
            if (!is_rego && operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(if_list->condition, var_list))){
                result = tmp;
                set_result = false;
                goto not_else;
            }
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

    var_list = popVarList(var_list, inter);
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
        if (st->u.while_branch.after != NULL && cycleBranchSafeInterStatement(&do_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.after, var_list))){
            result = do_tmp;
            set_result = false;
            goto not_else;
        }
        if (do_tmp.type == break_return)
            goto not_else;  // 直接跳转到not_else
        if (do_tmp.type == continue_return)
            PASS;
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

    var_list = popVarList(var_list, inter);
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
