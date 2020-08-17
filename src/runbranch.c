#include "__run.h"

bool checkNumber(INTER_FUNCTIONSIG){
    if (!isType(result->value->value, number)) {
        setResultErrorSt(result, inter, "TypeException", "Don't get a number value", st, father, true);
        return false;
    }
    return true;
}

bool checkString(INTER_FUNCTIONSIG){
    if (!isType(result->value->value, string)) {
        setResultErrorSt(result, inter, "TypeException", "Don't get a string value", st, father, true);
        return false;
    }
    return true;
}

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

ResultType ifBranch(INTER_FUNCTIONSIG) {
    StatementList *if_list = st->u.if_branch.if_list;
    Statement *else_st = st->u.if_branch.else_list;
    Statement *finally = st->u.if_branch.finally;
    bool set_result = true;
    bool is_rego = false;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);
    for (PASS; if_list != NULL; if_list = if_list->next){
        freeResult(result);
        if (if_list->type == if_b){
            LinkValue *condition_value = NULL;
            if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->condition, var_list, result, father))){
                set_result = false;
                goto not_else;
            }

            condition_value = result->value;
            freeResult(result);
            if (if_list->var != NULL) {
                assCore(if_list->var, condition_value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
                if (!run_continue(result)){
                    set_result = false;
                    goto not_else;
                }
                freeResult(result);
            }

            bool condition = is_rego ? true : checkBool(condition_value->value);  // 若是rego则不执行checkbool的判断了
            if (condition){
                is_rego = false;
                if (ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->code, var_list, result, father))){
                    set_result = false;
                    goto not_else;
                }
                if (result->type == rego_return)
                    is_rego = true;
                else {
                    freeResult(result);
                    goto not_else;
                }
                freeResult(result);
            }
        }
        else{
            if (ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->code, var_list, result, father))){
                set_result = false;
                goto not_else;
            }
            if (result->type == rego_return)
                is_rego = true;
            freeResult(result);
        }
    }
    if (else_st != NULL && ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, father)))
        set_result = false;
    else
        freeResult(result);

    not_else:
    if (finally != NULL && ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, father))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    var_list = popVarList(var_list);
    if (set_result)
        setResult(result, inter, father);
    return result->type;
}

ResultType whileBranch(INTER_FUNCTIONSIG) {
    StatementList *while_list = st->u.while_branch.while_list;
    Statement *first = st->u.while_branch.first;
    Statement *after = st->u.while_branch.after;
    Statement *else_st = st->u.while_branch.else_list;
    Statement *finally = st->u.while_branch.finally;
    bool set_result = true;
    bool is_break = false;
    bool do_while = st->u.while_branch.type == do_while_;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);

    if (first != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(first, var_list, result, father)))
        set_result = false;
    else
        freeResult(result);

    while (!is_break){
        LinkValue *condition_value = NULL;
        bool condition = false;

        freeResult(result);
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(while_list->condition, var_list, result, father))){
            set_result = false;
            goto not_else;
        }

        condition_value = result->value;
        freeResult(result);
        if (while_list->var != NULL){
            assCore(while_list->var, condition_value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
            if (!run_continue(result)){
                set_result = false;
                goto not_else;
            }
            freeResult(result);  // 赋值的返回值被丢弃
        }

        condition = do_while || checkBool(condition_value->value);
        do_while = false;
        if (condition){
            if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(while_list->code, var_list, result, father))){
                set_result = false;
                goto not_else;
            }
            else if (result->type == break_return)
                is_break = true;
            freeResult(result);
        }
        else
            break;

        if (after == NULL)
            continue;

        if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(after, var_list, result, father))){
            set_result = false;
            goto not_else;
        }
        else if (result->type == break_return) {
            freeResult(result);
            goto not_else;
        }

        freeResult(result);
    }
    if (!is_break && else_st != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, father)))
        set_result = false;
    else
        freeResult(result);

    not_else:
    if (finally != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, father))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    var_list = popVarList(var_list);
    if (set_result)
        setResult(result, inter, father);
    return result->type;
}

ResultType withBranch(INTER_FUNCTIONSIG) {
    StatementList *with_list = st->u.with_branch.with_list;
    Statement *else_st = st->u.with_branch.else_list;
    Statement *finally = st->u.with_branch.finally;
    VarList *new = NULL;
    LinkValue *_enter_ = NULL;
    LinkValue *_exit_ = NULL;
    LinkValue *value = NULL;
    bool set_result = true;

    Result finally_tmp;
    Result else_tmp;
    Result exit_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);
    setResultCore(&else_tmp);
    setResultCore(&exit_tmp);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(with_list->condition, var_list, result, father))){
        set_result = false;
        goto not_else;
    }

    value = result->value;
    if (with_list->var == NULL) {
        new = copyVarListCore(result->value->value->object.var, inter);
        new->next = var_list;
    }
    else {
        LinkValue *enter_value = NULL;
        char *enter_name = setStrVarName(inter->data.object_enter, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        char *exit_name = setStrVarName(inter->data.object_exit, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        _enter_ = findFromVarList(enter_name, 0, false, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
        _exit_ = findFromVarList(exit_name, 0, false, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
        memFree(enter_name);
        memFree(exit_name);

        freeResult(result);
        if (_enter_ == NULL || _exit_ == NULL){
            _enter_ = NULL;
            _exit_ = NULL;
            setResultErrorSt(result, inter, "EnterException", "Get Not Support Value to Enter with", st, father, true);
            set_result = false;
            goto not_else;
        }

        gc_addTmpLink(&_enter_->gc_status);
        gc_addTmpLink(&_exit_->gc_status);
        callBackCore(_enter_, NULL, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, value));
        if (!run_continue(result)){
            set_result = false;
            goto not_else;
        }

        new = pushVarList(var_list, inter);
        enter_value = result->value;
        freeResult(result);
        assCore(with_list->var, enter_value, CALL_INTER_FUNCTIONSIG_NOT_ST (new, result, father));
        if (!run_continue(result)){
            set_result = false;
            popVarList(new);
            goto not_else;
        }
        freeResult(result);
    }

    if (tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(with_list->code, new, result, father)))
        set_result = false;
    else
        freeResult(result);

    if (else_st != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, new, &else_tmp, father))) {
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = else_tmp;
    }
    else
        freeResult(&else_tmp);

    popVarList(new);
    if (_exit_ != NULL && _enter_ != NULL) {
        callBackCore(_exit_, NULL, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &exit_tmp, value));
        if (!run_continue_type(exit_tmp.type)) {
            if (!set_result)
                freeResult(result);
            set_result = false;
            *result = exit_tmp;
        } else
            freeResult(&exit_tmp);
        gc_freeTmpLink(&_enter_->gc_status);
        gc_freeTmpLink(&_exit_->gc_status);
    }

    not_else:
    if (finally != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, father))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    if (set_result)
        setResult(result, inter, father);
    return result->type;
}

ResultType tryBranch(INTER_FUNCTIONSIG) {
    StatementList *except_list = st->u.try_branch.except_list;
    Statement *try = st->u.try_branch.try;
    Statement *else_st = st->u.try_branch.else_list;
    Statement *finally = st->u.try_branch.finally;
    LinkValue *error_value = NULL;
    bool set_result = true;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    var_list = pushVarList(var_list, inter);
    if (!tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(try, var_list, result, father))){
        freeResult(result);
        goto not_except;
    }

    if (except_list == NULL) {
        set_result = false;
        goto not_else;
    }

    error_value = result->value;
    freeResult(result);
    if (except_list->var != NULL){
        assCore(except_list->var, error_value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
        if (!run_continue(result)){
            set_result = false;
            goto not_else;
        }
        freeResult(result);
    }
    if (tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(except_list->code, var_list, result, father)))
        set_result = false;
    else
        freeResult(result);
    goto not_else;

    not_except:
    if (else_st != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, father)))
        set_result = false;
    else
        freeResult(result);

    not_else:
    if (finally != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, father))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    var_list = popVarList(var_list);
    if (set_result)
        setResult(result, inter, father);
    return result->type;
}

ResultType breakCycle(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.break_cycle.times == NULL)
        goto not_times;

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.break_cycle.times, var_list, result, father)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter, father);
    if (times_int >= 0) {
        result->type = break_return;
        result->times = times_int;
    }
    return result->type;
}

ResultType continueCycle(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.continue_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.continue_cycle.times, var_list, result, father)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter, father);
    if (times_int >= 0) {
        result->type = continue_return;
        result->times = times_int;
    }
    return result->type;
}

ResultType regoIf(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.rego_if.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.rego_if.times, var_list, result, father)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter, father);
    if (times_int >= 0) {
        result->type = rego_return;
        result->times = times_int;
    }
    return result->type;
}

ResultType restartCode(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.restart.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.restart.times, var_list, result, father)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter, father);
    if (times_int >= 0) {
        result->type = restart_return;
        result->times = times_int;
    }
    return result->type;
}

ResultType returnCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (st->u.return_code.value == NULL) {
        setResult(result, inter, father);
        goto set_result;
    }

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.return_code.value, var_list, result, father)))
        return result->type;

    set_result:
    result->type = function_return;
    return result->type;
}

ResultType raiseCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (st->u.raise_code.value == NULL) {
        setResult(result, inter, father);
        goto set_result;
    }

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list, result, father)))
        return result->type;

    set_result:
    result->type = error_return;
    result->error = connectError(makeError("RaiseException", "Exception was raise by user", st->line, st->code_file), result->error);
    return result->type;
}

ResultType assertCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list, result, father)))
        return result->type;

    if (checkBool(result->value->value))
        setResult(result, inter, father);
    else
        setResultErrorSt(result, inter, "AssertException", "Raise by user", st, father, true);
    return result->type;
}

ResultType gotoLabel(INTER_FUNCTIONSIG){
    int times_int = 0;
    char *label = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.label, var_list, result, father)))
        return result->type;
    if (!checkString(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;
    label = memStrcpy(result->value->value->data.str.str);

    freeResult(result);
    if (st->u.goto_.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.times, var_list, result, father))) {
        memFree(label);
        return result->type;
    }
    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, father))) {
        memFree(label);
        return result->type;
    }
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);
    not_times:
    if (st->u.goto_.return_ == NULL)
        setResult(result, inter, father);
    else if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.return_, var_list, result, father))) {
        memFree(label);
        return result->type;
    }

    result->times = times_int;
    result->type = goto_return;
    result->label = label;
    return result->type;
}

ResultType runLabel(INTER_FUNCTIONSIG) {
    // goto的值通过result传入
    LinkValue *goto_value = result->value;
    result->value = NULL;
    freeResult(result);
    var_list = pushVarList(var_list, inter);
    if (st->u.label_.as != NULL)
        assCore(st->u.label_.as, goto_value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    gc_freeTmpLink(&goto_value->gc_status);
    if (st->u.label_.as != NULL && !run_continue(result))
        goto return_;

    freeResult(result);
    if (st->u.label_.command != NULL)
        operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.label_.command, var_list, result, father));
    else
        setResult(result, inter, father);

    return_:
    popVarList(var_list);
    return result->type;
}
