#include "__run.h"

Result getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    Result times_tmp;

    *name = setStrVarName(st->u.base_var.name, false, inter, var_list);
    *times = 0;
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times_tmp, CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list)))
        return times_tmp;
    if (!isType(times_tmp.value->value, number)){
        setResultError(&result, inter, "TypeException", "Don't get a number value", st, true);
        goto return_;
    }
    *times = (int)times_tmp.value->value->data.num.num;

    not_times:
    setResultOperation(&result, inter);
    result.value->value = makeStringValue(st->u.base_var.name, inter);
    return_: return result;
}

Result getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    Result times_tmp;

    if (operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(st->u.base_svar.name, var_list)))
        return result;
    *name = getNameFromValue(result.value->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times_tmp, CALL_INTER_FUNCTIONSIG(st->u.base_svar.times, var_list)))
        return times_tmp;
    if (!isType(times_tmp.value->value, number)){
        setResultError(&result, inter, "TypeException", "Don't get a number value", st, true);
        goto return_;
    }
    *times = (int)times_tmp.value->value->data.num.num;

    not_times:
    result.type = operation_return;
    return_: return result;
}

Result getVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    if (st->type == base_var)
        result = getBaseVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list));
    else if (st->type == base_svar)
        result = getBaseSVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list));
    else{
        if (operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(st, var_list)))
            return result;
        *name = getNameFromValue(result.value->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        *times = 0;
    }
    return result;
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
            return memStrcpy(inter->data.var_defualt, 0, false, false);
    }
}
