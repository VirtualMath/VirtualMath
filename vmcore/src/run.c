#include "__run.h"

/**
 * 运行单个statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
ResultType runStatement(FUNC) {
    ResultType type = R_not;
    bool run_gc = false;  // 若st已经没有下一部了则执行gc
    setResultCore(result);
    gc_addTmpLink(&belong->gc_status);

    switch (st->type) {
        case base_value:
            type = getBaseValue(CNEXT);
            break;
        case base_var:
            type = getVar(CNEXT, getBaseVarInfo);
            break;
        case base_svar:
            type = getVar(CNEXT, getBaseSVarInfo);
            run_gc = true;
            break;
        case base_list:
            type = getList(CNEXT);
            break;
        case base_dict:
            type = getDict(CNEXT);
            break;
        case base_lambda:
            type = setLambda(CNEXT);
            break;
        case operation:
            type = operationStatement(CNEXT);
            run_gc = true;
            break;
        case set_class:
            type = setClass(CNEXT);
            break;
        case set_function:
            type = setFunction(CNEXT);
            break;
        case slice_:
            type = elementSlice(CNEXT);
            break;
        case call_function:
            type = callBack(CNEXT);
            break;
        case if_branch:
            type = ifBranch(CNEXT);
            break;
        case while_branch:
            type = whileBranch(CNEXT);
            break;
        case for_branch:
            type = forBranch(CNEXT);
            break;
        case with_branch:
            type = withBranch(CNEXT);
            break;
        case try_branch:
            type = tryBranch(CNEXT);
            break;
        case break_cycle:
            type = breakCycle(CNEXT);
            break;
        case continue_cycle:
            type = continueCycle(CNEXT);
            break;
        case rego_if:
            type = regoIf(CNEXT);
            break;
        case restart:
            type = restartCode(CNEXT);
            break;
        case return_code:
            type = returnCode(CNEXT);
            break;
        case yield_code:
            type = yieldCode(CNEXT);
            break;
        case raise_code:
            type = raiseCode(CNEXT);
            break;
        case include_file:
            type = includeFile(CNEXT);
            run_gc = true;
            break;
        case import_file:
            type = importFile(CNEXT);
            run_gc = true;
            break;
        case from_import_file:
            type = fromImportFile(CNEXT);
            run_gc = true;
            break;
        case default_var:
            type = setDefault(CNEXT);
            break;
        case assert_:
            type = assertCode(CNEXT);
            break;
        case goto_:
            type = gotoLabel(CNEXT);
            break;
        case del_:
            type = delOperation(CNEXT);
            break;
        default:
            setResult(result, inter);
            break;
    }

    if (RUN_TYPE(type) && st->aut != auto_aut)
        result->value->aut = st->aut;  // 权限覆盖
    result->node = st;
    gc_freeTmpLink(&belong->gc_status);
#if START_GC
    if (run_gc && SHOULD_RUNGC(inter))
        gc_run(inter, var_list);
#endif
    return type;
}

ResultType runStatementOpt(bool run_gc, FUNC) {  // 不运行gc机制
    ResultType type = R_not;
    setResultCore(result);
    gc_addTmpLink(&belong->gc_status);

    switch (st->type) {
        case base_value:
            type = getBaseValue(CNEXT);
            break;
        case base_var:
            type = getVar(CNEXT, getBaseVarInfo);
            break;
        case base_svar:
            type = getVar(CNEXT, getBaseSVarInfo);
            break;
        case base_list:
            type = getList(CNEXT);
            break;
        case base_dict:
            type = getDict(CNEXT);
            break;
        case base_lambda:
            type = setLambda(CNEXT);
            break;
        case operation:
            type = operationStatement(CNEXT);
            break;
        case slice_:
            type = elementSlice(CNEXT);
            break;
        case call_function:
            type = callBack(CNEXT);
            break;
        default:
            setResult(result, inter);
            err_asert(runStatementOpt default);
            break;
    }

    if (RUN_TYPE(type) && st->aut != auto_aut)
        result->value->aut = st->aut;  // 权限覆盖
    result->node = st;
#if START_GC
    if (run_gc && SHOULD_RUNGC(inter))
        gc_run(inter, var_list);
#endif
    gc_freeTmpLink(&belong->gc_status);
    return type;
}

void signalDefault(vsignal sig_num, fline line, char *file, FUNC_NT) {
    switch (sig_num) {  // 信号的默认处理 (使用switch-case分支)
        case SIGINT:
            setResultError(E_KeyInterrupt, KEY_INTERRUPT, line, file, true, CNEXT_NT);
            break;
        default:
            break;
    }
}

static bool checkSignal(fline line, char *file, FUNC_NT) {  // 检查信号
    if (signal_tag.status == signal_appear){  // 出现信号
        SignalList *sig_list;
        vsignal sig_num = signal_tag.signum;
        Result bak = *result;  // 备份result
        setResultCore(result);  // 重置result
        signal_tag.status = signal_reset;  // 复位
        if ((sig_list = checkSignalList(sig_num, inter->sig_list)) == NULL)
            signalDefault(sig_num, line, file, CNEXT_NT);
        else {
            Argument *arg;
            makeIntValue(sig_num, LINEFILE, CNEXT_NT);
            arg = makeValueArgument(result->value);
            freeResult(result);
            callBackCore(sig_list->value, arg, line, file, 0, CNEXT_NT);  // 运行指定函数
            freeArgument(arg, true);
        }
        if (CHECK_RESULT(result)) {
            freeResult(result);
            *result = bak;  // 恢复result
            return false;
        } else {
            freeResult(&bak);  // 释放原有result
            return true;
        }
    }
    return false;  // 返回false表示未遇到error
}

static bool gotoStatement(Statement **next, FUNC) {
    Statement *label_st = checkLabel(st, result->label);

    if (label_st == NULL){
        setResultErrorSt(E_GotoException, L"Don't find label", true, st, CNEXT_NT);
        return false;
    }

    runLabel(CFUNC(label_st, var_list, result, belong));
    if (!CHECK_RESULT(result))
        return false;
    *next = label_st->next;
    return true;
}

/**
 * 局部程序运行statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */

#define RUN_CHECK_SIGNAL if (checkSignal(base->line, base->code_file, CNEXT_NT)) { \
type = result->type; \
break; \
}

ResultType iterStatement(FUNC) {
    Statement *base;
    ResultType type;

    setResultCore(result);
    if (st == NULL){
        setResult(result, inter);
        return result->type;
    }

    gc_addTmpLink(&belong->gc_status);
    do {
        base = st;
        while (base != NULL) {
            freeResult(result);
            type = runStatement(CFUNC(base, var_list, result, belong));
            RUN_CHECK_SIGNAL;
            if (type == R_goto && result->times == 0){
                if (!gotoStatement(&base, CNEXT)) {
                    type = result->type;
                    break;
                }
            }
            else if (!RUN_TYPE(type))
                break;
            else
                base = base->next;
        }
    } while (type == R_restart && result->times == 0);

    if (type == R_not || type == R_restart)
        setResultOperationNone(result, inter, belong);
    result->node = base;

    gc_freeTmpLink(&belong->gc_status);
    return result->type;
}

/**
 * 全局程序运行statement
 * @param inter
 * @return
 */
ResultType globalIterStatement(Result *result, Inter *inter, Statement *st, bool p_clock) {
    ResultType type;
    VarList *var_list = NULL;
    Statement *base;
    LinkValue *belong = inter->base_belong;
    clock_t start = clock();
    clock_t stop;

    if (st == NULL){
        setResult(result, inter);
        return result->type;
    }

    gc_addTmpLink(&belong->gc_status);
    do {
        base = st;
        var_list = inter->var_list;
        while (base != NULL) {
            freeResult(result);
            type = runStatement(CFUNC(base, var_list, result, belong));
            RUN_CHECK_SIGNAL;
            if (type == R_goto){
                if (!gotoStatement(&base, CNEXT)) {
                    type = result->type;
                    break;
                }
            }
            else if (!RUN_TYPE(type))
                break;
            else
                base = base->next;
        }
    } while (type == R_restart && result->times == 0);
    stop = clock();

    if (type != R_error && type != R_func)
        setResultOperationNone(result, inter, belong);
    result->node = base;

    if (p_clock)
        printf("run times = %Lf sec\n", (double long)(stop - start) / CLOCKS_PER_SEC);
    gc_freeTmpLink(&belong->gc_status);
    return result->type;
}

#undef RUN_CHECK_SIGNAL

// 若需要中断执行, 则返回true
bool operationSafeInterStatement(FUNC){
    ResultType type;
    assert(st->next == NULL);  // opt 以单句形式存在
    type = runStatementOpt(true, CNEXT);
    if (RUN_TYPE(type))
        return false;
    assert(type == return_code || type == R_error);
    return true;
}

bool optSafeInterStatement(FUNC){
    ResultType type;
    assert(st->next == NULL);  // opt 以单句形式存在
    type = runStatementOpt(false, CNEXT);
    if (RUN_TYPE(type))
        return false;
    assert(type == return_code || type == R_error);
    return true;
}

bool ifBranchSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (RUN_TYPE(type))
        return false;
    if (type == R_rego){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (type == R_restart || type == R_goto)
        result->times--;
    return true;
}

bool cycleBranchSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (RUN_TYPE(type))
        return false;
    if (type == R_break || type == R_continue){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (type == R_restart || type == R_goto)
        result->times--;
    return true;
}

bool withBranchSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (RUN_TYPE(type))
        return false;
    if (type == R_restart || type == R_goto)
        result->times--;
    return true;
}

bool tryBranchSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (RUN_TYPE(type))
        return false;
    if (type == R_restart || type == R_goto)
        result->times--;
    return true;
}

bool functionSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (type == R_error || result->type == R_yield)
        return true;
    else if (type == R_func)
        result->type = R_opt;
    else
        result->type = R_not;
    return false;
}

bool includeSafeInterStatement(FUNC){
    iterStatement(CNEXT);
    return !CHECK_RESULT(result);
}

bool blockSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (type == R_error || type == R_yield)
        return true;
    result->type = R_opt;
    return false;
}

Statement *checkLabel(Statement *base, wchar_t *label){
    for (PASS; base != NULL; base = base->next)
        if (base->type == label_ && eqWide(base->u.label_.label, label))
            return base;
    return NULL;
}

bool is_quitExc(LinkValue *value, Inter *inter) {
    return value->value == inter->data.base_exc[E_QuitException]->value || checkAttribution(value->value, inter->data.base_exc[E_QuitException]->value);
}
