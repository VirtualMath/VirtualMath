#include "__run.h"

static bool checkNumber(FUNC){
    if (!isType(result->value->value, V_int)) {
        setResultErrorSt(E_TypeException, L"Don't get a int of layers", true, st, CNEXT_NT);
        return false;
    }
    return true;
}

static void newBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status, Inter *inter){
    if (new_var != NULL)
        new_var->next = NULL;
    gc_freeze(inter, new_var, NULL, true);
    branch_st->info.var_list = new_var;
    branch_st->info.node = node->type == yield_code ? node->next : node;
    branch_st->info.branch.sl_node = sl_node;
    branch_st->info.branch.status = status;
    branch_st->info.have_info = true;
}

static void newWithBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *value, LinkValue *_exit_, LinkValue *_enter_, LinkValue *with){
    newBranchYield(branch_st, node, sl_node, new_var, status, inter);
    branch_st->info.branch.with_.value = value;
    branch_st->info.branch.with_._exit_ = _exit_;
    branch_st->info.branch.with_._enter_ = _enter_;
    branch_st->info.branch.with_.with_belong = with;
}

static void newForBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, VarList *new_var, enum StatementInfoStatus status,
                        Inter *inter, LinkValue *iter){
    newBranchYield(branch_st, node, sl_node, new_var, status, inter);
    branch_st->info.branch.for_.iter = iter;
    gc_addTmpLink(&iter->gc_status);
}

static void updateBranchYield(Statement *branch_st, Statement *node, StatementList *sl_node, enum StatementInfoStatus status){
    branch_st->info.node = node->type == yield_code ? node->next : node;
    branch_st->info.branch.sl_node = sl_node;
    branch_st->info.branch.status = status;
    branch_st->info.have_info = true;
}

static void setBranchResult(bool yield_run, StatementList *sl, Statement *st, Result *result, enum StatementInfoStatus status, FUNC_CORE) {
    if (yield_run) {
        if (result->type == R_yield)
            updateBranchYield(st, result->node, sl, status);
        else
            freeRunInfo(st);
    } else {
        if (result->type == R_yield)
            newBranchYield(st, result->node, sl, var_list, status, inter);
        else
            var_list = popVarList(var_list);
    }
}

static bool runBranchHeard(Statement *condition, Statement *var, LinkValue **condition_value, FUNC_NT) {
    setResultCore(result);
    if (operationSafeInterStatement(CFUNC(condition, var_list, result, belong)))
        return false;

    *condition_value = result->value;
    if (var != NULL) {
        result->value = NULL;
        freeResult(result);
        assCore(var, *condition_value, false, false, CNEXT_NT);
        gc_freeTmpLink(&(*condition_value)->gc_status);

        if (!CHECK_RESULT(result))
            return false;
    }
    return true;
}

static int checkCondition(bool is_rego, LinkValue *condition_value, fline line, char *file, FUNC_NT) {
    bool return_;
    if (is_rego)
        return 1;

    return_ = checkBool(condition_value, line, file, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return -1;
    return return_;
}

static bool runFinally(bool set_result, bool (*run)(FUNC), FUNC) {
    Result finally_tmp;
    setResultCore(&finally_tmp);
    if (run(CFUNC(st, var_list, &finally_tmp, belong))) {
        if (!set_result)
            freeResult(result);
        *result = finally_tmp;
        return false;
    }
    freeResult(&finally_tmp);
    return true;
}

static int runIfList(StatementList *if_list, Statement *info_vl, fline line, char *file, FUNC_NT) {
    bool is_rego = false;
    setResultCore(result);

    for (PASS; if_list != NULL; if_list = if_list->next){
        bool condition;
        if (info_vl != NULL){
            if (ifBranchSafeInterStatement(CFUNC(info_vl, var_list, result, belong)))
                return -1;
            if (result->type == R_rego)
                is_rego = true;
            info_vl = NULL;
        } else if (if_list->type == if_b){
            LinkValue *condition_value = NULL;
            if (!runBranchHeard(if_list->condition, if_list->var, &condition_value, CNEXT_NT))
                return -1;

            freeResult(result);
            condition = checkCondition(is_rego, condition_value, line, file, CNEXT_NT);

            if (condition == -1)
                return -1;
            else if (condition == 1){
                is_rego = false;
                freeResult(result);
                if (ifBranchSafeInterStatement(CFUNC(if_list->code, var_list, result, belong)))
                    return -1;
                if (result->type == R_rego)
                    is_rego = true;
                else {
                    freeResult(result);
                    return 0;
                }
            }
        } else{
            if (ifBranchSafeInterStatement(CFUNC(if_list->code, var_list, result, belong)))
               return -1;
            if (result->type == R_rego)
                is_rego = true;
        }
        freeResult(result);
    }
    return 1;
}

ResultType ifBranch(FUNC) {
    StatementList *if_list = NULL;
    Statement *else_st = NULL;
    Statement *finally = NULL;
    Statement *info_vl = NULL;
    bool set_result = true;
    bool yield_run = false;
    enum StatementInfoStatus result_from = info_vl_branch;

    setResultCore(result);
    yield_run = popYieldVarList(st, &var_list, var_list, inter);

    if (yield_run) {
        if (st->info.branch.status == info_vl_branch){
            if_list = st->info.branch.sl_node;
            info_vl = st->info.node;
            else_st = st->u.if_branch.else_list;
            finally = st->u.if_branch.finally;
            if (info_vl == NULL)
                if_list = NULL;  // 证明if分支的yield已经到头了
        } else if (st->info.branch.status == info_else_branch) {
            else_st = st->info.node;
            finally = st->u.if_branch.finally;
        } else if (st->info.branch.status == info_finally_branch)
            finally = st->info.node;
        else {
            var_list = popVarList(var_list);
            setResultError(E_SystemException, L"yield info error", st->line, st->code_file, true, CNEXT_NT);
            return R_error;
        }
    } else {
        if_list = st->u.if_branch.if_list;
        else_st = st->u.if_branch.else_list;
        finally = st->u.if_branch.finally;
    }

    if (if_list != NULL) {
        int status = runIfList(if_list, info_vl, st->line, st->code_file, CNEXT_NT);
        if (status == -1 || status == 0)
            else_st = NULL;

        if (status == -1)
            set_result = false;
    }

    if (else_st != NULL) {
        if (ifBranchSafeInterStatement(CFUNC(else_st, var_list, result, belong))) {
            set_result = false;
            result_from = info_else_branch;
        } else
            freeResult(result);
    }

    if (finally != NULL && !runFinally(set_result, ifBranchSafeInterStatement, finally, CNEXT_NT)) {
        set_result = false;
        result_from = info_finally_branch;
    }

    setBranchResult(yield_run, if_list, st, result, result_from, CFUNC_CORE(var_list));
    if (set_result)
        setResult(result, inter);
    return result->type;
}

static int runWhileList(StatementList *while_list, Statement *after, Statement *info_vl, Statement *after_vl, bool do_while, fline line, char *file, enum StatementInfoStatus *from, FUNC_NT) {
    bool is_break = false;
    setResultCore(result);

    while (!is_break){
        LinkValue *condition_value = NULL;
        Statement *after_st = after;
        Statement *while_st = while_list->code;
        bool condition;

        *from = info_vl_branch;
        if (info_vl != NULL) {
            condition = 1;
            while_st = info_vl;
            info_vl = NULL;
        } else if (after_vl != NULL) {
            condition = -2;
            after_st = after_vl;
            after_vl = NULL;
        } else{
            if (!runBranchHeard(while_list->condition, while_list->var, &condition_value, CNEXT_NT))
                return -1;

            freeResult(result);
            condition = checkCondition(do_while, condition_value, line, file, CNEXT_NT);
        }

        do_while = false;
        if (condition == -1)
            return -1;
        else if (condition == 1){
            freeResult(result);
            if (cycleBranchSafeInterStatement(CFUNC(while_st, var_list, result, belong)))
                return -1;
            else if (result->type == R_break)
                is_break = true;
        } else if (condition != -2){
            freeResult(result);
            break;
        }

        if (after_st != NULL) {
            *from = info_after_do;
            freeResult(result);
            if (cycleBranchSafeInterStatement(CFUNC(after_st, var_list, result, belong)))
                return -1;
            else if (result->type == R_break) {
                freeResult(result);
                return 0;
            }
        }
        freeResult(result);
    }
    return is_break ? 0 : 1;
}

ResultType whileBranch(FUNC) {
    StatementList *while_list = st->u.while_branch.while_list;
    Statement *first = st->u.while_branch.first;
    Statement *after = st->u.while_branch.after;
    Statement *else_st = st->u.while_branch.else_list;
    Statement *finally = st->u.while_branch.finally;

    Statement *info_vl = NULL;
    Statement *after_vl = NULL;

    bool set_result = true;
    bool yield_run = false;
    bool do_while = st->u.while_branch.type == do_while_;
    enum StatementInfoStatus result_from = info_first_do;

    setResultCore(result);
    yield_run = popYieldVarList(st, &var_list, var_list, inter);
    if (yield_run) {
        if (st->info.branch.status == info_first_do)
            first = st->info.node;
        else if (st->info.branch.status == info_vl_branch){
            first = NULL;
            info_vl = st->info.node;
        }
        else if (st->info.branch.status == info_after_do){
            first = NULL;
            after_vl = st->info.node;
        }
        else if (st->info.branch.status == info_else_branch){
            first = NULL;
            while_list = NULL;
            else_st = st->info.node;
        }
        else if (st->info.branch.status == info_finally_branch){
            first = NULL;
            while_list = NULL;
            else_st = NULL;
            finally = st->info.node;
        } else {
            var_list = popVarList(var_list);
            setResultError(E_SystemException, L"yield info error", st->line, st->code_file, true, CNEXT_NT);
            return R_error;
        }
    }

    if (first != NULL) {
        if (cycleBranchSafeInterStatement(CFUNC(first, var_list, result, belong))) {
            set_result = false;
            while_list = NULL;
            else_st = NULL;
        } else
            freeResult(result);
    }

    if (while_list != NULL){
        int status = runWhileList(while_list, after, info_vl, after_vl, do_while, st->line, st->code_file, &result_from, CNEXT_NT);
        if (status == -1 || status == 0)
            else_st = NULL;

        if (status == -1)
            set_result = false;
    }

    if (else_st != NULL) {
        if (cycleBranchSafeInterStatement(CFUNC(else_st, var_list, result, belong))) {
            result_from = info_else_branch;
            set_result = false;
        } else
            freeResult(result);
    }

    if (finally != NULL && !runFinally(set_result, cycleBranchSafeInterStatement, finally, CNEXT_NT)) {
        set_result = false;
        result_from = info_finally_branch;
    }

    setBranchResult(yield_run, while_list, st, result, result_from, CFUNC_CORE(var_list));
    if (set_result)
        setResult(result, inter);
    return result->type;
}

static int getForHeard(LinkValue **iter, LinkValue **first_yield, StatementList *for_list, fline line, char *file, FUNC_NT) {
    LinkValue *tmp = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CFUNC(for_list->condition, var_list, result, belong)))
        return -1;
    if (result->is_yield){
        iter = NULL;
        GET_RESULTONLY(*first_yield, result);
        return 1;
    }
    GET_RESULT(tmp, result);
    getIter(tmp, 1, line, file, CNEXT_NT);
    gc_freeTmpLink(&tmp->gc_status);
    if (!CHECK_RESULT(result))
        return -1;
    GET_RESULTONLY(*iter, result);
    return 0;
}

static int runForHeard(LinkValue *iter, LinkValue *first_yield, StatementList *for_list, fline line, char *file, FUNC_NT) {
    LinkValue *element = NULL;
    if (iter != NULL) {
        getIter(iter, 0, line, file, CNEXT_NT);
        if (!CHECK_RESULT(result)) {
            if (is_iterStop(result->value, inter)) {
                freeResult(result);
                return 1;
            } else
                return -1;
        }
        GET_RESULT(element, result);
    } else if (first_yield != NULL)
        element = first_yield;
    else {
        if (operationSafeInterStatement(CFUNC(for_list->condition, var_list, result, belong)))
            return -1;
        GET_RESULT(element, result);
        if (!result->is_yield)
            return 1;
    }

    assCore(for_list->var, element, false, false, CNEXT_NT);
    gc_freeTmpLink(&element->gc_status);
    if (!CHECK_RESULT(result))
        return -1;
    freeResult(result);
    return 0;
}

static int runForList(StatementList *for_list, Statement *after, Statement *info_vl, Statement *after_vl, LinkValue *iter, LinkValue *first_yield, fline line, char *file, enum StatementInfoStatus *from, FUNC_NT) {
    bool is_break = false;
    while (!is_break){  // 循环执行的本体
        Statement *for_st = for_list->code;
        Statement *after_st = after;

        *from = info_vl_branch;
        if (info_vl != NULL){
            for_st = info_vl;
            info_vl = NULL;
        }
        else if (after_vl != NULL){
            after_st = after_vl;
            after_vl = NULL;
            for_st = NULL;
        } else {
            int status = runForHeard(iter, first_yield, for_list, line, file, CNEXT_NT);
            first_yield = NULL;
            if (status == -1)
                return -1;
            else if (status == 1)
                break;
        }

        if (for_st != NULL) {
            if (cycleBranchSafeInterStatement(CFUNC(for_st, var_list, result, belong)))
                return -1;
            else if (result->type == R_break)
                is_break = true;
        }

        *from = info_after_do;
        if (after_st != NULL) {
            freeResult(result);
            if (cycleBranchSafeInterStatement(CFUNC(after_st, var_list, result, belong)))
                return -1;
            else if (result->type == R_break)
                is_break = true;
        }
        freeResult(result);
    }
    return is_break ? 0 : 1;
}

static void setForResult(bool yield_run, StatementList *sl, Statement *st, Result *result, LinkValue *iter, enum StatementInfoStatus status, FUNC_CORE) {
    if (yield_run) {
        if (result->type == R_yield)
            updateBranchYield(st, result->node, sl, status);
        else
            freeRunInfo(st);
    } else {
        if (result->type == R_yield)
            newForBranchYield(st, result->node, sl, var_list, status, inter, iter);
        else
            popVarList(var_list);
    }
}

ResultType forBranch(FUNC) {
    StatementList *for_list = st->u.for_branch.for_list;
    Statement *first = st->u.for_branch.first_do;
    Statement *after = st->u.for_branch.after_do;
    Statement *else_st = st->u.for_branch.else_list;
    Statement *finally = st->u.for_branch.finally;
    Statement *info_vl = NULL;
    Statement *after_vl = NULL;
    bool set_result = true;
    bool yield_run = false;
    bool heard = true;
    LinkValue *first_yield = NULL;
    LinkValue *iter = NULL;
    enum StatementInfoStatus result_from = info_first_do;

    setResultCore(result);

    yield_run = popYieldVarList(st, &var_list, var_list, inter);
    if (yield_run) {
        if (st->info.branch.status == info_first_do)
            first = st->info.node;
        else if (st->info.branch.status == info_vl_branch) {
            first = NULL;
            info_vl = st->info.node;
            iter = st->info.branch.for_.iter;
            heard = false;
        } else if (st->info.branch.status == info_after_do) {
            first = NULL;
            after_vl = st->info.node;
            iter = st->info.branch.for_.iter;
            heard = false;
        } else if (st->info.branch.status == info_else_branch) {
            first = NULL;
            heard = false;
            for_list = false;
            else_st = st->info.node;
        } else if (st->info.branch.status == info_finally_branch) {
            first = NULL;
            heard = false;
            for_list = false;
            else_st = NULL;
            finally = st->info.node;
        } else {
            var_list = popVarList(var_list);
            setResultError(E_SystemException, L"yield info error", st->line, st->code_file, true, CNEXT_NT);
            return R_error;
        }
    }

    if (first != NULL) {
        if (cycleBranchSafeInterStatement(CFUNC(first, var_list, result, belong))) {
            set_result = false;
            heard = false;
            for_list = NULL;
            else_st = NULL;
        } else
            freeResult(result);
    }

    if (heard){
        int status = getForHeard(&iter, &first_yield, for_list, st->line, st->code_file, CNEXT_NT);
        if (status == -1) {
            set_result = false;
            for_list = NULL;
            else_st = NULL;
        } else
            freeResult(result);
    }

    if (for_list != NULL){
        int status = runForList(for_list, after, info_vl, after_vl, iter, first_yield, st->line, st->code_file, &result_from, CNEXT_NT);
        if (status == -1 || status == 0)
            else_st = NULL;

        if (status == -1)
            set_result = false;
    }

    if (else_st != NULL) {
        if (cycleBranchSafeInterStatement(CFUNC(else_st, var_list, result, belong))) {
            result_from = info_else_branch;
            set_result = false;
        }
        else
            freeResult(result);
    }

    if (finally != NULL && !runFinally(set_result, cycleBranchSafeInterStatement, finally, CNEXT_NT)) {
        set_result = false;
        result_from = info_finally_branch;
    }

    setForResult(yield_run, for_list, st, result, iter, result_from, CFUNC_CORE(var_list));
    if (!yield_run && iter != NULL)
        gc_freeTmpLink(&iter->gc_status);

    if (set_result)
        setResult(result, inter);
    return result->type;
}

static bool getEnterExit(LinkValue *value, LinkValue **_enter_, LinkValue **_exit_, fline line, char *file, FUNC_NT) {
    setResultCore(result);

    *_enter_ = findAttributes(inter->data.mag_func[M_ENTER], false, LINEFILE, true, CFUNC_NT(var_list, result, value));
    if (!CHECK_RESULT(result))
        return false;
    freeResult(result);

    *_exit_ = findAttributes(inter->data.mag_func[M_EXIT], false, LINEFILE, true, CFUNC_NT(var_list, result, value));
    if (!CHECK_RESULT(result))
        return false;
    freeResult(result);

    if (*_enter_ == NULL || *_exit_ == NULL) {
        *_enter_ = NULL;
        *_exit_ = NULL;
        setResultError(E_TypeException, OBJ_NOTSUPPORT(__enter__/__exit__), line, file, true, CNEXT_NT);
        return false;
    }
    gc_addTmpLink(&(*_enter_)->gc_status);
    gc_addTmpLink(&(*_exit_)->gc_status);
    return true;
}

static int runWithList(StatementList *with_list, LinkValue **with_belong, LinkValue **value, VarList **new, LinkValue **_enter_, LinkValue **_exit_, fline line, char *file, FUNC_NT) {
    if (operationSafeInterStatement(CFUNC(with_list->condition, var_list, result, belong)))
        return -1;

    GET_RESULT(*value, result);
    if (with_list->var == NULL) {
        *with_belong = *value;
        gc_addTmpLink(&(*with_belong)->gc_status);

        *new = copyVarListCore((*value)->value->object.var, inter);
        (*new)->next = var_list;

        *_enter_ = NULL;
        *_exit_ = NULL;
    } else {
        LinkValue *enter_value = NULL;
        *with_belong = belong;
        gc_addTmpLink(&(*with_belong)->gc_status);

        *_enter_ = findAttributes(inter->data.mag_func[M_ENTER], false, LINEFILE, true, CFUNC_NT(var_list, result, *value));
        if (!CHECK_RESULT(result))
            goto error_;
        freeResult(result);

        *_exit_ = findAttributes(inter->data.mag_func[M_EXIT], false, LINEFILE, true, CFUNC_NT(var_list, result, *value));
        if (!CHECK_RESULT(result))
            goto error_;
        freeResult(result);

        if (!getEnterExit(*value, _enter_, _exit_, line, file, CNEXT_NT)) {
            error_:
            gc_freeTmpLink(&(*value)->gc_status);
            gc_freeTmpLink(&(*with_belong)->gc_status);
            *value = NULL;
            return -1;
        }

        callBackCore(*_enter_, NULL, line, file, 0, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return 0;

        *new = pushVarList(var_list, inter);
        enter_value = result->value;
        freeResult(result);
        assCore(with_list->var, enter_value, false, false, CFUNC_NT(*new, result, belong));
        if (!CHECK_RESULT(result)) {
            *new = popVarList(*new);
            return 0;
        }
        freeResult(result);
    }
    return 1;
}

static void setWithResult(bool yield_run, StatementList *sl, Statement *st, Result *result, LinkValue *value, LinkValue *_enter_, LinkValue *_exit_, LinkValue *with, enum StatementInfoStatus status, FUNC_CORE) {
    if (yield_run) {
        if (result->type == R_yield)
            if (status == info_finally_branch) {
                freeRunInfo(st);
                newBranchYield(st, result->node, sl, NULL, status, inter);
            } else
                updateBranchYield(st, result->node, sl, status);
        else
            freeRunInfo(st);
    } else {
        if (result->type == R_yield)
            if (status == info_finally_branch) {
                newBranchYield(st, result->node, sl, NULL, status, inter);
                popVarList(var_list);
            }
            else
                newWithBranchYield(st, result->node, sl, var_list, status, inter, value, _exit_, _enter_, with);
        else {
            if (var_list != NULL)
                popVarList(var_list);
        }
    }
}

ResultType withBranch(FUNC) {
    StatementList *with_list = st->u.with_branch.with_list;
    Statement *else_st = st->u.with_branch.else_list;
    Statement *finally = st->u.with_branch.finally;
    Statement *vl_info = NULL;
    VarList *new = NULL;
    LinkValue *_enter_ = NULL;
    LinkValue *_exit_ = NULL;
    LinkValue *value = NULL;
    LinkValue *with_belong = NULL;  // with在运行的时候调整belong, 就类似于定义class的时候调整belong
    bool set_result = true;
    bool run_block = true;
    bool run_exit = true;
    bool yield_run;
    enum StatementInfoStatus result_from = info_vl_branch;

    setResultCore(result);
    if ((yield_run = st->info.have_info)){
        value = st->info.branch.with_.value;
        _enter_ = st->info.branch.with_._enter_;
        _exit_ = st->info.branch.with_._exit_;
        with_belong = st->info.branch.with_.with_belong;
        if (st->info.var_list != NULL) {
            new = st->info.var_list;
            new->next = var_list;
        }

        if (st->info.branch.status == info_vl_branch) {
            vl_info = st->info.node;
            if (vl_info == NULL)
                run_block = false;
        }
        else if (st->info.branch.status == info_else_branch) {
            run_block = false;
            else_st = st->info.node;
        }
        else if (st->info.branch.status == info_finally_branch){
            run_block = false;
            else_st = NULL;
            finally = st->info.node;
        }
    } else {
        int status = runWithList(with_list, &with_belong, &value, &new, &_enter_, &_exit_, st->line, st->code_file, CNEXT_NT);
        if (status == -1) {
            set_result = false;
            run_block = false;
            else_st = NULL;
        } else if (status == 0) {
            set_result = false;
            gc_freeTmpLink(&value->gc_status);
            gc_freeTmpLink(&_enter_->gc_status);
            gc_freeTmpLink(&_exit_->gc_status);
            gc_freeTmpLink(&with_belong->gc_status);
            run_block = false;
            else_st = NULL;
        }
    }

    gc_freeze(inter, new, var_list, true);
    if (run_block) {
        if (vl_info == NULL)
            vl_info = with_list->code;
        if (withBranchSafeInterStatement(CFUNC(vl_info, new, result, with_belong))) {
            set_result = false;
            if (result->type == R_yield) {
                run_exit = false;
                else_st = NULL;
            }
        } else
            freeResult(result);
    }

    if (else_st != NULL){
        Result else_tmp;
        setResultCore(&else_tmp);
        if (withBranchSafeInterStatement(CFUNC(else_st, new, &else_tmp, with_belong))) {
            if (!set_result)
                freeResult(result);
            set_result = false;
            *result = else_tmp;
            result_from = info_else_branch;
            if (result->type == R_yield)
                run_exit = false;
        } else
            freeResult(&else_tmp);
    }

    if (run_exit){  // 若运行中的某处返回了R_yield则不执行exit
        if (_exit_ != NULL){
            Result exit_tmp;
            setResultCore(&exit_tmp);

            callBackCore(_exit_, NULL, st->line, st->code_file, 0, CFUNC_NT(var_list, &exit_tmp, belong));
            if (!RUN_TYPE(exit_tmp.type)) {
                if (!set_result)
                    freeResult(result);
                set_result = false;
                *result = exit_tmp;
            } else
                freeResult(&exit_tmp);

            if (!yield_run) {
                gc_freeTmpLink(&_enter_->gc_status);
                gc_freeTmpLink(&_exit_->gc_status);
                _enter_ = NULL;
                _exit_ = NULL;
            }
        }

        if (!yield_run) {
            if (with_belong != NULL){
                gc_freeTmpLink(&with_belong->gc_status);
                with_belong = NULL;
            }

            if (value != NULL){
                gc_freeTmpLink(&value->gc_status);
                value = NULL;
            }
        }
    }

    if (finally != NULL && !runFinally(set_result, withBranchSafeInterStatement, finally, CNEXT_NT)) {
        set_result = false;
        result_from = info_finally_branch;
    }

    gc_freeze(inter, new, var_list, false);
    setWithResult(yield_run, with_list, st, result, value, _enter_, _exit_, with_belong, result_from, CFUNC_CORE(new));
    if (set_result)
        setResult(result, inter);
    return result->type;
}

static int checkError(StatementList **except_list, LinkValue *error_value, FUNC_NT) {
    setResultCore(result);
    if (*except_list == NULL)
        return -1;
    for (PASS; *except_list != NULL; except_list = &(*except_list)->next) {
        freeResult(result);
        if ((*except_list)->condition == NULL)
            break;
        if (operationSafeInterStatement(CFUNC((*except_list)->condition, var_list, result, belong)))
            return 0;
        if (result->value->value == error_value->value || checkAttribution(error_value->value, result->value->value))
            break;
    }
    return *except_list == NULL ? -1 : 1;
}

ResultType tryBranch(FUNC) {
    StatementList *except_list = NULL;
    Statement *try = st->u.try_branch.try;
    Statement *else_st = st->u.try_branch.else_list;
    Statement *finally = st->u.try_branch.finally;
    Statement *info_vl = NULL;
    LinkValue *error_value = NULL;
    bool set_result = true;
    bool yield_run;
    enum StatementInfoStatus result_from = info_first_do;

    setResultCore(result);
    yield_run = popYieldVarList(st, &var_list, var_list, inter);
    if (yield_run && st->info.branch.status == info_first_do)
        try = st->info.node;
    else if (yield_run && st->info.branch.status == info_vl_branch){
        try = NULL;
        info_vl = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_else_branch){
        try = NULL;
        else_st = st->info.node;
    }
    else if (yield_run && st->info.branch.status == info_finally_branch){
        try = NULL;
        else_st = NULL;
        finally = st->info.node;
    }

    if (try != NULL){
        if (tryBranchSafeInterStatement(CFUNC(try, var_list, result, belong))) {
            if (result->type == R_yield) {
                result_from = info_first_do;
                set_result = false;
                else_st = NULL;
            } else {
                int status;
                GET_RESULT(error_value, result);

                except_list = st->u.try_branch.except_list;
                status = checkError(&except_list, error_value, CNEXT_NT);
                if (status == -1 || status == 0) {
                    set_result = false;
                    result_from = info_vl_branch;
                    except_list = NULL;
                    else_st = NULL;
                } else
                    freeResult(result);
                gc_freeTmpLink(&error_value->gc_status);
            }
        } else
            freeResult(result);
    }

    if (except_list != NULL) {
        if (except_list->var != NULL) {
            gc_addTmpLink(&error_value->gc_status);
            assCore(except_list->var, error_value, false, false, CNEXT_NT);
            gc_freeTmpLink(&error_value->gc_status);

            if (!CHECK_RESULT(result)) {
                set_result = false;
                else_st = NULL;
            } else
                freeResult(result);
        }
        info_vl = except_list->code;
    }

    if (info_vl != NULL) {
        freeResult(result);
        if (tryBranchSafeInterStatement(CFUNC(info_vl, var_list, result, belong))) {
            result_from = info_vl_branch;
            set_result = false;
        } else
            freeResult(result);
    }

    if (else_st != NULL) {
        if (tryBranchSafeInterStatement(CFUNC(else_st, var_list, result, belong))) {
            set_result = false;
            result_from = info_else_branch;
        }
        else
            freeResult(result);
    }

    if (finally != NULL && !runFinally(set_result, ifBranchSafeInterStatement, finally, CNEXT_NT)) {
        set_result = false;
        result_from = info_finally_branch;
    }

    setBranchResult(yield_run, except_list, st, result, result_from, CFUNC_CORE(var_list));

    if (set_result)
        setResult(result, inter);
    return result->type;
}

ResultType breakCycle(FUNC){
    int times_int = 0;
    setResultCore(result);
    if (st->u.break_cycle.times == NULL)
        goto not_times;

    if (operationSafeInterStatement(CFUNC(st->u.break_cycle.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CNEXT))
        return result->type;
    times_int = (int)result->value->value->data.int_.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_break;
        result->times = times_int;
    }
    return result->type;
}

ResultType continueCycle(FUNC){
    int times_int = 0;
    setResultCore(result);
    if (st->u.continue_cycle.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CFUNC(st->u.continue_cycle.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CNEXT))
        return result->type;
    times_int = (int)result->value->value->data.int_.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_continue;
        result->times = times_int;
    }
    return result->type;
}

ResultType regoIf(FUNC){
    int times_int = 0;
    setResultCore(result);
    if (st->u.rego_if.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CFUNC(st->u.rego_if.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CNEXT))
        return result->type;
    times_int = (int)result->value->value->data.int_.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_rego;
        result->times = times_int;
    }
    return result->type;
}

ResultType restartCode(FUNC){
    int times_int = 0;
    setResultCore(result);
    if (st->u.restart.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CFUNC(st->u.restart.times, var_list, result, belong)))
        return result->type;

    if (!checkNumber(CNEXT))
        return result->type;
    times_int = (int)result->value->value->data.int_.num;
    freeResult(result);

    not_times:
    setResult(result, inter);
    if (times_int >= 0) {
        result->type = R_restart;
        result->times = times_int;
    }
    return result->type;
}

ResultType returnCode(FUNC){
    setResultCore(result);
    if (st->u.return_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CFUNC(st->u.return_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_func;
    return result->type;
}

ResultType yieldCode(FUNC){
    setResultCore(result);
    if (st->u.yield_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CFUNC(st->u.yield_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_yield;
    return result->type;
}

ResultType raiseCode(FUNC){
    setResultCore(result);
    if (st->u.raise_code.value == NULL) {
        setResult(result, inter);
        goto set_result;
    }

    if (operationSafeInterStatement(CFUNC(st->u.raise_code.value, var_list, result, belong)))
        return result->type;

    set_result:
    result->type = R_error;
    result->error = connectError(makeError(L"RaiseException", L"Exception was raise by user", st->line, st->code_file), result->error);
    return result->type;
}

ResultType assertCode(FUNC){
    bool result_;
    LinkValue *opt;
    setResultCore(result);

    if (inter->data.assert_run == assert_ignore) {  // 不执行断言
        setResult(result, inter);
        return result->type;
    }

    if (operationSafeInterStatement(CFUNC(st->u.raise_code.value, var_list, result, belong)) || inter->data.assert_run == assert_run)
        return result->type;
    GET_RESULT(opt, result);

    result_ = checkBool(opt, st->line, st->code_file, CNEXT_NT);
    gc_freeTmpLink(&opt->gc_status);
    if (!CHECK_RESULT(result))
        return result->type;
    else if (result_)
        setResult(result, inter);
    else
        setResultErrorSt(E_AssertException, L"Assertion check error", true, st, CNEXT_NT);
    return result->type;
}

ResultType gotoLabel(FUNC){
    int times_int = 0;
    wchar_t *label = NULL;
    setResultCore(result);

    if (operationSafeInterStatement(CFUNC(st->u.goto_.label, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, V_str)) {
        setResultErrorSt(E_TypeException, ONLY_ACC(label name, V_str), true, st, CNEXT_NT);
        return result->type;
    }
    label = memWidecpy(result->value->value->data.str.str);

    freeResult(result);
    if (st->u.goto_.times == NULL)
        goto not_times;
    if (operationSafeInterStatement(CFUNC(st->u.goto_.times, var_list, result, belong))) {
        memFree(label);
        return result->type;
    }
    if (!checkNumber(CNEXT)) {
        memFree(label);
        return result->type;
    }
    times_int = (int)result->value->value->data.int_.num;
    freeResult(result);
    not_times:
    if (st->u.goto_.return_ == NULL)
        setResult(result, inter);
    else if (operationSafeInterStatement(CFUNC(st->u.goto_.return_, var_list, result, belong))) {
        memFree(label);
        return result->type;
    }

    result->times = times_int;
    result->type = R_goto;
    result->label = label;
    return result->type;
}

ResultType runLabel(FUNC) {
    LinkValue *goto_value;
    GET_RESULT(goto_value, result);  // goto的值通过result传入, 因此不能进行setResultCore
    if (st->u.label_.as != NULL)
        assCore(st->u.label_.as, goto_value, false, false, CNEXT_NT);

    gc_freeTmpLink(&goto_value->gc_status);
    if (st->u.label_.as != NULL && !CHECK_RESULT(result))
        return result->type;

    freeResult(result);
    if (st->u.label_.command != NULL)
        operationSafeInterStatement(CFUNC(st->u.label_.command, var_list, result, belong));
    else
        setResult(result, inter);

    return result->type;
}
