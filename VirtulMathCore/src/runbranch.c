#include "__run.h"

static bool checkNumber(INTER_FUNCTIONSIG){
    if (!isType(result->value->value, V_num)) {
        setResultErrorSt(E_TypeException, L"Don't get a V_num of layers", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    }
    return true;
}

void newBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status, Inter *inter){
    if (new_var != NULL)
        new_var->next = NULL;
    gc_freeze(inter, new_var, NULL, true);
    branch_st->info.var_list = new_var;
    branch_st->info.node = node->type == yield_code ? node->next : node;
    branch_st->info.branch.sl_node = sl_node;
    branch_st->info.branch.status = status;
    branch_st->info.have_info = true;
}

void newWithBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *value, LinkValue *_exit_, LinkValue *_enter_){
    newBranchYield(branch_st, node, sl_node, new_var, status, inter);
    branch_st->info.branch.with_.value = value;
    branch_st->info.branch.with_._exit_ = _exit_;
    branch_st->info.branch.with_._enter_ = _enter_;

}

void newForBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *iter){
    newBranchYield(branch_st, node, sl_node, new_var, status, inter);
    branch_st->info.branch.for_.iter = iter;

}

void updateBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, enum StatementInfoStatus status){
    branch_st->info.node = node->type == yield_code ? node->next : node;
    branch_st->info.branch.sl_node = sl_node;
    branch_st->info.branch.status = status;
    branch_st->info.have_info = true;
}

ResultType ifBranch(INTER_FUNCTIONSIG) {
    StatementList *if_list = st->u.if_branch.if_list;
    Statement *else_st = st->u.if_branch.else_list;
    Statement *finally = st->u.if_branch.finally;
    Statement *info_vl = NULL;
    bool set_result = true;
    bool is_rego = false;
    bool yield_run = false;
    enum StatementInfoStatus result_from = info_vl_branch;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    yield_run = popStatementVarList(st, &var_list, var_list, inter);
    if (yield_run && st->info.branch.status == info_vl_branch){
        if_list = st->info.branch.sl_node;
        info_vl = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_else_branch){
        if_list = NULL;
        else_st = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_finally_branch){
        finally = st->info.node;
        goto not_else;
    }

    for (PASS; if_list != NULL; if_list = if_list->next){
        bool condition;
        freeResult(result);
        if (info_vl != NULL){
            if (ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(info_vl, var_list, result, belong))){
                set_result = false;
                goto not_else;
            }
            if (result->type == R_rego)
                is_rego = true;
            freeResult(result);
            info_vl = NULL;
        }
        else if (if_list->type == if_b){
            LinkValue *condition_value = NULL;
            if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->condition, var_list, result, belong))){
                set_result = false;
                goto not_else;
            }

            condition_value = result->value;
            freeResult(result);
            if (if_list->var != NULL) {
                assCore(if_list->var, condition_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                if (!CHECK_RESULT(result)){
                    set_result = false;
                    goto not_else;
                }
                freeResult(result);
            }

            if (!(condition = is_rego)){
                condition = checkBool(condition_value, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                if (!CHECK_RESULT(result)) {
                    set_result = false;
                    goto not_else;
                }
                freeResult(result);
            }

            if (condition){
                is_rego = false;
                if (ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->code, var_list, result, belong))){
                    set_result = false;
                    goto not_else;
                }
                if (result->type == R_rego)
                    is_rego = true;
                else {
                    freeResult(result);
                    goto not_else;
                }
                freeResult(result);
            }
        }
        else{
            if (ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(if_list->code, var_list, result, belong))){
                set_result = false;
                goto not_else;
            }
            if (result->type == R_rego)
                is_rego = true;
            freeResult(result);
        }
    }
    if (else_st != NULL && ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, belong))) {
        set_result = false;
        result_from = info_else_branch;
    }
    else
        freeResult(result);

    not_else:
    if (finally != NULL && ifBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, belong))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        result_from = info_finally_branch;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    if (yield_run)
        if (result->type == R_yield)
            updateBranchYield(st, result->node, if_list, result_from);
        else
            freeRunInfo(st);
    else
        if (result->type == R_yield)
            newBranchYield(st, result->node, if_list, var_list, result_from, inter);
        else
            var_list = popVarList(var_list);
    if (set_result)
        setResult(result, inter);
    return result->type;
}

ResultType whileBranch(INTER_FUNCTIONSIG) {
    StatementList *while_list = st->u.while_branch.while_list;
    Statement *first = st->u.while_branch.first;
    Statement *after = st->u.while_branch.after;
    Statement *else_st = st->u.while_branch.else_list;
    Statement *finally = st->u.while_branch.finally;
    Statement *info_vl = NULL;
    Statement *after_vl = NULL;
    bool set_result = true;
    bool is_break = false;
    bool do_while = st->u.while_branch.type == do_while_;
    int yield_run = false;
    enum StatementInfoStatus result_from = info_vl_branch;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    yield_run = popStatementVarList(st, &var_list, var_list, inter);
    if (yield_run && st->info.branch.status == info_first_do)
        first = st->info.node;
    else if (yield_run && st->info.branch.status == info_vl_branch){
        first = NULL;
        info_vl = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_after_do){
        first = NULL;
        after_vl = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_else_branch){
        else_st = st->info.node;
        goto run_else;
    }
    else if (yield_run && st->info.branch.status == info_finally_branch){
        finally = st->info.node;
        goto not_else;
    }


    if (first != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(first, var_list, result, belong))) {
        result_from = info_first_do;
        set_result = false;
    }
    else
        freeResult(result);

    while (!is_break){
        LinkValue *condition_value = NULL;
        Statement *after_st = after;
        Statement *while_st = while_list->code;
        bool condition;
        freeResult(result);

        if (info_vl != NULL){
            while_st = info_vl;
            info_vl = NULL;
            goto do_while_st;
        }
        else if (after_vl != NULL){
            after_st = after_vl;
            after_vl = NULL;
            goto do_after;
        }

        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(while_list->condition, var_list, result, belong))){
            set_result = false;
            goto not_else;
        }

        condition_value = result->value;
        freeResult(result);
        if (while_list->var != NULL){
            assCore(while_list->var, condition_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result)){
                set_result = false;
                goto not_else;
            }
            freeResult(result);  // 赋值的返回值被丢弃
        }

        if (!(condition = do_while)){
            condition = checkBool(condition_value, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result)){
                set_result = false;
                goto not_else;
            }
            freeResult(result);
        }
        do_while = false;

        if (condition){
            do_while_st:
            if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(while_st, var_list, result, belong))){
                set_result = false;
                goto not_else;
            }
            else if (result->type == R_break)
                is_break = true;
            freeResult(result);
        }
        else
            break;

        do_after:
        if (after_st == NULL)
            continue;
        if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(after_st, var_list, result, belong))){
            result_from = info_after_do;
            set_result = false;
            goto not_else;
        }
        else if (result->type == R_break) {
            freeResult(result);
            goto not_else;
        }

        freeResult(result);
    }

    run_else:
    if (!is_break && else_st != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, belong))) {
        result_from = info_else_branch;
        set_result = false;
    }
    else
        freeResult(result);

    not_else:
    if (finally != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, belong))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        result_from = info_finally_branch;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    if (yield_run)
        if (result->type == R_yield)
            updateBranchYield(st, result->node, while_list, result_from);
        else
            freeRunInfo(st);
    else
        if (result->type == R_yield)
            newBranchYield(st, result->node, while_list, var_list, result_from, inter);
        else
            var_list = popVarList(var_list);
    if (set_result)
        setResult(result, inter);
    return result->type;
}

// TODO-szh 简化函数
ResultType forBranch(INTER_FUNCTIONSIG) {
    StatementList *for_list = st->u.for_branch.for_list;
    Statement *first = st->u.for_branch.first_do;
    Statement *after = st->u.for_branch.after_do;
    Statement *else_st = st->u.for_branch.else_list;
    Statement *finally = st->u.for_branch.finally;
    LinkValue *iter = NULL;
    Statement *info_vl = NULL;
    Statement *after_vl = NULL;
    bool set_result = true;
    bool is_break = false;
    bool yield_run = false;
    LinkValue *first_yield = NULL;
    enum StatementInfoStatus result_from = info_vl_branch;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    yield_run = popStatementVarList(st, &var_list, var_list, inter);
    if (yield_run && st->info.branch.status == info_first_do)
        first = st->info.node;
    else if (yield_run && st->info.branch.status == info_vl_branch){
        first = NULL;
        info_vl = st->info.node;
        iter = st->info.branch.for_.iter;
        goto do_for;
    }
    else if (yield_run && st->info.branch.status == info_after_do){
        first = NULL;
        after_vl = st->info.node;
        iter = st->info.branch.for_.iter;
        goto do_for;
    }
    else if (yield_run && st->info.branch.status == info_else_branch){
        else_st = st->info.node;
        goto run_else;
    }
    else if (yield_run && st->info.branch.status == info_finally_branch){
        finally = st->info.node;
        goto not_else;
    }

    if (first != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(first, var_list, result, belong))) {
        result_from = info_first_do;
        set_result = false;
    }
    else if (first != NULL)
        freeResult(result);

    {
        LinkValue *tmp = NULL;
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(for_list->condition, var_list, result, belong))){
            set_result = false;
            goto not_else;
        }
        if (result->is_yield){
            iter = NULL;
            first_yield = result->value;
            result->value = NULL;
            goto do_for;
        }

        tmp = result->value;
        result->value = NULL;
        freeResult(result);

        getIter(tmp, 1, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&tmp->gc_status);
        if (!CHECK_RESULT(result)) {
            set_result = false;
            goto not_else;
        }
        iter = result->value;
        result->value = NULL;
    }

    do_for:
    while (!is_break){  // 循环执行的本体
        Statement *for_st = for_list->code;
        Statement *after_st = after;
        freeResult(result);
        if (info_vl != NULL){
            for_st = info_vl;
            info_vl = NULL;
            goto do_for_st;
        }
        else if (after_vl != NULL){
            after_st = after_vl;
            after_vl = NULL;
            goto do_after;
        }

        {
            LinkValue *element = NULL;
            if (iter != NULL) {
                getIter(iter, 0, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                if (!CHECK_RESULT(result)) {
                    if (is_iterStop(result->value, inter)) {
                        freeResult(result);
                        break;
                    } else {
                        set_result = false;
                        goto not_else;
                    }
                }
                element = result->value;
                result->value = NULL;
                freeResult(result);
            } else if (first_yield != NULL){
                element = first_yield;
                first_yield = NULL;
            } else {
                if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(for_list->condition, var_list, result, belong))){
                    set_result = false;
                    goto not_else;
                }
                if (result->is_yield){
                    element = result->value;
                    result->value = NULL;
                    freeResult(result);
                } else {
                    freeResult(result);
                    break;
                }
            }

            assCore(for_list->var, element, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            gc_freeTmpLink(&element->gc_status);
            if (!CHECK_RESULT(result)){
                set_result = false;
                goto not_else;
            }
            freeResult(result);
        }

        do_for_st:
        if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(for_st, var_list, result, belong))){
            result_from = info_vl_branch;
            set_result = false;
            goto not_else;
        }
        else if (result->type == R_break)
            is_break = true;

        freeResult(result);
        if (after_st == NULL)
            continue;
        do_after:
        if (cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(after_st, var_list, result, belong))){
            result_from = info_after_do;
            set_result = false;
            goto not_else;
        }
        else if (result->type == R_break) {
            freeResult(result);
            goto not_else;
        }
        freeResult(result);
    }

    run_else:
    if (!is_break && else_st != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, belong))) {
        result_from = info_else_branch;
        set_result = false;
    }
    else
        freeResult(result);

    not_else:
    if (finally != NULL && cycleBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, belong))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        result_from = info_finally_branch;
        *result = finally_tmp;
    }
    else
        freeResult(&finally_tmp);

    if (yield_run) {
        if (result->type == R_yield)
            if (result_from == info_finally_branch) {
                freeRunInfo(st);
                newBranchYield(st, result->node, for_list, var_list, result_from, inter);
            } else
                updateBranchYield(st, result->node, for_list, result_from);
        else
            freeRunInfo(st);
        iter = NULL;
    } else {
        if (result->type == R_yield)
            if (result_from == info_finally_branch)
                newBranchYield(st, result->node, for_list, var_list, result_from, inter);
            else {
                newForBranchYield(st, result->node, for_list, var_list, result_from, inter, iter);
                iter = NULL;
            }
        else {
            popVarList(var_list);
        }
    }
    if (iter != NULL)
        gc_freeTmpLink(&iter->gc_status);
    if (set_result)
        setResult(result, inter);
    return result->type;
}

ResultType withBranch(INTER_FUNCTIONSIG) {
    StatementList *with_list = st->u.with_branch.with_list;
    Statement *else_st = st->u.with_branch.else_list;
    Statement *finally = st->u.with_branch.finally;
    Statement *vl_info = NULL;
    VarList *new = NULL;
    LinkValue *_enter_ = NULL;
    LinkValue *_exit_ = NULL;
    LinkValue *value = NULL;
    LinkValue *with_belong = belong;
    bool set_result = true;
    bool yield_run;
    enum StatementInfoStatus result_from = info_vl_branch;

    Result finally_tmp;
    Result else_tmp;
    Result exit_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);
    setResultCore(&else_tmp);
    setResultCore(&exit_tmp);

    if ((yield_run = st->info.have_info)){
        value = st->info.branch.with_.value;
        _enter_ = st->info.branch.with_._enter_;
        _exit_ = st->info.branch.with_._exit_;
        if (st->info.var_list != NULL) {
            new = st->info.var_list;
            new->next = var_list;
        }
        if (st->info.branch.status == info_vl_branch)
            vl_info = st->info.node;
        else if (st->info.branch.status == info_else_branch) {
            else_st = st->info.node;
            goto run_else;
        }
        else if (st->info.branch.status == info_finally_branch){
            finally = st->info.node;
            goto run_finally;
        }
    }
    else {
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(with_list->condition, var_list, result, belong))) {
            set_result = false;
            goto run_finally;
        }

        if (with_list->var == NULL) {
            with_belong = result->value;
            new = copyVarListCore(result->value->value->object.var, inter);
            new->next = var_list;
            freeResult(result);
        } else {
            LinkValue *enter_value = NULL;
            value = result->value;
            result->value = NULL;

            _enter_ = findAttributes(inter->data.object_enter, false, value, inter);
            _exit_ = findAttributes(inter->data.object_exit, false, value, inter);
            freeResult(result);
            if (_enter_ == NULL || _exit_ == NULL) {
                gc_freeTmpLink(&value->gc_status);
                _enter_ = NULL;
                _exit_ = NULL;
                value = NULL;
                setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(__enter__/__exit__), true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                set_result = false;
                goto run_finally;
            }

            gc_addTmpLink(&_enter_->gc_status);
            gc_addTmpLink(&_exit_->gc_status);
            callBackCore(_enter_, NULL, st->line, st->code_file, 0,
                         CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, value));
            if (!CHECK_RESULT(result)) {
                set_result = false;
                gc_freeTmpLink(&value->gc_status);
                gc_freeTmpLink(&_enter_->gc_status);
                gc_freeTmpLink(&_exit_->gc_status);
                goto run_finally;
            }

            new = pushVarList(var_list, inter);
            enter_value = result->value;
            freeResult(result);
            assCore(with_list->var, enter_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(new, result, belong));
            if (!CHECK_RESULT(result)) {
                set_result = false;
                popVarList(new);
                gc_freeTmpLink(&value->gc_status);
                gc_freeTmpLink(&_enter_->gc_status);
                gc_freeTmpLink(&_exit_->gc_status);
                goto run_finally;
            }
            freeResult(result);
        }
    }

    gc_freeze(inter, new, var_list, true);
    if (vl_info == NULL)
        vl_info = with_list->code;
    if (tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(vl_info, new, result, with_belong))) {
        set_result = false;
        if (result->type == R_yield)
            goto run_finally;
    }
    else
        freeResult(result);

    run_else:
    if (else_st != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, new, &else_tmp, with_belong))) {
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = else_tmp;
        result_from = info_else_branch;
        if (result->type == R_yield)
            goto run_finally;
    }
    else
        freeResult(&else_tmp);

    if (_exit_ != NULL) {
        callBackCore(_exit_, NULL, st->line, st->code_file, 0,
                     CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &exit_tmp, value));
        if (!RUN_TYPE(exit_tmp.type)) {
            if (!set_result)
                freeResult(result);
            set_result = false;
            *result = exit_tmp;
        }
        else
            freeResult(&exit_tmp);
        if (!yield_run){
            gc_freeTmpLink(&value->gc_status);
            gc_freeTmpLink(&_enter_->gc_status);
            gc_freeTmpLink(&_exit_->gc_status);
        }
    }

    run_finally:
    if (finally != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, belong))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
        result_from = info_finally_branch;
    }
    else
        freeResult(&finally_tmp);

    gc_freeze(inter, new, var_list, false);
    if (yield_run)
        if (result->type == R_yield)
            if (result_from == info_finally_branch) {
                freeRunInfo(st);
                newBranchYield(st, result->node, with_list, NULL, result_from, inter);
            }
            else
                updateBranchYield(st, result->node, with_list, result_from);
        else
            freeRunInfo(st);
    else {
        if (result->type == R_yield)
            if (result_from == info_finally_branch) {
                if (value != NULL) {
                    gc_freeTmpLink(&value->gc_status);
                    gc_freeTmpLink(&_enter_->gc_status);
                    gc_freeTmpLink(&_exit_->gc_status);
                }
                newBranchYield(st, result->node, with_list, NULL, result_from, inter);
                popVarList(new);
            }
            else
                newWithBranchYield(st, result->node, with_list, new, result_from, inter, value, _exit_, _enter_);
        else
            popVarList(new);
    }

    if (set_result)
        setResult(result, inter);
    return result->type;
}

ResultType tryBranch(INTER_FUNCTIONSIG) {
    StatementList *except_list = st->u.try_branch.except_list;
    Statement *try = st->u.try_branch.try;
    Statement *else_st = st->u.try_branch.else_list;
    Statement *finally = st->u.try_branch.finally;
    Statement *info_vl = NULL;
    LinkValue *error_value = NULL;
    bool set_result = true;
    bool yield_run;
    enum StatementInfoStatus result_from = info_first_do;

    Result finally_tmp;
    setResultCore(result);
    setResultCore(&finally_tmp);

    yield_run = popStatementVarList(st, &var_list, var_list, inter);
    if (yield_run && st->info.branch.status == info_first_do)
        try = st->info.node;
    else if (yield_run && st->info.branch.status == info_vl_branch){
        try = NULL;
        info_vl = st->info.node;
        goto run_except;
    }
    else if (yield_run && st->info.branch.status == info_else_branch){
        try = NULL;
        else_st = st->info.node;
        goto run_else;
    }
    else if (yield_run && st->info.branch.status == info_finally_branch){
        try = NULL;
        else_st = NULL;
        finally = st->info.node;
        goto run_finally;
    }

    if (try != NULL && !tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(try, var_list, result, belong))){
        if (result->type == R_yield) {
            result_from = info_first_do;
            goto run_finally;
        }
        freeResult(result);

        run_else:
        if (else_st != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(else_st, var_list, result, belong))) {
            set_result = false;
            result_from = info_else_branch;
        }
        else
            freeResult(result);
    } else if (try != NULL) {
        if (except_list == NULL) {
            set_result = false;
            result_from = info_first_do;
            goto run_finally;
        }

        error_value = result->value;
        result->value = NULL;
        for (PASS; except_list != NULL; except_list = except_list->next) {
            if (except_list->condition == NULL)
                break;
            freeResult(result);
            if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(except_list->condition, var_list, result, belong))) {
                set_result = false;
                goto run_finally;
            }
            if (result->value->value == error_value->value || checkAttribution(error_value->value, result->value->value))
                break;
        }
        if (except_list == NULL) {
            gc_freeTmpLink(&error_value->gc_status);
            goto run_finally;
        }
        freeResult(result);
        if (except_list->var != NULL) {
            assCore(except_list->var, error_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            if (!CHECK_RESULT(result)) {
                set_result = false;
                goto run_finally;
            }
            freeResult(result);
        }
        gc_freeTmpLink(&error_value->gc_status);
        info_vl = except_list->code;

        run_except:
        freeResult(result);
        if (tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(info_vl, var_list, result, belong))) {
            result_from = info_vl_branch;
            set_result = false;
        } else
            freeResult(result);
    }

    run_finally:
    if (finally != NULL && tryBranchSafeInterStatement(CALL_INTER_FUNCTIONSIG(finally, var_list, &finally_tmp, belong))){
        if (!set_result)
            freeResult(result);
        set_result = false;
        *result = finally_tmp;
        result_from = info_finally_branch;
    }
    else
        freeResult(&finally_tmp);

    if (yield_run)
        if (result->type == R_yield)
            updateBranchYield(st, result->node, except_list, result_from);
        else
            freeRunInfo(st);
    else
        if (result->type == R_yield)
            newBranchYield(st, result->node, except_list, var_list, result_from, inter);
        else
            var_list = popVarList(var_list);

    if (set_result)
        setResult(result, inter);
    return result->type;
}

ResultType breakCycle(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.break_cycle.times == NULL)
        goto not_times;

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.break_cycle.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_break;
        result->times = times_int;
    }
    return result->type;
}

ResultType continueCycle(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.continue_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.continue_cycle.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_continue;
        result->times = times_int;
    }
    return result->type;
}

ResultType regoIf(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.rego_if.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.rego_if.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_rego;
        result->times = times_int;
    }
    return result->type;
}

ResultType restartCode(INTER_FUNCTIONSIG){
    int times_int = 0;
    setResultCore(result);
    if (st->u.restart.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.restart.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_restart;
        result->times = times_int;
    }
    return result->type;
}

ResultType returnCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (st->u.return_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.return_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_func;
    return result->type;
}

ResultType yieldCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (st->u.yield_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.yield_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_yield;
    return result->type;
}

ResultType raiseCode(INTER_FUNCTIONSIG){
    setResultCore(result);
    if (st->u.raise_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_error;
    result->error = connectError(makeError(L"RaiseException", L"Exception was raise by user", st->line, st->code_file), result->error);
    return result->type;
}

ResultType assertCode(INTER_FUNCTIONSIG){
    bool result_;
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.raise_code.value, var_list, result, belong)))
        return result->type;

    result_ = checkBool(result->value, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    else if (result_)
        setResult(result, inter);
    else
        setResultErrorSt(E_AssertException, L"Assertion check error", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType gotoLabel(INTER_FUNCTIONSIG){
    int times_int = 0;
    wchar_t *label = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.label, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, V_str)) {
        setResultErrorSt(E_TypeException, ONLY_ACC(label name, V_str), true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return result->type;
    }
    label = memWidecpy(result->value->value->data.str.str);

    freeResult(result);
    if (st->u.goto_.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.times, var_list, result, belong))) {
        memFree(label);
        return result->type;
    }
    if (!checkNumber(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong))) {
        memFree(label);
        return result->type;
    }
    times_int = (int)result->value->value->data.num.num;
    freeResult(result);
    not_times:
    if (st->u.goto_.return_ == NULL)
        setResult(result, inter);
    else if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.goto_.return_, var_list, result, belong))) {
        memFree(label);
        return result->type;
    }

    result->times = times_int;
    result->type = R_goto;
    result->label = label;
    return result->type;
}

ResultType runLabel(INTER_FUNCTIONSIG) {
    LinkValue *goto_value = result->value;  // goto的值通过result传入, 因此不能进行setResultCore
    result->value = NULL;
    freeResult(result);

    if (st->u.label_.as != NULL)
        assCore(st->u.label_.as, goto_value, false, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    gc_freeTmpLink(&goto_value->gc_status);
    if (st->u.label_.as != NULL && !CHECK_RESULT(result))
        return result->type;

    freeResult(result);
    if (st->u.label_.command != NULL)
        operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.label_.command, var_list, result, belong));
    else
        setResult(result, inter);

    return result->type;
}
