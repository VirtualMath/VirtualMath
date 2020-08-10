#include "__run.h"

enum ResultType addOperation(INTER_FUNCTIONSIG);
enum ResultType subOperation(INTER_FUNCTIONSIG);
enum ResultType mulOperation(INTER_FUNCTIONSIG);
enum ResultType divOperation(INTER_FUNCTIONSIG);
enum ResultType assOperation(INTER_FUNCTIONSIG);

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
            addOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result));
            break;
        case OPT_SUB:
            subOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result));
            break;
        case OPT_MUL:
            mulOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result));
            break;
        case OPT_DIV:
            divOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result));
            break;
        case OPT_ASS:
            assOperation(CALL_INTER_FUNCTIONSIG(st, var_list, result));
            break;
        default:
            setResult(result, inter);
            break;
    }
    return result->type;
}

bool getLeftRightValue(Result *left, Result *right, INTER_FUNCTIONSIG){
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left, var_list, result)) || result->value->value->type == none)
        return true;
    *left = *result;
    setResultCore(result);

    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result)) || result->value->value->type == none)
        return true;
    *right = *result;
    return false;
}

ResultType addOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, NULL, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num + right.value->value->data.num.num, inter);
    else if(left.value->value->type == string && right.value->value->type == string){
        char *new_string = memStrcat(left.value->value->data.str.str, right.value->value->data.str.str, false);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType subOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, NULL, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num - right.value->value->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType mulOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, NULL, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num * right.value->value->data.num.num, inter);
    else if(left.value->value->type == number && right.value->value->type == string){
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
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType divOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);

    if (getLeftRightValue(&left, &right, CALL_INTER_FUNCTIONSIG(st, var_list, result)))
        return result->type;

    setResultOperationBase(result, makeLinkValue(NULL, NULL, inter), inter);
    if (left.value->value->type == number && right.value->value->type == number)
        result->value->value = makeNumberValue(left.value->value->data.num.num / right.value->value->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", st, true);

    freeResult(&left);
    freeResult(&right);
    return result->type;
}

ResultType assOperation(INTER_FUNCTIONSIG) {
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.right, var_list, result)))
        return result->type;
    assCore(st->u.operation.left, result->value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result));

    return result->type;
}

ResultType assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST){
    int int_times;
    setResultCore(result);
    gcAddTmp(&value->gc_status);

    if (name->type == base_list && name->u.base_list.type == value_tuple){
        Parameter *pt = NULL;
        Argument *call = NULL;
        Statement *tmp_st = makeBaseLinkValueStatement(value, name->line, name->code_file);

        pt = makeArgsParameter(tmp_st);
        call = getArgument(pt, result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(result)) {
            freeArgument(call, false);
            freeParameter(pt, true);
            goto return_;
        }

        freeResult(result);
         setParameterCore(call, name->u.base_list.list, var_list, name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result));
        if (run_continue(result)){
            Argument *tmp = call;
            LinkValue *new_value = makeLinkValue(makeListValue(&tmp, inter, value_tuple), NULL, inter);
            freeResult(result);
            setResultOperation(result, new_value, inter);
        }
        freeArgument(call, false);
        freeParameter(pt, true);
    }
    else{
        char *str_name = NULL;

        getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result));
        if (!run_continue(result)) {
            memFree(str_name);
            return result->type;
        }
        addFromVarList(str_name, var_list, int_times, value, result->value);
        memFree(str_name);
        freeResult(result);

        result->type = operation_return;
        result->value = value;
        gcAddTmp(&result->value->gc_status);
    }

    return_:
    gcFreeTmpLink(&value->gc_status);
    return result->type;
}

ResultType getVar(INTER_FUNCTIONSIG, VarInfo var_info) {
    int int_times = 0;
    char *name = NULL;

    freeResult(result);
    var_info(&name, &int_times, CALL_INTER_FUNCTIONSIG(st, var_list, result));
    if (!run_continue(result)) {
        memFree(name);
        return result->type;
    }

    freeResult(result);
    result->type = operation_return;
    result->value = findFromVarList(name, var_list, int_times, false);

    if (result->value == NULL){
        char *info = memStrcat("Name Not Found: ", name, false);
        setResultError(result, inter, "NameException", info, st, true);
        memFree(info);
    }
    else
        gcAddTmp(&result->value->gc_status);

    memFree(name);
    return result->type;
}

ResultType getBaseValue(INTER_FUNCTIONSIG) {
    freeResult(result);
    if (st->u.base_value.type == link_value) {
        result->value = st->u.base_value.value;
    }
    else if (st->u.base_value.type == number_str){
        char *stop = NULL;
        result->value = makeLinkValue(makeNumberValue(strtol(st->u.base_value.str, &stop, 10), inter), NULL, inter);
    }
    else
        result->value = makeLinkValue(makeStringValue(st->u.base_value.str, inter), NULL, inter);

    result->type = operation_return;
    gcAddTmp(&result->value->gc_status);
    return result->type;
}

ResultType getList(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    freeResult(result);
    at = getArgument(st->u.base_list.list, result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result->type;
    }

    LinkValue *value = makeLinkValue(makeListValue(&at, inter, st->u.base_list.type), NULL, inter);
    setResultOperation(result, value, inter);
    freeArgument(at_tmp, false);

    return result->type;
}

// TODO-szh 设置字典key为变量时直接取值
ResultType getDict(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    freeResult(result);
    at = getArgument(st->u.base_dict.dict, result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, false);
        return result->type;
    }

    freeResult(result);
    Value *tmp_value = makeDictValue(&at, true, result, inter, var_list);
    if (!run_continue(result)) {
        freeArgument(at_tmp, false);
        return result->type;
    }

    freeResult(result);
    LinkValue *value = makeLinkValue(tmp_value, NULL, inter);
    setResultOperation(result, value, inter);
    freeArgument(at_tmp, false);

    return result->type;
}
