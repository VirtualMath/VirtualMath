#include "__run.h"

/**
 * 运行单个statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
ResultType runStatement(INTER_FUNCTIONSIG) {
    setResultCore(result);
    ResultType type = not_return;
    switch (st->type) {
        case base_value:
            type = getBaseValue(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case base_var:
            type = getVar(CALL_INTER_FUNCTIONSIG(st, var_list, result, father), getBaseVarInfo);
            break;
        case base_svar:
            type = getVar(CALL_INTER_FUNCTIONSIG(st, var_list, result, father), getBaseSVarInfo);
            break;
        case base_list:
            type = getList(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case base_dict:
            type = getDict(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case base_lambda:
            type = setLambda(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case operation:
            type = operationStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            if (run_continue_type(type))
                printLinkValue(result->value, "operation result = ", "\n", inter->data.debug);
            break;
        case set_class:
            type = setClass(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case set_function:
            type = setFunction(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case call_function:
            type = callBack(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case if_branch:
            type = ifBranch(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case while_branch:
            type = whileBranch(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case with_branch:
            type = withBranch(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case try_branch:
            type = tryBranch(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case break_cycle:
            type = breakCycle(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case continue_cycle:
            type = continueCycle(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case rego_if:
            type = regoIf(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case restart:
            type = restartCode(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case return_code:
            type = returnCode(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case raise_code:
            type = raiseCode(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case include_file:
            type = includeFile(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case import_file:
            type = importFile(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case from_import_file:
            type = fromImportFile(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case default_var:
            type = setDefault(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case assert:
            type = assertCode(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case goto_:
            type = gotoLabel(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        default:
            setResult(result, inter, father);
            break;
    }

    if (run_continue_type(type) && result->value->aut == auto_aut)
        result->value->aut = st->aut;
    gc_run(inter, 1, 0, 0, var_list);
    return type;
}

/**
 * 局部程序运行statement
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
ResultType iterStatement(INTER_FUNCTIONSIG) {
    Statement *base_st = NULL;
    ResultType type;
    setResultCore(result);

    if (st == NULL){
        setResult(result, inter, father);
        return result->type;
    }

    do {
        for (base_st = st; base_st != NULL; PASS) {
            freeResult(result);
            type = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list, result, father));
            if (type == goto_return && result->times == 0){
                Statement *label_st = checkLabel(st, result->label);
                if (label_st == NULL){
                    setResultError(result, inter, "GotoException", "Don't find label", st, father, true);
                    type = error_return;
                    break;
                }
                type = runLabel(CALL_INTER_FUNCTIONSIG(label_st, var_list, result, father));
                if (!run_continue_type(type))
                    break;
                base_st = label_st->next;
            }
            else if (!run_continue_type(type))
                break;
            else
                base_st = base_st->next;
        }
    } while (type == restart_return && result->times == 0);

    if (type == not_return || type == restart_return)
        setResultOperationNone(result, inter, father);

    gc_run(inter, 1, 0, 0, var_list);
    return result->type;
}

/**
 * 全局程序运行statement
 * @param inter
 * @return
 */
ResultType globalIterStatement(Result *result, LinkValue *base_father, Inter *inter, Statement *st) {
    LinkValue *father = makeLinkValue(makeObject(inter, copyVarList(inter->var_list, false, inter), NULL, NULL), base_father, inter);
    Statement *base_st = NULL;
    VarList *var_list = NULL;
    enum ResultType type;
    do {
        for (base_st = st, var_list = inter->var_list; base_st != NULL; PASS) {
            freeResult(result);
            type = runStatement(CALL_INTER_FUNCTIONSIG(base_st, var_list, result, father));
            if (type == goto_return){
                Statement *label_st = checkLabel(st, result->label);
                if (label_st == NULL){
                    setResultError(result, inter, "GotoException", "Don't find label", st, father, true);
                    type = error_return;
                    break;
                }
                type = runLabel(CALL_INTER_FUNCTIONSIG(label_st, var_list, result, father));
                if (!run_continue_type(type))
                    break;
                base_st = label_st->next;
            }
            else if (!run_continue_type(type))
                break;
            else
                base_st = base_st->next;
        }
    } while (type == restart_return && result->times == 0);

    if (type != error_return && type != function_return)
        setResultOperationNone(result, inter, father);

    gc_run(inter, 1, 0, 0, var_list);
    return result->type;
}

// 若需要中断执行, 则返回true
bool operationSafeInterStatement(INTER_FUNCTIONSIG){
    ResultType type;
    type = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (run_continue_type(type))
        return false;
    return true;
}

bool ifBranchSafeInterStatement(INTER_FUNCTIONSIG){
    ResultType type;
    type = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (run_continue_type(type))
        return false;
    if (type == rego_return){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (type == restart_return || type == goto_return)
        result->times--;
    return true;
}

bool cycleBranchSafeInterStatement(INTER_FUNCTIONSIG){
    ResultType type;
    type = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (run_continue_type(type)){
        return false;
    }
    if (type == break_return || type == continue_return){
        result->times--;
        if (result->times < 0)
            return false;
    }
    if (type == restart_return || type == goto_return)
        result->times--;
    return true;
}

bool tryBranchSafeInterStatement(INTER_FUNCTIONSIG){
    ResultType type;
    type = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (run_continue_type(type)){
        return false;
    }
    if (type == restart_return || type == goto_return)
        result->times--;
    return true;
}

bool functionSafeInterStatement(INTER_FUNCTIONSIG){
    ResultType type;
    type = iterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (type == error_return)
        return true;
    else if (type == function_return){
        result->type = operation_return;
        return true;
    }
    result->type = not_return;
    return false;
}

Statement *checkLabel(Statement *base, char *label){
    for (PASS; base != NULL; base = base->next)
        if (base->type == label_ && eqString(base->u.label_.label, label))
            return base;
    return NULL;
}
