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
        setResultErrorSt(result, inter, "TypeException", "Don't get a number value", st, father, true);
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    value = makeLinkValue(makeStringValue(st->u.base_var.name, inter), father, inter);
    setResultOperation(result, value);

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
        setResultErrorSt(result, inter, "TypeException", "Don't get a number value", st, father, true);
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
    return memStrcat(inter->data.var_str_prefix, old, false, free_old);
}

char *setNumVarName(NUMBER_TYPE num, INTER_FUNCTIONSIG_CORE) {
    char name[50];
    snprintf(name, 50, "%"NUMBER_FORMAT, num);
    return memStrcat(inter->data.var_num_prefix, name, false, false);
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

bool popStatementVarList(Statement *funtion_st, VarList **function_var, VarList *out_var, Inter *inter){
    bool yield_run;
    if ((yield_run = funtion_st->info.have_info)) {
        *function_var = funtion_st->info.var_list;
        (*function_var)->next = out_var;
    }
    else
        *function_var = pushVarList(out_var, inter);
    return yield_run;
}

void newFunctionYield(Statement *funtion_st, Statement *node, VarList *new_var, Inter *inter){
    new_var->next = NULL;
    gc_freeze(inter, new_var, NULL, true);
    funtion_st->info.var_list = new_var;
    funtion_st->info.node = node->type == yield_code ? node->next : node;
    funtion_st->info.have_info = true;
}

void updateFunctionYield(Statement *function_st, Statement *node){
    function_st->info.node = node->type == yield_code ? node->next : node;
    function_st->info.have_info = true;
}

void freeFunctionYield(Statement *function_st, Inter *inter){  // TODO-szh 去除该函数
    function_st->info.var_list->next = NULL;
    gc_freeze(inter, function_st->info.var_list, NULL, false);
    freeVarList(function_st->info.var_list);
    function_st->info.var_list = NULL;
    function_st->info.have_info = false;
    function_st->info.node = NULL;
}

Statement *getRunInfoStatement(Statement *funtion_st){  // TODO-szh 去除该函数
    return funtion_st->info.node;
}

ResultType setFunctionArgument(Argument **arg, LinkValue *function_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST){
    Argument *tmp = NULL;
    setResultCore(result);
    if (function_value->father != NULL)
        tmp = makeValueArgument(function_value->father);
    else{
        setResultError(result, inter, "ArgumentException", "Don't get self", line, file, father, true);
        return error_return;
    }
    tmp->next = makeValueArgument(function_value);
    tmp->next->next = *arg;
    *arg = tmp;
    setResultBase(result, inter, father);
    return result->type;
}

void freeFunctionArgument(Argument *arg){
    arg->next->next = NULL;
    freeArgument(arg, true);
}