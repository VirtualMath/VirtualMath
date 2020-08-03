#include "__run.h"

Result getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    Result times_tmp;

    *name = setStrVarName(st->u.base_var.name, false);

    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times_tmp, CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list)))
        return times_tmp;
    *times = (int)times_tmp.value->value->data.num.num; // TODO-szh 类型检查

    not_times:
    setResult(&result, true, inter);
    return result;
}

Result getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    Result value;
    Result times_tmp;

    if (operationSafeInterStatement(&value, CALL_INTER_FUNCTIONSIG(st->u.base_svar.name, var_list)))
        return value;
    *name = getNameFromValue(value.value->value);

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times_tmp, CALL_INTER_FUNCTIONSIG(st->u.base_svar.times, var_list)))
        return times_tmp;
    *times = (int)times_tmp.value->value->data.num.num; // TODO-szh 类型检查

    not_times:
    setResult(&result, true, inter);
    return result;
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
        *name = getNameFromValue(result.value->value);
        *times = 0;
    }
    return result;
}

char *setStrVarName(char *old, bool free_old){
    char *name = memStrcat(VARSTR_PREFIX, old);
    if (free_old)
        memFree(old);
    return name;
}

char *setNumVarName(NUMBER_TYPE num){
    char name[50];
    snprintf(name, 50, "%"NUMBER_FORMAT, num);
    return memStrcpy(name, 0, false, false);
}

char *getNameFromValue(Value *value){
    switch (value->type){
        case string:
            return setStrVarName(value->data.str.str, false);
        case number:
            return setNumVarName(value->data.num.num);
        default:
            return memStrcpy(VARDEFAULT_PREFIX, 0, false, false);
    }
}
