#include "__virtualmath.h"
/**
 * operation.c中是用于数学计算的函数
 */

#define getresult(base, var, inter) var = iterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation. base, var_list))
#define viewtype_core(a, b, valuetype_a, valuetype_a_b) a .value->value->type == valuetype_a && b.value->value->type == valuetype_a_b
#define viewtype(a, b, valuetype) viewtype_core(a, b, valuetype, valuetype)
#define operationValue(a, b, type, symbol) a.value->value->data.type symbol b.value->value->data.type
#define valueToResult(result, result_value, type, inter) result.value->value = make##type##Value(result_value, inter)

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
    setResult(&result, true, inter);
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
            break;
    }
    return result;
}

Result addOperation(INTER_FUNCTIONSIG) {
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

Result subOperation(INTER_FUNCTIONSIG) {
    Result left, right, result;
    setResult(&result, true, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, -)), Number, inter);
    }
    return result;
}

Result mulOperation(INTER_FUNCTIONSIG) {
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

Result divOperation(INTER_FUNCTIONSIG) {
    Result left, right, result;
    setResult(&result, true, inter);
    getresult(left, left, inter);
    getresult(right, right, inter);
    if (viewtype(left, right, number)){
        valueToResult(result, (operationValue(left, right, num.num, /)), Number, inter);
    }
    return result;
}

Result assOperation(INTER_FUNCTIONSIG) {
    Result times, right;
    int int_times;
    getresult(right, right, inter);
    if (st->u.operation.left->type == base_var){
        if (st->u.operation.left->u.base_var.times == NULL){
            int_times = 0;
            goto not_times;
        }
        times = iterStatement(CALL_INTER_FUNCTIONSIG(st->u.operation.left->u.base_var.times, var_list));
        int_times = (int)times.value->value->data.num.num;
        not_times:
        addFromVarList(st->u.operation.left->u.base_var.name, var_list, int_times, right.value);
    }
    return right;
}
