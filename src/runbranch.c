#include "__run.h"

#define checkNumber(new_result) do{ \
if (!isType(new_result.value->value, number)){ \
freeResult(&new_result); \
setResultError(&result, inter, "TypeException", "Don't get a number value", st, true); \
return result; \
}}while(0) /*该Macro只适用于控制分支*/

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
    StatementList *if_list = st->u.if_branch.if_list;
    bool set_result = true;
    bool is_rego = false;

    Result result;
    Result else_tmp;
    Result finally_tmp;
    setResultCore(&result);
    setResultCore(&else_tmp);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);
    while (if_list != NULL){
        if (if_list->type == if_b){
            Result condition_tmp;
            Result var_tmp;
            setResultCore(&condition_tmp);
            setResultCore(&var_tmp);

            if (operationSafeInterStatement(&condition_tmp, CALL_INTER_FUNCTIONSIG(if_list->condition, var_list))){
                result = condition_tmp;
                set_result = false;
                goto not_else;
            }

            if (if_list->var != NULL) {
                freeResult(&var_tmp);
                var_tmp = assCore(if_list->var, condition_tmp.value, inter, var_list);
                if (!run_continue(var_tmp)){
                    freeResult(&condition_tmp);
                    result = var_tmp;
                    set_result = false;
                    goto not_else;
                }
                freeResult(&var_tmp);  // 赋值的返回值被丢弃
            }

            bool condition = is_rego ? true : checkBool(condition_tmp.value->value);  // 若是rego则不执行checkbool的判断了
            freeResult(&condition_tmp);
            if (condition){
                Result code_tmp;
                setResultCore(&code_tmp);

                is_rego = false;
                if (ifBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(if_list->code, var_list))){
                    result = code_tmp;
                    set_result = false;
                    goto not_else;
                }
                if (code_tmp.type == rego_return)
                    is_rego = true;
                else {
                    freeResult(&code_tmp);
                    goto not_else;
                }

                freeResult(&code_tmp);
            }
        }
        else{
            Result code_tmp;
            setResultCore(&code_tmp);
            if (ifBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(if_list->code, var_list))){
                result = code_tmp;
                set_result = false;
                goto not_else;
            }
            if (code_tmp.type == rego_return)
                is_rego = true;
            freeResult(&code_tmp);
        }
        if_list = if_list->next;
    }
    if (st->u.if_branch.else_list != NULL && ifBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.if_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }
    else
        freeResult(&else_tmp);

    not_else:
    if (st->u.if_branch.finally != NULL && ifBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.if_branch.finally, var_list))){
        if (!set_result)
            freeResult(&result);
        set_result = false;
        result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, inter);
    return result;
}

Result whileBranch(INTER_FUNCTIONSIG) {
    StatementList *while_list = st->u.while_branch.while_list;
    bool set_result = true;
    bool is_break = false;
    Result result;
    Result else_tmp;
    Result finally_tmp;
    setResultCore(&result);
    setResultCore(&else_tmp);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);
    while (!is_break){
        Result tmp;
        Result var_tmp;
        Result do_tmp;
        setResultCore(&tmp);
        setResultCore(&var_tmp);
        setResultCore(&do_tmp);

        if (operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(while_list->condition, var_list))){
            result = tmp;
            set_result = false;
            goto not_else;
        }
        if (while_list->var != NULL){
            freeResult(&var_tmp);
            var_tmp = assCore(while_list->var, tmp.value, inter, var_list);
            if (!run_continue(var_tmp)){
                freeResult(&tmp);
                result = var_tmp;
                set_result = false;
                goto not_else;
            }
            freeResult(&var_tmp);  // 赋值的返回值被丢弃
        }

        bool condition = checkBool(tmp.value->value);
        freeResult(&tmp);
        if (condition){
            Result code_tmp;
            setResultCore(&code_tmp);
            if (cycleBranchSafeInterStatement(&code_tmp, CALL_INTER_FUNCTIONSIG(while_list->code, var_list))){
                result = code_tmp;
                set_result = false;
                goto not_else;
            }
            if (code_tmp.type == break_return)
                is_break = true;
            if (code_tmp.type == continue_return)
                PASS;
            freeResult(&code_tmp);
        }
        else
            break;

        if (st->u.while_branch.after == NULL)
            goto not_after_do;

        if (cycleBranchSafeInterStatement(&do_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.after, var_list))){
            result = do_tmp;
            set_result = false;
            goto not_else;
        }
        if (do_tmp.type == break_return) {
            freeResult(&do_tmp);
            goto not_else;
        }
        if (do_tmp.type == continue_return)
            PASS;

        freeResult(&do_tmp);
        not_after_do: PASS;
    }
    if (!is_break && st->u.while_branch.else_list != NULL && cycleBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }
    else
        freeResult(&else_tmp);

    not_else:
    if (st->u.while_branch.finally != NULL && cycleBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.while_branch.finally, var_list))){
        if (!set_result)
            freeResult(&result);
        set_result = false;
        result = finally_tmp;
    }

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, inter);
    return result;
}

Result tryBranch(INTER_FUNCTIONSIG) {
    bool set_result = true;
    StatementList *except_list = st->u.try_branch.except_list;
    Result result;
    Result try_result;
    Result var_tmp;
    Result except_result;
    Result else_tmp;
    Result finally_tmp;
    setResultCore(&result);
    setResultCore(&except_result);
    setResultCore(&try_result);
    setResultCore(&var_tmp);
    setResultCore(&else_tmp);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);
    if (!tryBranchSafeInterStatement(&try_result, CALL_INTER_FUNCTIONSIG(st->u.try_branch.try, var_list))){
        freeResult(&try_result);
        goto not_except;
    }

    if (except_list == NULL) {
        result = try_result;
        set_result = false;
        goto not_else;
    }
    if (except_list->var != NULL){
        freeResult(&var_tmp);
        var_tmp = assCore(except_list->var, try_result.value, inter, var_list);
        if (!run_continue(var_tmp)){
            freeResult(&try_result);
            result = var_tmp;
            set_result = false;
            goto not_else;
        }
        freeResult(&var_tmp);  // 赋值的返回值被丢弃
    }
    if (tryBranchSafeInterStatement(&except_result, CALL_INTER_FUNCTIONSIG(except_list->code, var_list))){
        result = except_result;
        set_result = false;
    }
    else
        freeResult(&except_result);

    freeResult(&try_result);
    goto not_else;

    not_except:
    if (st->u.try_branch.else_list != NULL && tryBranchSafeInterStatement(&else_tmp, CALL_INTER_FUNCTIONSIG(st->u.try_branch.else_list, var_list))){
        set_result = false;
        result = else_tmp;
    }
    else
        freeResult(&else_tmp);

    not_else:
    if (st->u.try_branch.finally != NULL && tryBranchSafeInterStatement(&finally_tmp, CALL_INTER_FUNCTIONSIG(st->u.try_branch.finally, var_list))){
        if (!set_result)
            freeResult(&result);
        set_result = false;
        result = finally_tmp;
    }

    var_list = popVarList(var_list);
    if (set_result)
        setResult(&result, inter);
    return result;
}

Result breakCycle(INTER_FUNCTIONSIG){
    Result result;
    Result times;
    setResultCore(&result);
    setResultCore(&times);

    int times_int = 0;
    if (st->u.break_cycle.times == NULL)
        goto not_times;

    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.break_cycle.times, var_list)))
        return times;

    checkNumber(times);
    times_int = (int)times.value->value->data.num.num;
    freeResult(&times);

    not_times:
    setResult(&result, inter);
    if (times_int >= 0) {
        result.type = break_return;
        result.times = times_int;
    }
    return result;
}

Result continueCycle(INTER_FUNCTIONSIG){
    Result result;
    Result times;
    setResultCore(&result);
    setResultCore(&times);

    int times_int = 0;
    if (st->u.continue_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.continue_cycle.times, var_list)))
        return times;

    checkNumber(times);
    times_int = (int)times.value->value->data.num.num;
    freeResult(&times);

    not_times:
    setResult(&result, inter);
    if (times_int >= 0) {
        result.type = continue_return;
        result.times = times_int;
    }
    return result;
}

Result regoIf(INTER_FUNCTIONSIG){
    Result result;
    Result times;
    setResultCore(&result);
    setResultCore(&times);

    int times_int = 0;
    if (st->u.rego_if.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.rego_if.times, var_list)))
        return times;

    checkNumber(times);
    times_int = (int)times.value->value->data.num.num;
    freeResult(&times);

    not_times:
    setResult(&result, inter);
    if (times_int >= 0) {
        result.type = rego_return;
        result.times = times_int;
    }
    return result;
}

Result restartCode(INTER_FUNCTIONSIG){
    Result result;
    Result times;
    setResultCore(&result);
    setResultCore(&times);

    int times_int = 0;
    if (st->u.restart.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.restart.times, var_list)))
        return times;

    checkNumber(times);
    times_int = (int)times.value->value->data.num.num;
    freeResult(&times);

    not_times:
    setResult(&result, inter);
    if (times_int >= 0) {
        result.type = restart_return;
        result.times = times_int;
    }
    return result;
}

Result returnCode(INTER_FUNCTIONSIG){
    Result result;
    setResultCore(&result);

    if (st->u.return_code.value == NULL) {
        setResult(&result, inter);
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
    setResultCore(&result);

    if (st->u.raise_code.value == NULL) {
        setResult(&result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list)))
        return result;

    set_result:
    result.type = error_return;
    setResultError(&result, inter, "RaiseException", "Exception was raise by user", st, false);
    return result;
}
