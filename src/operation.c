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
            setResult(&result, true, inter);
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
        char *new_string = memStrcat(left.value->value->data.str.str, right.value->value->data.str.str);
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
    Result times;
    setResult(&result, true, inter);
    int int_times;
    if (name->type == base_var){
        if (name->u.base_var.times == NULL){
            int_times = 0;
            goto not_times;
        }

        if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(name->u.base_var.times, var_list))){
            return times;
        }

        int_times = (int)times.value->value->data.num.num;
        not_times:
        addFromVarList(name->u.base_var.name, var_list, int_times, value);
    }
    return result;
}

Result getBaseVar(INTER_FUNCTIONSIG) {
    Result result;
    Result times;
    int int_times;
    setResultOperation(&result, inter);

    if (st->u.base_var.times == NULL){
        int_times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times, CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list))){
        return times;
    }
    int_times = (int)times.value->value->data.num.num;

    not_times:
    result.value = findFromVarList(st->u.base_var.name, var_list, int_times);
    if (result.value == NULL){
        writeLog_(inter->debug, WARNING, "var not found[%s]\n", st->u.base_var.name);
        setResultError(&result, inter);
    }
    return result;
}

Result getBaseValue(INTER_FUNCTIONSIG) {
    Result result;
    setResult(&result, true, inter);
    result.value = st->u.base_value.value;
    result.type = operation_return;
    return result;
}

Result getList(INTER_FUNCTIONSIG) {
    Result result;
    Argument *at = getArgument(st->u.base_list.list, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list)), *at_tmp = at;
    if (!run_continue(result)){
        freeArgument(at_tmp, true);
        return result;
    }

    int type = st->u.base_list.type == tuple_ ? value_tuple : value_list;
    Value *value = makeListValue(&at, inter, type);
    setResultOperation(&result ,inter);
    result.value->value = value;
    freeArgument(at_tmp, true);
    return result;
}
