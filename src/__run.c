#include "__run.h"

Result getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    Result times_tmp;

    *name = setVarName(st->u.base_var.name, false);
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(&times_tmp, CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list)))
        return times_tmp;
    // TODO-szh 类型检查
    *times = (int)times_tmp.value->value->data.num.num;

    not_times:
    setResult(&result, true, inter);
    return result;
}

Result getVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    Result result;
    if (st->type == base_var)
        result = getBaseVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list));
    else{
        *name = NULL;
        *times = 0;
        setResultError(&result, inter);
    }
    return result;
}

char *setVarName(char *old, bool free_old){
    char *name = memStrcat(VARSTR_PREFIX, old);
    if (free_old)
        memFree(old);
    return name;
}
