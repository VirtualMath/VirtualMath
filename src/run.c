#include "__virtualmath.h"
#define printResult(result, first, last) do{ \
switch (result.value->value->type){ \
    case number: \
        printf("%s %ld %s \n", first, result.value->value->data.num.num, last); \
        break; \
    case string: \
        printf("%s %s %s \n", first, result.value->value->data.str.str, last); \
        break; \
    default: \
        printf("%s default %s \n", first, last); \
        break; \
} \
} while(0) \

Result getBaseVar(baseFunctionSig) {
    Result times, result;
    int int_times;
    if (st->u.base_var.times == NULL){
        int_times = 0;
        goto not_times;
    }
    times = iterStatement(callFunctionSig(st->u.base_var.times, var_list));
    int_times = (int)times.value->value->data.num.num;

    not_times:
    result.value = findFromVarList(st->u.base_var.name, var_list, int_times);
    if (result.value == NULL){
        printf("not found[%s]\n", st->u.base_var.name);
    }
    return result;
}

Result runStatement(baseFunctionSig) {
    Result result;
    setResult(&result, true, inter);
    switch (st->type) {
        case base_value:
            result.value->value = st->u.base_value.value;
            break;
        case base_var:
            result = getBaseVar(callFunctionSig(st, var_list));
            break;
        case operation:
            result = operationStatement(callFunctionSig(st, var_list));
            printResult(result, "operation result = ", "");
            break;
        default:
            break;
    }
    return result;
}

Result iterStatement(baseFunctionSig) {
    Result result;
    Statement *base_st = st;
    VarList *new_var_list = var_list;
    while(base_st != NULL){
        result = runStatement(callFunctionSig(base_st, new_var_list));
        base_st = base_st->next;
    }
    return result;
}

Result globalIterStatement(Inter *inter) {
    Result result;
    Statement *base_st = inter->statement;
    VarList *new_var_list = inter->var_list;
    while(base_st != NULL){
        result = runStatement(callFunctionSig(base_st, new_var_list));
        base_st = base_st->next;
    }
    return result;
}
