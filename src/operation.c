#include "__virtualmath.h"
/**
 * operation.c中是用于数学计算的函数
 */

#define getresult(base, var, inter) var = iterStatement(callFunctionSig(st->u.operation. base, var_list))
#define viewtype_core(a, b, valuetype_a, valuetype_a_b) a .value->value->type == valuetype_a && b.value->value->type == valuetype_a_b
#define viewtype(a, b, valuetype) viewtype_core(a, b, valuetype, valuetype)
#define operationValue(a, b, type, symbol) a.value->value->data.type symbol b.value->value->data.type
#define valueToResult(result, result_value, type, inter) result.value->value = make##type##Value(result_value, inter)

Result addOperation(baseFunctionSig);
Result subOperation(baseFunctionSig);
Result mulOperation(baseFunctionSig);
Result divOperation(baseFunctionSig);
Result assOperation(baseFunctionSig);

Result operationStatement(baseFunctionSig) {
    Result result;
    setResult(&result, true, inter);
    switch (st->u.operation.OperationType) {
        case ADD:
            result = addOperation(callFunctionSig(st, var_list));
            break;
        case SUB:
            result = subOperation(callFunctionSig(st, var_list));
            break;
        case MUL:
            result = mulOperation(callFunctionSig(st, var_list));
            break;
        case DIV:
            result = divOperation(callFunctionSig(st, var_list));
            break;
        case ASS:
            result = assOperation(callFunctionSig(st, var_list));
            break;
        default:
            break;
    }
    return result;
}

Result addOperation(baseFunctionSig) {
    Result left, right, result;
    setResult(&result, true, inter);
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
    return result;
}

Result subOperation(baseFunctionSig) {
    Result left, right, result;
    setResult(&result, true, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, -)), Number, inter);
    }
    return result;
}

Result mulOperation(baseFunctionSig) {
    Result left, right, result;
    setResult(&result, true, inter);
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
    return result;
}

Result divOperation(baseFunctionSig) {
    Result left, right, result;
    setResult(&result, true, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, /)), Number, inter);
    }
    return result;
}

Result assOperation(baseFunctionSig) {
    Result times, right;
    int int_times;
    getresult(right, right, inter);
    if (st->u.operation.left->type == base_var){
        if (st->u.operation.left->u.base_var.times == NULL){
            int_times = 0;
            goto not_times;
        }
        times = iterStatement(callFunctionSig(st->u.operation.left->u.base_var.times, var_list));
        int_times = (int)times.value->value->data.num.num;
        not_times:
        addFromVarList(st->u.operation.left->u.base_var.name, var_list, int_times, right.value);
    }
    return right;
}
