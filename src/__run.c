#include "__run.h"

ResultType getBaseVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    LinkValue *value;

    *name = setStrVarName(st->u.base_var.name, false, inter);
    *times = 0;
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultErrorSt(result, inter, "TypeException", "Don't get a number value", st, belong, true);
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    value = makeLinkValue(makeStringValue(st->u.base_var.name, inter), belong, inter);
    setResultOperation(result, value);

    return result->type;
}

ResultType getBaseSVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    freeResult(result);

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultErrorSt(result, inter, "TypeException", "Don't get a number value", st, belong, true);
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;

    freeResult(result);
    not_times:
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.name, var_list, result, belong)))
        return result->type;

    *name = getNameFromValue(result->value->value, inter);
    result->type = operation_return;  // 执行 operationSafeInterStatement 的时候已经初始化 result

    return result->type;
}

ResultType getVarInfo(char **name, int *times, INTER_FUNCTIONSIG){
    if (st->type == base_var)
        getBaseVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
    else if (st->type == base_svar)
        getBaseSVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
    else{
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
            return result->type;
        *name = getNameFromValue(result->value->value, inter);
        *times = 0;
    }
    return result->type;
}

char *setStrVarName(char *old, bool free_old, Inter *inter) {
    return memStrcat(inter->data.var_str_prefix, old, false, free_old);
}

char *setNumVarName(NUMBER_TYPE num, struct Inter *inter) {
    char name[50];
    snprintf(name, 50, "%"NUMBER_FORMAT, num);
    return memStrcat(inter->data.var_num_prefix, name, false, false);
}

char *getNameFromValue(Value *value, struct Inter *inter) {
    switch (value->type){
        case string:
            return setStrVarName(value->data.str.str, false, inter);
        case number:
            return setNumVarName(value->data.num.num, inter);
        case bool_:
            if (value->data.bool_.bool_)
                return memStrcat(inter->data.var_bool_prefix, "true", false, false);
            else
                return memStrcat(inter->data.var_bool_prefix, "false", false, false);
        case none:
            return memStrcpy(inter->data.var_none);
        case pass_:
            return memStrcpy(inter->data.var_pass);
        case class:{
            size_t len = memStrlen(inter->data.var_class_prefix) + 20;
            char *name = memString(len);
            char *return_ = NULL;
            snprintf(name, len, "%s%p", inter->data.var_class_prefix, value);
            return_ = memStrcpy(name);
            memFree(name);
            return return_;
        }
        default:{
            size_t len = memStrlen(inter->data.var_object_prefix) + 20;
            char *name = memString(len);
            char *return_ = NULL;
            snprintf(name, len, "%s%p", inter->data.var_object_prefix, value);
            return_ = memStrcpy(name);
            memFree(name);
            return return_;
        }
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

ResultType setFunctionArgument(Argument **arg, LinkValue *function_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST){
    Argument *tmp = NULL;
    enum FunctionPtType pt_type = function_value->value->data.function.function_data.pt_type;
    setResultCore(result);
    if (function_value->belong == NULL){
        setResultError(result, inter, "ArgumentException", "Don't get self", line, file, belong, true);
        return error_return;
    }

    switch (pt_type) {
        case static_:
            tmp = makeValueArgument(function_value);
            tmp->next = *arg;
            *arg = tmp;
            break;
        case class_static_:
            tmp = makeValueArgument(function_value);
            tmp->next = makeValueArgument(function_value->belong);
            tmp->next->next = *arg;
            *arg = tmp;
            break;
        case object_static_:
            tmp = makeValueArgument(function_value);
            if (function_value->belong->value->type == class)
                tmp->next = *arg;
            else {
                tmp->next = makeValueArgument(function_value->belong);
                tmp->next->next = *arg;
            }
            *arg = tmp;
            break;
        case class_free_:
            tmp = makeValueArgument(function_value->belong);
            tmp->next = *arg;
            *arg = tmp;
            break;
        case object_free_:
            if (function_value->belong->value->type != class) {
                tmp = makeValueArgument(function_value->belong);
                tmp->next = *arg;
                *arg = tmp;
            }
            break;
        default:
            break;
    }
    setResultBase(result, inter, belong);
    return result->type;
}

void freeFunctionArgument(Argument *arg, Argument *base) {
    for (Argument *tmp = arg; tmp != NULL; tmp = tmp->next) {
        if (tmp->next == base) {
            tmp->next = NULL;
            freeArgument(arg, true);
            break;
        }
    }
}

LinkValue *findStrVar(char *name, bool free_old, INTER_FUNCTIONSIG_CORE){
    LinkValue *tmp = NULL;
    char *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, get_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(name_);
    return tmp;
}

LinkValue *checkStrVar(char *name, bool free_old, INTER_FUNCTIONSIG_CORE){
    LinkValue *tmp = NULL;
    char *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, read_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(name_);
    return tmp;
}

void addStrVar(char *name, bool free_old, LinkValue *value, LinkValue *belong, INTER_FUNCTIONSIG_CORE){
    char *var_name = setStrVarName(name, free_old, inter);
    LinkValue *name_ = makeLinkValue(makeStringValue(var_name, inter), belong, inter);
    addFromVarList(var_name, name_, 0, value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(var_name);
}

LinkValue *findAttributes(char *name, bool free_old, LinkValue *value, Inter *inter) {
    LinkValue *attr = findStrVar(name, free_old, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
    if (attr != NULL && (attr->belong == NULL || attr->belong->value != value->value && checkAttribution(value->value, attr->belong->value)))
        attr->belong = value;
    return attr;
}

void addAttributes(char *name, bool free_old, LinkValue *value, LinkValue *belong, Inter *inter){
    addStrVar(name, free_old, value, belong, inter, belong->value->object.var);
}

ResultType elementDownOne(LinkValue *element, LinkValue *index, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    gc_addTmpLink(&element->gc_status);
    gc_addTmpLink(&index->gc_status);

    _func_ = findAttributes(inter->data.object_down, false, element, inter);
    if (_func_ != NULL){
        Argument *arg = NULL;
        gc_addTmpLink(&_func_->gc_status);
        arg = makeValueArgument(index);
        callBackCore(_func_,arg, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else
        setResultError(result, inter, "TypeException", "Don't find __down__", line, file, belong, true);

    gc_freeTmpLink(&element->gc_status);
    gc_freeTmpLink(&index->gc_status);
    return result->type;
}

ResultType getIter(LinkValue *value, int status, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    if (status == 1)
        _func_ = findAttributes(inter->data.object_iter, false, value, inter);
    else
        _func_ = findAttributes(inter->data.object_next, false, value, inter);

    if (_func_ != NULL){
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultError(result, inter, "IterException", "Object Not Iterable", line, file, belong, true);

    return result->type;
}
