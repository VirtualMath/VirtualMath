#include "__run.h"
/**
 * operation.c中是用于数学计算的函数
 */

#define getresult(base, var, inter) do{ \
if (operationSafeInterStatement(&var, CALL_INTER_FUNCTIONSIG(st->u.operation.base, var_list))){ return var; }} while (0)

#define getresultFree(base, var, other, inter) do{ \
if (operationSafeInterStatement(&var, CALL_INTER_FUNCTIONSIG(st->u.operation.base, var_list))){ \
freeResult(&other); \
return var; \
} \
}while(0)
#define viewtype_core(a, b, valuetype_a, valuetype_a_b) a .value->value->type == valuetype_a && b.value->value->type == valuetype_a_b
#define viewtype(a, b, valuetype) viewtype_core(a, b, valuetype, valuetype)
#define operationValue(a, b, type, symbol) a.value->value->data.type symbol b.value->value->data.type
#define valueToResult(result, result_value, type, inter) result.value->value = make##type##Value(result_value, inter)
#define noneOperation(left, right, result, st) \
else if (left.value->value->type == none){ \
result = right; \
gcAddTmp(&result.value->gc_status); \
} \
else if (right.value->value->type == none){ \
result = left; \
gcAddTmp(&result.value->gc_status); \
} \
else{ \
setResultError(&result, inter, "TypeException", "Get Not Support Value", st, true); \
}PASS

Result addOperation(INTER_FUNCTIONSIG);
Result subOperation(INTER_FUNCTIONSIG);
Result mulOperation(INTER_FUNCTIONSIG);
Result divOperation(INTER_FUNCTIONSIG);
Result assOperation(INTER_FUNCTIONSIG);

/**
 * operation的整体操作
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
Result operationStatement(INTER_FUNCTIONSIG) {
    Result result;
    setResultCore(&result);
    switch (st->u.operation.OperationType) {
        case OPT_ADD:
            result = addOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case OPT_SUB:
            result = subOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case OPT_MUL:
            result = mulOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case OPT_DIV:
            result = divOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case OPT_ASS:
            result = assOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        default:
            setResult(&result, inter);
            break;
    }
    return result;
}

Result addOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultCore(&left);
    setResultCore(&right);
    setResultCore(&result);

    getresult(left, left, inter);
    getresultFree(right, right, left, inter);

    setResultOperationBase(&result, makeLinkValue(NULL, NULL, inter), inter);
    if (viewtype(left, right, number))
        valueToResult(result, (operationValue(left, right, num.num, +)), Number, inter);
    else if(viewtype(left, right, string)){
        char *new_string = memStrcat(left.value->value->data.str.str, right.value->value->data.str.str, false);
        valueToResult(result, new_string, String, inter);
        memFree(new_string);
    }
    noneOperation(left, right, result, st);
    freeResult(&left);
    freeResult(&right);
    return result;
}

Result subOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultCore(&left);
    setResultCore(&right);
    setResultCore(&result);

    getresult(left, left, inter);
    getresultFree(right, right, left, inter);

    setResultOperationBase(&result, makeLinkValue(NULL, NULL, inter), inter);
    if (viewtype(left, right, number))
        valueToResult(result, (operationValue(left, right, num.num, -)), Number, inter);

    noneOperation(left, right, result, st);
    freeResult(&left);
    freeResult(&right);
    return result;
}

Result mulOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultCore(&left);
    setResultCore(&right);
    setResultCore(&result);

    getresult(left, left, inter);
    getresultFree(right, right, left, inter);

    setResultOperationBase(&result, makeLinkValue(NULL, NULL, inter), inter);
    if (viewtype(left, right, number))
        valueToResult(result, (operationValue(left, right, num.num, *)), Number, inter);
    else if(viewtype_core(left, right, number, string)){
        Result tmp = left;
        left = right;
        right = tmp;
        goto mul_str;
    }
    else if(viewtype_core(left, right, string, number)){
        mul_str:
        {
            char *new_string = memStrcpySelf(left.value->value->data.str.str, right.value->value->data.num.num);
            valueToResult(result, new_string, String, inter);
            memFree(new_string);
        }
    }
    noneOperation(left, right, result, st);

    freeResult(&left);
    freeResult(&right);
    return result;
}

Result divOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultCore(&left);
    setResultCore(&right);
    setResultCore(&result);

    getresult(left, left, inter);
    getresultFree(right, right, left, inter);

    setResultOperationBase(&result, makeLinkValue(NULL, NULL, inter), inter);
    if (viewtype(left, right, number))
        valueToResult(result, (operationValue(left, right, num.num, /)), Number, inter);
    noneOperation(left, right, result, st);
    freeResult(&left);
    freeResult(&right);
    printf("result.tmp = %ld, %p\n", result.value->gc_status.tmp_link, result.value);
    return result;
}

Result assOperation(INTER_FUNCTIONSIG) {
    Result result;
    Result times;
    setResultCore(&result);
    setResultCore(&times);

    getresult(right, result, inter);
    times = assCore(st->u.operation.left, result.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (is_error(times)) {
        freeResult(&result);
        return times;
    }
    freeResult(&times);
    return result;
}

Result assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_CORE){
    Result result;
    Result tmp_result;
    setResultCore(&result);
    setResultCore(&tmp_result);

    int int_times;
    if (name->type == base_list && name->u.base_list.type == value_tuple){
        Parameter *pt = NULL;
        Argument *call = NULL;
        Statement *tmp_st = makeBaseLinkValueStatement(value, name->line, name->code_file);

        pt = makeArgsParameter(tmp_st);
        call = getArgument(pt, &tmp_result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(tmp_result)) {
            freeArgument(call, false);
            freeParameter(pt, true);
            return tmp_result;
        }

        freeResult(&tmp_result);
        tmp_result = setParameterCore(call, name->u.base_list.list, var_list, name, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(tmp_result))
            result = tmp_result;
        else{
            Argument *tmp = call;
            LinkValue *new_value = makeLinkValue(makeListValue(&tmp, inter, value_tuple), NULL, inter);
            freeResult(&tmp_result);
            setResultOperation(&result, new_value, inter);
        }
        freeArgument(call, false);
        freeParameter(pt, true);
    }
    else{
        char *str_name = NULL;

        tmp_result = getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list));
        if (!run_continue(tmp_result)) {
            memFree(str_name);
            return tmp_result;
        }
        addFromVarList(str_name, var_list, int_times, value, tmp_result.value);
        memFree(str_name);
        freeResult(&tmp_result);

        setResultCore(&result);
        result.type = operation_return;
        result.value = value;
        gcAddTmp(&result.value->gc_status);
    }
    return result;
}

Result getVar(INTER_FUNCTIONSIG, VarInfo var_info) {
    int int_times = 0;
    char *name = NULL;
    Result result;
    Result tmp;
    setResultCore(&result);
    setResultCore(&tmp);

    tmp = var_info(&name, &int_times, CALL_INTER_FUNCTIONSIG(st, var_list));
    if (!run_continue(tmp)) {
        memFree(name);
        return tmp;
    }
    freeResult(&tmp);

    setResultCore(&result);
    result.type = operation_return;
    result.value = findFromVarList(name, var_list, int_times, false);
    memFree(name);

    if (result.value == NULL){
        char *info = memStrcat("Name Not Found: ", st->u.base_var.name, false);
        setResultError(&result, inter, "NameException", info, st, true);
        memFree(info);
    }
    else
        gcAddTmp(&result.value->gc_status);

    return result;
}

Result getBaseValue(INTER_FUNCTIONSIG) {
    Result result;
    setResultCore(&result);
    if (st->u.base_value.type == link_value) {
        result.value = st->u.base_value.value;
    }
    else if (st->u.base_value.type == number_str){
        char *stop = NULL;
        result.value = makeLinkValue(makeNumberValue(strtol(st->u.base_value.str, &stop, 10), inter), NULL, inter);
    }
    else
        result.value = makeLinkValue(makeStringValue(st->u.base_value.str, inter), NULL, inter);
    result.type = operation_return;
    gcAddTmp(&result.value->gc_status);
    return result;
}

Result getList(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;
    Result result;
    setResultCore(&result);

    at = getArgument(st->u.base_list.list, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result;
    }

    LinkValue *value = makeLinkValue(makeListValue(&at, inter, st->u.base_list.type), NULL, inter);
    setResultOperation(&result, value, inter);
    freeArgument(at_tmp, false);

    return result;
}

Result getDict(INTER_FUNCTIONSIG) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;
    Result result;
    setResultCore(&result);

    at = getArgument(st->u.base_dict.dict, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, false);
        return result;
    }

    Value *tmp_value = makeDictValue(&at, true, &result, inter, var_list);
    if (!run_continue(result)) {
        freeArgument(at_tmp, false);
        return result;
    }
    freeResult(&result);
    LinkValue *value = makeLinkValue(tmp_value, NULL, inter);
    setResultOperation(&result, value, inter);
    freeArgument(at_tmp, false);

    return result;
}
