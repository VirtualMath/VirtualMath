#include "__run.h"

ResultType addOperation(INTER_FUNCTIONSIG);
ResultType subOperation(INTER_FUNCTIONSIG);
ResultType mulOperation(INTER_FUNCTIONSIG);
ResultType divOperation(INTER_FUNCTIONSIG);
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
            addOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_SUB:
            subOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_MUL:
            mulOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_DIV:
            divOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_ASS:
            assOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_POINT:
            pointOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        case OPT_BLOCK:
            blockOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
            break;
        default:
            setResult(result, inter, father);
            break;
    }
    return result->type;
}

bool getLeftRightValue(Result *left, Result *right, INTER_FUNCTIONSIG){
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result, father)) || result->value->value->type == none)
        return true;
    *left = *result;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result, father)) || result->value->value->type == none)
        return true;
    *right = *result;
    return false;
}

ResultType addOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, father, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num + right.value->value->data.num.num, inter);
    else if(left.value->value->type == string && right.value->value->type == string){
        char *new_string = memStrcat(left.value->value->data.str.str, right.value->value->data.str.str, false, false);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, father, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType subOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, father, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num - right.value->value->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, father, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType mulOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, father, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num * right.value->value->data.num.num, inter);
    else if(left.value->value->type == number && right.value->value->type == string) {
        Result tmp = left;
        left = right;
        right = tmp;
        goto mul_str;
    }
    else if(left.value->value->type == string && right.value->value->type == number) mul_str: {
        char *new_string = memStrcpySelf(left.value->value->data.str.str, right.value->value->data.num.num);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, father, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType divOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, father, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num / right.value->value->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, father, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType blockOperation(INTER_FUNCTIONSIG) {
    ResultType type;
    var_list = pushVarList(var_list, inter);
    type = operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result, father));
    if (run_continue_type(type) && st->aut != auto_aut)
        result->value->aut = st->aut;
    popVarList(var_list);
    return type;
}

ResultType pointOperation(INTER_FUNCTIONSIG) {
    LinkValue *left;

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result, father)) || result->value->value->type == none)
        return result->type;

    left = result->value;
    setResultCore(result);
    VarList *object = left->value->object.var;
    VarList *out_var = NULL;
    for (out_var = object; out_var->next != NULL; out_var = out_var->next)
        PASS;
    out_var->next = left->value->object.out_var;

    gc_freeze(inter, var_list, object, true);
    operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, object, result, left));
    if (!run_continue(result))
        goto return_;
    else if ((left->aut == public_aut || left->aut == auto_aut) && (result->value->aut != public_aut && result->value->aut != auto_aut))
        setResultError(result, inter, "PermissionsException", "Wrong Permissions: access variables as public", st, father, true);
    else if ((left->aut == protect_aut) && (result->value->aut == private_aut))
        setResultError(result, inter, "PermissionsException", "Wrong Permissions: access variables as protect", st, father, true);

    if (result->value->father->value != left->value && checkAttribution(left->value, result->value->father->value)) {
        /**
         * 若获得的值的father是left的father, 则将获得值的father调整为left (拷贝)
         */
        LinkValue *return_value = copyLinkValue(result->value, inter);
        return_value->father = left;
        gc_freeTmpLink(&result->value->gc_status);
        gc_addTmpLink(&return_value->gc_status);
        result->value = return_value;
    }
    return_:
    gc_freeze(inter, var_list, object, false);
    if (out_var != NULL)
        out_var->next = NULL;
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}

ResultType assOperation(INTER_FUNCTIONSIG) {
    LinkValue *value = NULL;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result, father)))
        return result->type;
    value = result->value;

    freeResult(result);
    assCore(st->u.operation.left, value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    return result->type;
}

ResultType assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST){
    int int_times;
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);

    if (name->type == base_list && name->u.base_list.type == value_tuple){
        Parameter *pt = NULL;
        Argument *call = NULL;
        Statement *tmp_st = makeBaseLinkValueStatement(value, name->line, name->code_file);

        pt = makeArgsParameter(tmp_st);
        call = getArgument(pt, false, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
        if (!run_continue(result)) {
            freeArgument(call, false);
            freeParameter(pt, true);
            goto return_;
        }

        freeResult(result);
        setParameterCore(call, name->u.base_list.list, var_list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        if (run_continue(result)){
            Argument *tmp = call;
            LinkValue *new_value = makeLinkValue(makeListValue(&tmp, inter, value_tuple), father, inter);
            freeResult(result);
            setResultOperation(result, new_value, inter);
        }
        freeArgument(call, false);
        freeParameter(pt, true);
    }
    else if (name->type == operation && name->u.operation.OperationType == OPT_POINT)
        pointAss(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    else{
        char *str_name = NULL;
        getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result, father));
        if (!run_continue(result)) {
            memFree(str_name);
            return result->type;
        }
        LinkValue *var_value = copyLinkValue(value, inter);
        if (var_value->aut == auto_aut)
            var_value->aut = name->aut;
        addFromVarList(str_name, result->value, int_times, var_value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        memFree(str_name);
        freeResult(result);

        result->type = operation_return;
        result->value = value;
        gc_addTmpLink(&result->value->gc_status);
    }

    return_:
    gc_freeTmpLink(&value->gc_status);
    return result->type;
}

ResultType pointAss(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST) {
    Result left;
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(name->u.operation.left, var_list, result, father)))
        return result->type;
    left = *result;
    setResultCore(result);

    VarList *object = left.value->value->object.var;
    gc_freeze(inter, var_list, object, true);
    if (name->u.operation.right->type == OPERATION && name->u.operation.right->u.operation.OperationType == OPT_POINT)
        pointAss(name->u.operation.right, value, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, father));
    else
        assCore(name->u.operation.right, value, CALL_INTER_FUNCTIONSIG_NOT_ST(object, result, father));
    gc_freeze(inter, var_list, object, true);

    freeResult(&left);
    return result->type;
}

ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info) {
    int int_times = 0;
    char *name = NULL;

    freeResult(result);
    var_info(&name, &int_times, CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    if (!run_continue(result)) {
        memFree(name);
        return result->type;
    }

    freeResult(result);
    result->type = operation_return;
    result->value = findFromVarList(name, int_times, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (result->value == NULL) {
        char *info = memStrcat("Name Not Found: ", name, false, false);
        setResultError(result, inter, "NameException", info, st, father, true);
        memFree(info);
    }
    else if ((st->aut == public_aut) && (result->value->aut != public_aut && result->value->aut != auto_aut)){
        setResultCore(result);
        char *info = memStrcat("Wrong Permissions: access variables as public ", name, false, false);
        setResultError(result, inter, "PermissionsException", info, st, father, true);
        memFree(info);
    }
    else if ((st->aut == protect_aut) && (result->value->aut == private_aut)){
        setResultCore(result);
        char *info = memStrcat("Wrong Permissions: access variables as protect ", name, false, false);
        setResultError(result, inter, "PermissionsException", info, st, father, true);
        memFree(info);
    }
    else
        setResultOperationBase(result, result->value, inter);

    memFree(name);
    return result->type;
}

ResultType getBaseValue(INTER_FUNCTIONSIG) {
    setResultCore(result);
    if (st->u.base_value.type == link_value)
        result->value = st->u.base_value.value;
    else if (st->u.base_value.type == number_str){
        char *stop = NULL;
        result->value = makeLinkValue(makeNumberValue(strtol(st->u.base_value.str, &stop, 10), inter), father, inter);
    }
    else
        result->value = makeLinkValue(makeStringValue(st->u.base_value.str, inter), father, inter);

    result->type = operation_return;
    gc_addTmpLink(&result->value->gc_status);
    return result->type;
}

ResultType getList(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_list.list, false, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result->type;
    }

    LinkValue *value = makeLinkValue(makeListValue(&at, inter, st->u.base_list.type), father, inter);
    setResultOperation(result, value, inter);
    freeArgument(at_tmp, false);

    return result->type;
}

ResultType getDict(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_dict.dict, true, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, false);
        return result->type;
    }

    freeResult(result);
    Value *tmp_value = makeDictValue(&at, true, father, result, inter, var_list);
    if (!run_continue(result)) {
        freeArgument(at_tmp, false);
        return result->type;
    }

    freeResult(result);
    LinkValue *value = makeLinkValue(tmp_value, father, inter);
    setResultOperation(result, value, inter);
    freeArgument(at_tmp, false);

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
        getVarInfo(&name, &times, CALL_INTER_FUNCTIONSIG(pt->data.value, var_list, result, father));
        if (!run_continue(result))
            break;
        if (type != default_)
            times = base;
        var_list->default_var = connectDefaultVar(var_list->default_var, name, times);
        memFree(name);
    }
    return result->type;
}
