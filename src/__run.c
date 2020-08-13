#include "__run.h"

ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    LinkValue *value;

    *name = setStrVarName(st->u.base_var.name, false, inter, var_list);
    *times = 0;
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list, result, father)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultError(result, inter, "TypeException", "Don't get a number value", st, father, true);
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    value = makeLinkValue(makeStringValue(st->u.base_var.name, inter), father, inter);
    setResultOperation(result, value, inter);

    return result->type;
}

ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    freeResult(result);

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.times, var_list, result, father)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultError(result, inter, "TypeException", "Don't get a number value", st, father, true);
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;

    freeResult(result);
    not_times:
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.name, var_list, result, father)))
        return result->type;

    *name = getNameFromValue(result->value->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    result->type = operation_return;  // 执行 operationSafeInterStatement 的时候已经初始化 result

    return result->type;
}

ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    if (st->type == base_var)
        getBaseVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    else if (st->type == base_svar)
        getBaseSVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, father));
    else{
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, father)))
            return result->type;
        *name = getNameFromValue(result->value->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        *times = 0;
    }
    return result->type;
}

char *setStrVarName(char *old, bool free_old, INTER_FUNCTIONSIG_CORE) {
    char *name = memStrcat(inter->data.var_str_prefix, old, false);
    if (free_old)
        memFree(old);
    return name;
}

char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE) {
    char name[50];
    snprintf(name, 50, "%"NUMBER_FORMAT, num);
    return memStrcat(inter->data.var_num_prefix, name, false);
}

char *getNameFromValue(Value *value, INTER_FUNCTIONSIG_CORE) {
    switch (value->type){
        case string:
            return setStrVarName(value->data.str.str, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        case number:
            return setNumVarName(value->data.num.num, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        default:
            return memStrcpy(inter->data.var_defualt);
    }
}
