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
            printf("TAG A\n");
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
            break;
        case import_file:
            type = importFile(CNEXT);
            break;
        case from_import_file:
            type = fromImportFile(CNEXT);
            break;
        case default_var:
            type = setDefault(CNEXT);
            break;
        case assert:
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

    if (RUN_TYPE(type) && result->value->aut == auto_aut)
        result->value->aut = st->aut;
    result->node = st;
#if START_GC
    gc_freeTmpLink(&belong->gc_status);
    gc_run(inter, var_list, 1, 2, 0, var_list, belong, result->value);
#endif
    return type;
}

static bool checkSignal(fline line, char *file, FUNC_NT) {
    if (is_KeyInterrupt == signal_appear){
        is_KeyInterrupt = signal_reset;
        setResultError(E_KeyInterrupt, KEY_INTERRUPT, line, file, true, CNEXT_NT);
        return true;
    }
    return false;
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
ResultType iterStatement(FUNC) {
    Statement *base;
    ResultType type;
    void *bak = NULL;

    setResultCore(result);
    if (st == NULL){
        setResult(result, inter);
        return result->type;
    }

    is_KeyInterrupt = signal_reset;
    bak = signal(SIGINT, signalStopInter);
    gc_addTmpLink(&belong->gc_status);
    do {
        base = st;
        if (checkSignal(base->line, base->code_file, CNEXT_NT)) {
            type = result->type;
            break;
        }
        while (base != NULL) {
            freeResult(result);
            type = runStatement(CFUNC(base, var_list, result, belong));
            if (checkSignal(base->line, base->code_file, CNEXT_NT)) {
                type = result->type;
                break;
            }
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

#if START_GC
    gc_freeTmpLink(&belong->gc_status);
    gc_run(inter, var_list, 1, 2, 0, var_list, belong, result->value);
#endif
    signal(SIGINT, bak);
    return result->type;
}

/**
 * 全局程序运行statement
 * @param inter
 * @return
 */
ResultType globalIterStatement(Result *result, Inter *inter, Statement *st) {
    ResultType type;
    VarList *var_list = NULL;
    Statement *base;
    LinkValue *belong = inter->base_belong;
    void *bak = NULL;

    if (st == NULL){
        setResult(result, inter);
        return result->type;
    }

    is_KeyInterrupt = signal_reset;
    bak = signal(SIGINT, signalStopInter);
    gc_addTmpLink(&belong->gc_status);
    do {
        base = st;
        var_list = inter->var_list;
        if (checkSignal(base->line, base->code_file, CNEXT_NT)) {
            type = result->type;
            break;
        }
        while (base != NULL) {
            freeResult(result);
            type = runStatement(CFUNC(base, var_list, result, belong));
            if (checkSignal(base->line, base->code_file, CNEXT_NT)) {
                type = result->type;
                break;
            }
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

    if (type != R_error && type != R_func)
        setResultOperationNone(result, inter, belong);
    result->node = base;

#if START_GC
    gc_freeTmpLink(&belong->gc_status);
    gc_run(inter, var_list, 1, 2, 0, var_list, belong, result->value);
#endif
    signal(SIGINT, bak);
    return result->type;
}

// 若需要中断执行, 则返回true
bool operationSafeInterStatement(FUNC){
    ResultType type;
    type = iterStatement(CNEXT);
    if (RUN_TYPE(type))
        return false;
    else if (type != return_code && type != R_error)
        setResultErrorSt(E_ResultException, L"Operation get not support result type", true, st, CNEXT_NT);
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
    return value->value == inter->data.quit_exc->value || checkAttribution(value->value, inter->data.quit_exc->value);
}
