#include "__run.h"

static bool getLeftRightValue(Result *left, Result *right, INTER_FUNCTIONSIG);
static ResultType operationCore(INTER_FUNCTIONSIG, char *name);
ResultType assOperation(INTER_FUNCTIONSIG);
ResultType pointOperation(INTER_FUNCTIONSIG);
ResultType blockOperation(INTER_FUNCTIONSIG);

/**
 * operation的整体操作
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
ResultType operationStatement(INTER_FUNCTIONSIG) {
    setResultCore(result);
    switch (st->u.operation.OperationType) {
        case OPT_ADD:
            operationCore(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong), inter->data.object_add);
            break;
        case OPT_SUB:
            operationCore(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong), inter->data.object_sub);
            break;
        case OPT_MUL:
            operationCore(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong), inter->data.object_mul);
            break;
        case OPT_DIV:
            operationCore(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong), inter->data.object_div);
            break;
        case OPT_ASS:
            assOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
            break;
        case OPT_LINK:
        case OPT_POINT:
            pointOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
            break;
        case OPT_BLOCK:
            blockOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
            break;
        default:
            setResult(result, inter, belong);
            break;
    }
    return result->type;
}

ResultType blockOperation(INTER_FUNCTIONSIG) {
    Statement *info_st = st->u.operation.left;
    bool yield_run;
    if ((yield_run = popStatementVarList(st, &var_list, var_list, inter)))
        info_st = st->info.node;
    blockSafeInterStatement(CALL_INTER_FUNCTIONSIG(info_st, var_list, result, belong));
    if (result->type == error_return)
        return result->type;
    else if (yield_run) {
        if (result->type == yield_return){
            updateFunctionYield(st, result->node);
            result->type = operation_return;
        }
        else
            freeRunInfo(st);
    }
    else {
        if (result->type == yield_return){
            newFunctionYield(st, result->node, var_list, inter);
            result->type = operation_return;
        }
        else
            popVarList(var_list);
    }
    if (CHECK_RESULT(result) && st->aut != auto_aut)
        result->value->aut = st->aut;
    return result->type;
}

ResultType pointOperation(INTER_FUNCTIONSIG) {
    LinkValue *left;
    VarList *object = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result, belong)) || result->value->value->type == none)
        return result->type;
    left = result->value;
    result->value = NULL;
    freeResult(result);

    if (st->u.operation.OperationType == OPT_POINT)
        object = left->value->object.var;
    else if (st->u.operation.OperationType == OPT_LINK)
        object = left->value->object.out_var;

    if (object == NULL) {
        setResultError(E_TypeException, "object not support . / ->", st->line, st->code_file, true,
                       CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }
    gc_freeze(inter, var_list, object, true);
    operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, object, result, left));
    if (!CHECK_RESULT(result) || !checkAut(left->aut, result->value->aut, st->line, st->code_file, NULL, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        PASS;
    else if (result->value->belong == NULL || result->value->belong->value != left->value && checkAttribution(left->value, result->value->belong->value))
        result->value->belong = left;
    gc_freeze(inter, var_list, object, false);

    return_:
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}

ResultType delOperation(INTER_FUNCTIONSIG) {
    Statement *var;
    setResultCore(result);
    var = st->u.del_.var;
    delCore(var, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType delCore(Statement *name, bool check_aut, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);
    if (name->type == base_list && name->u.base_list.type == value_tuple)
        listDel(name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else if (name->type == slice_)
        downDel(name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else if (name->type == operation && (name->u.operation.OperationType == OPT_POINT || name->u.operation.OperationType == OPT_LINK))
        pointDel(name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else
        varDel(name, check_aut, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType listDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);
    for (Parameter *pt = name->u.base_list.list; pt != NULL; pt = pt->next){
        delCore(pt->data.value, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        freeResult(result);
    }
    setResultBase(result, inter);
    return result->type;
}

ResultType varDel(Statement *name, bool check_aut, INTER_FUNCTIONSIG_NOT_ST) {
    char *str_name = NULL;
    int int_times = 0;
    setResultCore(result);
    getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        memFree(str_name);
        return result->type;
    }
    if (check_aut) {
        LinkValue *tmp = findFromVarList(str_name, int_times, read_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        freeResult(result);
        if (tmp != NULL && !checkAut(name->aut, tmp->aut, name->line, name->code_file, NULL, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
            goto return_;
        }
    }
    findFromVarList(str_name, int_times, del_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    setResult(result, inter, belong);
    return_:
    memFree(str_name);
    return result->type;
}

ResultType pointDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST) {
    Result left;
    VarList *object = NULL;
    Statement *right = name->u.operation.right;

    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(name->u.operation.left, var_list, result, belong)))
        return result->type;
    left = *result;
    setResultCore(result);

    object = name->u.operation.OperationType == OPT_POINT ? left.value->value->object.var : left.value->value->object.out_var;

    if (object == NULL) {
        setResultError(E_TypeException, "object not support . / ->", name->line, name->code_file, true,
                       CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }

    gc_freeze(inter, var_list, object, true);
    if (right->type == OPERATION && (right->u.operation.OperationType == OPT_POINT || right->u.operation.OperationType == OPT_LINK))
        pointDel(name->u.operation.right, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, belong));
    else
        delCore(name->u.operation.right, true, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, belong));
    gc_freeze(inter, var_list, object, false);

    return_:
    freeResult(&left);
    return result->type;
}

ResultType downDel(Statement *name, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *iter = NULL;
    LinkValue *_func_ = NULL;
    Parameter *pt = name->u.slice_.index;

    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(name->u.slice_.element, var_list, result, belong)))
        return result->type;
    iter = result->value;
    result->value = NULL;
    freeResult(result);
    if (name->u.slice_.type == SliceType_down_)
        _func_ = findAttributes(inter->data.object_down_del, false, iter, inter);
    else
        _func_ = findAttributes(inter->data.object_slice_del, false, iter, inter);
    if (_func_ != NULL){
        Argument *arg = NULL;
        gc_addTmpLink(&_func_->gc_status);
        arg = getArgument(pt, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto dderror_;
        freeResult(result);
        callBackCore(_func_, arg, name->line, name->code_file, 0,
                     CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

        dderror_:
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else
        setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(del(__down_del__/__slice_del__)), true, name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    gc_freeTmpLink(&iter->gc_status);
    return result->type;
}

ResultType assOperation(INTER_FUNCTIONSIG) {
    LinkValue *value = NULL;
    setResultCore(result);
    if (st->u.operation.left->type == call_function){
        Statement *return_st = makeReturnStatement(st->u.operation.right, st->line, st->code_file);
        LinkValue *func = NULL;
        makeVMFunctionValue(return_st, st->u.operation.left->u.call_function.parameter, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return_st->u.return_code.value = NULL;
        freeStatement(return_st);

        func = result->value;
        result->value = NULL;
        freeResult(result);
        assCore(st->u.operation.left->u.call_function.function, func, false, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&func->gc_status);
    }
    else{
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result, belong)))
            return result->type;
        value = result->value;

        freeResult(result);
        assCore(st->u.operation.left, value, false, false,
                CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    }
    return result->type;
}

ResultType assCore(Statement *name, LinkValue *value, bool check_aut, bool setting, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);

    if (name->type == base_list && name->u.base_list.type == value_tuple)
        listAss(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else if (name->type == slice_)
        downAss(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else if (name->type == operation && (name->u.operation.OperationType == OPT_POINT || name->u.operation.OperationType == OPT_LINK))
        pointAss(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    else
        varAss(name, value, check_aut, setting, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    gc_freeTmpLink(&value->gc_status);
    return result->type;
}


ResultType varAss(Statement *name, LinkValue *value, bool check_aut, bool setting, INTER_FUNCTIONSIG_NOT_ST) {
    char *str_name = NULL;
    int int_times = 0;
    LinkValue *var_value = NULL;

    setResultCore(result);
    getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        memFree(str_name);
        return result->type;
    }
    var_value = copyLinkValue(value, inter);
    if (name->aut != auto_aut)
        var_value->aut = name->aut;
    if (check_aut) {
        LinkValue *tmp = findFromVarList(str_name, int_times, read_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (tmp != NULL && !checkAut(value->aut, tmp->aut, name->line, name->code_file, NULL, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
            goto error_;
    } else if (name->aut != auto_aut){
        LinkValue *tmp = findFromVarList(str_name, int_times, read_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (tmp != NULL)
            tmp->aut = name->aut;
    }
    addFromVarList(str_name, result->value, int_times, value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (setting) {
        freeResult(result);
        newObjectSetting(value, name->line, name->code_file, value, result, inter, var_list);
        if (CHECK_RESULT(result))
            goto error_;
    }

    freeResult(result);
    result->type = operation_return;
    result->value = value;
    gc_addTmpLink(&result->value->gc_status);

    error_:
    memFree(str_name);
    return result->type;
}

ResultType listAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST) {
    Parameter *pt = NULL;
    Argument *call = NULL;
    Statement *tmp_st = makeBaseLinkValueStatement(value, name->line, name->code_file);

    setResultCore(result);
    pt = makeArgsParameter(tmp_st);
    call = getArgument(pt, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    setParameterCore(name->line, name->code_file, call, name->u.base_list.list, var_list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (CHECK_RESULT(result)){
        freeResult(result);
        makeListValue(call, name->line, name->code_file, value_tuple, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    }
    return_:
    freeArgument(call, false);
    freeParameter(pt, true);
    return result->type;
}

ResultType downAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *iter = NULL;
    LinkValue *_func_ = NULL;
    Parameter *pt = name->u.slice_.index;

    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(name->u.slice_.element, var_list, result, belong)))
        return result->type;
    iter = result->value;
    result->value = NULL;
    freeResult(result);
    if (name->u.slice_.type == SliceType_down_)
        _func_ = findAttributes(inter->data.object_down_assignment, false, iter, inter);
    else
        _func_ = findAttributes(inter->data.object_slice_assignment, false, iter, inter);
    if (_func_ != NULL){
        Argument *arg = makeValueArgument(value);
        gc_addTmpLink(&_func_->gc_status);
        arg->next = getArgument(pt, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto daerror_;

        freeResult(result);
        callBackCore(_func_, arg, name->line, name->code_file, 0,
                     CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

        daerror_:
        freeArgument(arg, true);
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(assignment(__down_assignment__/__slice_assignment__)), true, name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    gc_freeTmpLink(&iter->gc_status);
    return result->type;
}

ResultType pointAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST) {
    Result left;
    Statement *right = name->u.operation.right;
    VarList *object = NULL;

    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(name->u.operation.left, var_list, result, belong)))
        return result->type;
    left = *result;
    setResultCore(result);

    object = name->u.operation.OperationType == OPT_POINT ? left.value->value->object.var : left.value->value->object.out_var;

    if (object == NULL) {
        setResultError(E_TypeException, "object not support . / ->", name->line, name->code_file, true,
                       CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        goto return_;
    }

    gc_freeze(inter, var_list, object, true);
    if (right->type == OPERATION && (right->u.operation.OperationType == OPT_POINT || right->u.operation.OperationType == OPT_LINK))
        pointAss(name->u.operation.right, value, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, belong));
    else
        assCore(name->u.operation.right, value, true, false, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, belong));
    gc_freeze(inter, var_list, object, false);

    return_:
    freeResult(&left);
    return result->type;
}

ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info) {
    int int_times = 0;
    char *name = NULL;
    LinkValue *var;

    setResultCore(result);
    var_info(&name, &int_times, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        memFree(name);
        return result->type;
    }

    freeResult(result);
    var = findFromVarList(name, int_times, get_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (var == NULL) {
        char *info = memStrcat("Variable not found: ", name, false, false);
        setResultErrorSt(E_NameExceptiom, info, true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        memFree(info);
    }
    else if (checkAut(st->aut, var->aut, st->line, st->code_file, NULL, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)))
        setResultOperationBase(result, var);
    memFree(name);
    return result->type;
}

ResultType getBaseValue(INTER_FUNCTIONSIG) {
    setResultCore(result);
    if (st->u.base_value.type == link_value) {
        result->value = st->u.base_value.value;
        result->type = operation_return;
        gc_addTmpLink(&result->value->gc_status);
    }
    else
        switch (st->u.base_value.type){
            case number_str:
                makeNumberValue(strtol(st->u.base_value.str, NULL, 10), st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                break;
            case bool_true:
                makeBoolValue(true, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                break;
            case bool_false:
                makeBoolValue(false, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                break;
            case pass_value:
                makePassValue(st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                break;
            case null_value:
                useNoneValue(inter, result);
                break;
            default:
                makeStringValue(st->u.base_value.str, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                break;
        }
    return result->type;
}

ResultType getList(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_list.list, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    at_tmp = at;
    if (!CHECK_RESULT(result)){
        freeArgument(at_tmp, false);
        return result->type;
    }
    freeResult(result);
    makeListValue(at, st->line, st->code_file, st->u.base_list.type, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    freeArgument(at_tmp, false);

    return result->type;
}

ResultType getDict(INTER_FUNCTIONSIG) {
    Argument *at = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_dict.dict, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result)){
        freeArgument(at, false);
        return result->type;
    }

    freeResult(result);
    Value *tmp_value = makeDictValue(at, true, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        freeArgument(at, false);
        return result->type;
    }

    freeResult(result);
    LinkValue *value = makeLinkValue(tmp_value, belong, inter);
    setResultOperation(result, value);
    freeArgument(at, false);

    return result->type;
}

ResultType setDefault(INTER_FUNCTIONSIG){
    enum DefaultType type = st->u.default_var.default_type;
    int base = 0;  // 用于nonlocal和global
    setResultCore(result);
    if (type == global_)
        for (VarList *tmp = var_list; tmp->next != NULL; tmp = tmp->next)
            base++;
    else if (type == nonlocal_)
        base = 1;
    for (Parameter *pt = st->u.default_var.var; pt != NULL; pt = pt->next){
        char *name = NULL;
        int times = 0;
        freeResult(result);
        getVarInfo(&name, &times, CALL_INTER_FUNCTIONSIG(pt->data.value, var_list, result, belong));
        if (!CHECK_RESULT(result))
            break;
        if (type != default_)
            times = base;
        var_list->default_var = connectDefaultVar(var_list->default_var, name, times);
        memFree(name);
    }
    return result->type;
}

bool getLeftRightValue(Result *left, Result *right, INTER_FUNCTIONSIG){
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result, belong)) || result->value->value->type == none)
        return true;
    *left = *result;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result, belong)) || result->value->value->type == none)
        return true;
    *right = *result;
    setResultCore(result);
    return false;
}

ResultType operationCore(INTER_FUNCTIONSIG, char *name) {
    Result left;
    Result right;
    LinkValue *_func_ = NULL;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
        return result->type;

    _func_ = findAttributes(name, false, left.value, inter);
    if (_func_ != NULL){
        Argument *arg = makeValueArgument(right.value);
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, arg, st->line, st->code_file, 0,
                     CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else {
        char *message = memStrcat("Object not support ", name, false, false);
        setResultErrorSt(E_TypeException, message, true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        memFree(message);
    }

    freeResult(&left);
    freeResult(&right);
    return result->type;
}
