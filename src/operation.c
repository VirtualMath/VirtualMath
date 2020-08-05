#include "__run.h"
/**
 * operation.c中是用于数学计算的函数
 */

#define getresult(base, var, inter) do{ \
if (operationSafeInterStatement(&var, CALL_INTER_FUNCTIONSIG(st->u.operation. base, var_list))){ return var; } \
}while(0)
#define viewtype_core(a, b, valuetype_a, valuetype_a_b) a .value->value->type == valuetype_a && b.value->value->type == valuetype_a_b
#define viewtype(a, b, valuetype) viewtype_core(a, b, valuetype, valuetype)
#define operationValue(a, b, type, symbol) a.value->value->data.type symbol b.value->value->data.type
#define valueToResult(result, result_value, type, inter) result.value->value = make##type##Value(result_value, inter)
#define noneOperation(left, right, result) do{ \
if (left.value->value->type == none){ \
result = right; \
} \
else if (right.value->value->type == none){ \
result = left; \
} \
} while(0)

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
    switch (st->u.operation.OperationType) {
        case ADD:
            result = addOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case SUB:
            result = subOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case MUL:
            result = mulOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case DIV:
            result = divOperation(CALL_INTER_FUNCTIONSIG(st, var_list));
            break;
        case ASS:
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
    setResultOperation(&result, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, +)), Number, inter);
    }
    else if(viewtype(left, right, string)){
        char *new_string = memStrcat(left.value->value->data.str.str, right.value->value->data.str.str, false);
        valueToResult(result, new_string, String, inter);
        memFree(new_string);
    }
    noneOperation(left, right, result);
    return result;
}

Result subOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultOperation(&result, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, -)), Number, inter);
    }
    noneOperation(left, right, result);
    return result;
}

Result mulOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultOperation(&result, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, *)), Number, inter);
    }
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
    noneOperation(left, right, result);
    return result;
}

Result divOperation(INTER_FUNCTIONSIG) {
    Result left;
    Result right;
    Result result;
    setResultOperation(&result, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, /)), Number, inter);
    }
    noneOperation(left, right, result);
    return result;
}

Result assOperation(INTER_FUNCTIONSIG) {
    Result result;
    Result times;
    getresult(right, result, inter);
    times = assCore(st->u.operation.left, result.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    checkResult(times);
    return result;
}

Result assCore(Statement *name, LinkValue *value, INTER_FUNCTIONSIG_CORE){
    Result result;
    setResultOperation(&result, inter);
    int int_times;
    if (name->type == base_list && name->u.base_list.type == value_tuple){
        Result tmp_result;
        Statement *tmp_st = makeStatement(name->line, name->code_file);
        tmp_st->type = base_value;
        tmp_st->u.base_value.value = value;
        Parameter *pt = makeArgsParameter(tmp_st);
        Argument *call = getArgument(pt, &tmp_result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(tmp_result)) {
            freeArgument(call, false);
            freeParameter(pt, true);
            return tmp_result;
        }
        tmp_result = setParameterCore(call, name->u.base_list.list, var_list, name, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(tmp_result))
            result = tmp_result;
        else{
            Argument *tmp = call;
            result.value->value = makeListValue(&tmp, inter, value_tuple);
        }
        freeArgument(call, false);
        freeParameter(pt, true);
    }
    else{
        Result tmp;
        char *str_name = NULL;

        tmp = getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list));
        if (!run_continue(tmp)) {
            memFree(str_name);
            return tmp;
        }
        addFromVarList(str_name, var_list, int_times, value, tmp.value);
        memFree(str_name);
        result.value = value;
    }
    return result;
}

Result getVar(INTER_FUNCTIONSIG, VarInfo var_info) {
    Result result;
    Result tmp;
    char *name = NULL;
    int int_times;
    setResultOperation(&result, inter);
    tmp = var_info(&name, &int_times, CALL_INTER_FUNCTIONSIG(st, var_list));
    if (!run_continue(tmp)) {
        memFree(name);
        return tmp;
    }
    result.value = findFromVarList(name, var_list, int_times, false);
    memFree(name);
    if (result.value == NULL){
        char *info = memStrcat("Name Not Found: ", st->u.base_var.name, false);
        setResultError(&result, inter, "NameException", info, st, true);
        memFree(info);
    }
    return result;
}

Result getBaseValue(INTER_FUNCTIONSIG) {
    Result result;
    setResult(&result, inter);
    result.value = st->u.base_value.value;
    result.type = operation_return;
    return result;
}

Result getList(INTER_FUNCTIONSIG) {
    Result result;
    Argument *at = getArgument(st->u.base_list.list, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    Argument *at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result;
    }

    int type = st->u.base_list.type == value_tuple ? value_tuple : value_list;
    Value *value = makeListValue(&at, inter, type);
    setResultOperation(&result ,inter);
    result.value->value = value;
    freeArgument(at_tmp, false);
    return result;
}

Result getDict(INTER_FUNCTIONSIG) {
    Result result;
    Argument *at = getArgument(st->u.base_dict.dict, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    Argument *at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result;
    }

    Value *value = makeDictValue(&at, true, inter);
    setResultOperation(&result ,inter);
    result.value->value = value;
    freeArgument(at_tmp, false);
    return result;
}
