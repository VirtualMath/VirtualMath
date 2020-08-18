#include "__run.h"

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

ResultType setClass(INTER_FUNCTIONSIG) {
    Argument *call = NULL;
    LinkValue *tmp = NULL;
    FatherValue *class_father = NULL;
    VarList *father_var = NULL;
    setResultCore(result);

    call = getArgument(st->u.set_class.father, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (!run_continue(result))
        goto error_;

    freeResult(result);
    class_father = setFather(call, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    freeArgument(call, true);
    if (!run_continue(result))
        goto error_;

    tmp = makeLinkValue(makeClassValue(copyVarList(var_list, false, inter), inter, class_father), father, inter);
    gc_addTmpLink(&tmp->gc_status);

    father_var = tmp->value->object.var->next;
    tmp->value->object.var->next = var_list;
    freeResult(result);
    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.set_class.st, tmp->value->object.var, result, tmp));
    tmp->value->object.var->next = father_var;
    if (!run_continue(result))
        goto error_;
    freeResult(result);
    if (st->u.set_class.decoration != NULL){
        setDecoration(st->u.set_class.decoration, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        if (!run_continue(result))
            goto error_;
        gc_freeTmpLink(&tmp->gc_status);
        tmp = result->value;
        result->value = NULL;
        freeResult(result);
    }
    assCore(st->u.set_class.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (run_continue(result))
        setResult(result, inter, father);

    gc_freeTmpLink(&tmp->gc_status);
    return result->type;

    error_:
    gc_freeTmpLink(&tmp->gc_status);
    setResultErrorSt(result, inter, NULL, NULL, st, father, false);
    return result->type;
}

ResultType setFunction(INTER_FUNCTIONSIG) {
    LinkValue *tmp = NULL;
    Value *function_value = NULL;
    VarList *function_var = NULL;
    setResultCore(result);

    function_var = copyVarList(var_list, false, inter);
    function_value = makeVMFunctionValue(st->u.set_function.function, st->u.set_function.parameter, function_var, inter);
    tmp = makeLinkValue(function_value, father, inter);
    gc_addTmpLink(&tmp->gc_status);
    if (st->u.set_function.decoration != NULL){
        setDecoration(st->u.set_function.decoration, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        if (!run_continue(result))
            goto error_;
        gc_freeTmpLink(&tmp->gc_status);
        tmp = result->value;
        result->value = NULL;
        freeResult(result);
    }
    assCore(st->u.set_function.name, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    if (!run_continue(result))
        goto error_;
    setResult(result, inter, father);
    gc_freeTmpLink(&tmp->gc_status);
    return result->type;

    error_:
    gc_freeTmpLink(&tmp->gc_status);
    return result->type;
}

ResultType setLambda(INTER_FUNCTIONSIG) {
    Value *function_value = NULL;
    VarList *function_var = NULL;
    setResultCore(result);

    result->type = operation_return;
    function_var = copyVarList(var_list, false, inter);
    function_value = makeVMFunctionValue(st->u.base_lambda.function, st->u.base_lambda.parameter, function_var, inter);
    result->value = makeLinkValue(function_value, father, inter);
    gc_addTmpLink(&result->value->gc_status);
    return result->type;
}

ResultType callBackCore(LinkValue *function_value, Parameter *parameter, long line, char *file, INTER_FUNCTIONSIG_NOT_ST){
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);
    if (function_value->value->type == function && function_value->value->data.function.type == vm_function)
        callVMFunction(function_value, parameter, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    else if (function_value->value->type == function && function_value->value->data.function.type == c_function)
        callCFunction(function_value, parameter, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    else if (function_value->value->type == class)
        callClass(function_value, parameter, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    else{
        setResultError(result, inter, "TypeException", "Object is not callable", line, file, father, true);
        goto return_;
    }
    setResultError(result, inter, NULL, NULL, line, file, father, false);
    return_:
    gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

ResultType callBack(INTER_FUNCTIONSIG) {
    LinkValue *function_value = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.call_function.function, var_list, result, father)))
        goto return_;
    function_value = result->value;
    result->value = NULL;
    freeResult(result);
    callBackCore(function_value, st->u.call_function.parameter, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    gc_freeTmpLink(&function_value->gc_status);
    return_:
    return result->type;
}

ResultType callClass(LinkValue *class_value, Parameter *parameter, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *value = NULL;
    LinkValue *_init_ = NULL;
    setResultCore(result);

    value = makeLinkValue(makeObject(inter, NULL,copyVarList(class_value->value->object.out_var, false, inter),
                          setFatherCore(makeFatherValue(class_value))), father, inter);
    setResultOperation(result, value);

    char *init_name = setStrVarName(inter->data.object_init, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    _init_ = findFromVarList(init_name, 0, false, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
    memFree(init_name);

    if (_init_ != NULL){
        Result _init_result;
        setResultCore(&_init_result);
        _init_->father = value;
        gc_addTmpLink(&_init_->gc_status);
        callBackCore(_init_, parameter, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &_init_result, value));
        gc_freeTmpLink(&_init_->gc_status);

        if (!run_continue_type(_init_result.type)){
            freeResult(result);
            *result = _init_result;
            goto return_;
        }
        freeResult(&_init_result);
    }

    return_:
    return result->type;
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

Statement *getRunInfoStatement(Statement *funtion_st){  // TODO-szh 去除该函数
    return funtion_st->info.node;
}

ResultType callCFunction(LinkValue *function_value, Parameter *parameter, INTER_FUNCTIONSIG_NOT_ST){
    VarList *function_var = NULL;
    OfficialFunction of = NULL;
    Argument *self_tmp = makeValueArgument(function_value);
    Argument *father_tmp = makeValueArgument(function_value->father);
    Argument *arg = NULL;
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);
    of = function_value->value->data.function.of;
    function_var = function_value->value->object.out_var;
    gc_freeze(inter, var_list, function_var, true);

    gc_addTmpLink(&function_var->hashtable->gc_status);
    arg = getArgument(parameter, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    self_tmp->next = father_tmp;
    father_tmp->next = arg;
    gc_freeTmpLink(&function_var->hashtable->gc_status);
    if (!run_continue(result))
        goto return_;

    freeResult(result);
    of(CALL_OfficialFunction(self_tmp, function_var, result, function_value));

    return_:
    freeArgument(self_tmp, true);
    gc_freeze(inter, var_list, function_var, false);
    gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

ResultType callVMFunction(LinkValue *function_value, Parameter *parameter, long int line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    VarList *function_var = NULL;
    Statement *funtion_st = NULL;
    bool yield_run = false;
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);
    funtion_st = function_value->value->data.function.function;
    if ((yield_run = popStatementVarList(funtion_st, &function_var, function_value->value->object.out_var, inter)))
        funtion_st = getRunInfoStatement(funtion_st);

    gc_freeze(inter, var_list, function_var, true);
    gc_addTmpLink(&function_var->hashtable->gc_status);
    setParameter(line, file, parameter, function_value->value->data.function.pt, function_var, function_value,
                 CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
    gc_freeTmpLink(&function_var->hashtable->gc_status);
    if (!run_continue(result)) {
        gc_freeze(inter, var_list, function_var, false);
        funtion_st = function_value->value->data.function.function;
        if (yield_run)
            freeFunctionYield(funtion_st, inter);
        else
            popVarList(function_var);
        goto return_;
    }
    freeResult(result);
    functionSafeInterStatement(CALL_INTER_FUNCTIONSIG(funtion_st, function_var, result, function_value));

    gc_freeze(inter, var_list, function_var, false);
    funtion_st = function_value->value->data.function.function;
    if (yield_run) {
        if (result->type == yield_return){
            updateFunctionYield(funtion_st, result->node);
            result->type = operation_return;
        }
        else
            freeFunctionYield(funtion_st, inter);
    }
    else {
        if (result->type == yield_return){
            newFunctionYield(funtion_st, result->node, function_var, inter);
            result->type = operation_return;
        }
        else
            popVarList(function_var);
    }
    return_:
    gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

ResultType setDecoration(DecorationStatement *ds, LinkValue *value, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *decall = NULL;
    Parameter *pt = NULL;
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);
    for (PASS; ds != NULL; ds = ds->next){
        freeResult(result);
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(ds->decoration, var_list, result, father)))
            break;
        pt = makeValueParameter(makeBaseLinkValueStatement(value, ds->decoration->line, ds->decoration->code_file));
        decall = result->value;
        result->value = NULL;
        freeResult(result);
        callBackCore(decall, pt, ds->decoration->line, ds->decoration->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        gc_freeTmpLink(&decall->gc_status);
        freeParameter(pt, true);
        if (!run_continue(result))
            break;
        gc_freeTmpLink(&value->gc_status);
        value = result->value;
        gc_addTmpLink(&value->gc_status);
    }
    gc_freeTmpLink(&value->gc_status);
    return result->type;
}
