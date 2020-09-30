#include "__run.h"

ResultType setClass(FUNC) {
    Argument *call = NULL;
    LinkValue *tmp = NULL;
    Inherit *class_inherit = NULL;
    setResultCore(result);

    call = getArgument(st->u.set_class.father, false, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto error_;

    class_inherit = setFather(call);
    freeArgument(call, false);

    tmp = makeLinkValue(makeClassValue(var_list, inter, class_inherit), belong, inter);
    gc_addTmpLink(&tmp->gc_status);
    freeResult(result);

    {
        enum FunctionPtType pt_type_bak = inter->data.default_pt_type;
        VarList *var_backup = tmp->value->object.var->next;
        inter->data.default_pt_type = object_free_;
        tmp->value->object.var->next = var_list;

        gc_freeze(inter, var_backup, NULL, true);
        // 运行类定义的时候需要调整belong
        functionSafeInterStatement(CFUNC(st->u.set_class.st, tmp->value->object.var, result, tmp));
        gc_freeze(inter, var_backup, NULL, false);

        tmp->value->object.var->next = var_backup;
        inter->data.default_pt_type = pt_type_bak;

        if (result->type != R_yield && !CHECK_RESULT(result))
            goto error_;
        freeResult(result);
    }
    if (st->u.set_class.decoration != NULL){
        setDecoration(st->u.set_class.decoration, tmp, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto error_;
        gc_freeTmpLink(&tmp->gc_status);
        tmp = result->value;
        result->value = NULL;
        freeResult(result);
    }

    assCore(st->u.set_class.name, tmp, false, true, CNEXT_NT);
    if (CHECK_RESULT(result))
        setResult(result, inter);

    gc_freeTmpLink(&tmp->gc_status);
    return result->type;

    error_:
    gc_freeTmpLink(&tmp->gc_status);
    setResultErrorSt(E_BaseException, NULL, false, st, CNEXT_NT);
    return result->type;
}

ResultType setFunction(FUNC) {
    LinkValue *func = NULL;
    setResultCore(result);

    makeVMFunctionValue(st->u.set_function.function, st->u.set_function.parameter, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    func = result->value;
    result->value = NULL;
    freeResult(result);

    {
        enum FunctionPtType pt_type_bak = inter->data.default_pt_type;
        VarList *var_backup = func->value->object.var->next;
        inter->data.default_pt_type = object_free_;
        func->value->object.var->next = var_list;
        // 运行函数初始化模块的时候需要调整belong
        functionSafeInterStatement(CFUNC(st->u.set_function.first_do, func->value->object.var, result, func));
        func->value->object.var->next = var_backup;
        inter->data.default_pt_type = pt_type_bak;
        if (result->type != R_yield && !CHECK_RESULT(result))
            goto error_;
        freeResult(result);
    }

    if (st->u.set_function.decoration != NULL){
        setDecoration(st->u.set_function.decoration, func, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto error_;
        gc_freeTmpLink(&func->gc_status);
        func = result->value;
        result->value = NULL;
        freeResult(result);
    }
    assCore(st->u.set_function.name, func, false, true, CNEXT_NT);
    if (CHECK_RESULT(result))
        setResult(result, inter);

    error_:
    gc_freeTmpLink(&func->gc_status);
    return result->type;
}

ResultType setLambda(FUNC) {
    Statement *resunt_st = NULL;
    setResultCore(result);

    resunt_st = makeReturnStatement(st->u.base_lambda.function, st->line, st->code_file);
    makeVMFunctionValue(resunt_st, st->u.base_lambda.parameter, CNEXT_NT);
    resunt_st->u.return_code.value = NULL;
    freeStatement(resunt_st);
    return result->type;
}

ResultType elementSlice(FUNC) {
    LinkValue *element = NULL;
    LinkValue *_func_ = NULL;
    wchar_t *func_name = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CFUNC(st->u.slice_.element, var_list, result, belong)))
        return result->type;
    element = result->value;
    result->value = NULL;
    freeResult(result);

    func_name = st->u.slice_.type == SliceType_down_ ? inter->data.object_down : inter->data.object_slice;
    _func_ = findAttributes(func_name, false, LINEFILE, true, CFUNC_NT(var_list, result, element));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_func_ != NULL){
        gc_addTmpLink(&_func_->gc_status);
        callBackCorePt(_func_, st->u.slice_.index, st->line, st->code_file, CNEXT_NT);
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(__down__/__slice__), true, st, CNEXT_NT);

    return_:
    gc_freeTmpLink(&element->gc_status);
    return result->type;
}

ResultType callBack(FUNC) {
    LinkValue *function_value = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CFUNC(st->u.call_function.function, var_list, result, belong)))
        return result->type;
    function_value = result->value;
    result->value = NULL;
    freeResult(result);
    callBackCorePt(function_value, st->u.call_function.parameter, st->line, st->code_file, CNEXT_NT);
    gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

ResultType callBackCorePt(LinkValue *function_value, Parameter *pt, long line, char *file, FUNC_NT) {
    Argument *arg = NULL;
    int pt_sep =1;
    bool sep = false;
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);

    arg = getArgument(pt, false, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;
    for (Parameter *tmp = pt; tmp != NULL; tmp = tmp->next, pt_sep++) {
        if (tmp->data.is_sep) {
            sep = true;
            break;
        }
    }

    freeResult(result);
    callBackCore(function_value, arg, line, file, sep ? pt_sep : 0, CNEXT_NT);

    return_:
    gc_freeTmpLink(&function_value->gc_status);
    freeArgument(arg, false);
    return result->type;
}

static ResultType callClass(LinkValue *class_value, Argument *arg, fline line, char *file, int pt_sep, FUNC_NT) {
    LinkValue *_new_;
    setResultCore(result);
    gc_addTmpLink(&class_value->gc_status);

    _new_ = findAttributes(inter->data.object_new, false, LINEFILE, true, CFUNC_NT(var_list, result, class_value));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_new_ != NULL){
        gc_addTmpLink(&_new_->gc_status);
        callBackCore(_new_, arg, line, file, pt_sep, CNEXT_NT);
        gc_freeTmpLink(&_new_->gc_status);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(new(__new__)), line, file, true, CNEXT_NT);

    return_:
    gc_freeTmpLink(&class_value->gc_status);
    return result->type;
}

static ResultType callObject(LinkValue *object_value, Argument *arg, fline line, char *file, int pt_sep, FUNC_NT) {
    LinkValue *_call_;
    setResultCore(result);
    gc_addTmpLink(&object_value->gc_status);

    _call_ = findAttributes(inter->data.object_call, false, LINEFILE, true, CFUNC_NT(var_list, result, object_value));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_call_ != NULL){
        gc_addTmpLink(&_call_->gc_status);
        callBackCore(_call_, arg, line, file, pt_sep, CNEXT_NT);
        gc_freeTmpLink(&_call_->gc_status);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(call(__call__)), line, file, true, CNEXT_NT);

    return_:
    gc_freeTmpLink(&object_value->gc_status);
    return result->type;
}

static ResultType callCFunction(LinkValue *function_value, Argument *arg, long int line, char *file, int pt_sep, FUNC_NT){
    VarList *function_var = NULL;
    OfficialFunction of = NULL;
    Argument *bak;
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);

    setFunctionArgument(&arg, &bak, function_value, line, file, pt_sep, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;

    of = function_value->value->data.function.of;
    function_var = pushVarList(function_value->value->object.out_var != NULL ? function_value->value->object.out_var : var_list, inter);
    gc_freeze(inter, var_list, function_var, true);

    freeResult(result);
    of(CO_FUNC(arg, function_var, result, function_value));
    if (result->type == R_func)
        result->type = R_opt;
    else if (result->type != R_opt && result->type != R_error)
        setResult(result, inter);

    gc_freeze(inter, var_list, function_var, false);
    popVarList(function_var);
    freeFunctionArgument(arg, bak);

    return_: gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

static void updateFunctionYield(Statement *func_st, Statement *node){
    func_st->info.node = node->type == yield_code ? node->next : node;
    func_st->info.have_info = true;
}

static void newFunctionYield(Statement *func_st, Statement *node, VarList *new_var, Inter *inter){
    new_var->next = NULL;
    gc_freeze(inter, new_var, NULL, true);
    func_st->info.var_list = new_var;
    func_st->info.node = node->type == yield_code ? node->next : node;
    func_st->info.have_info = true;
}

static void setFunctionResult(LinkValue *func_value, bool yield_run, Result *result, FUNC_CORE) {
    Statement *st_func = func_value->value->data.function.function;
    if (yield_run) {
        if (result->type == R_yield) {
            updateFunctionYield(st_func, result->node);
            result->type = R_opt;
            result->is_yield = true;
        } else
            freeRunInfo(st_func);
    } else {
        if (result->type == R_yield) {
            newFunctionYield(st_func, result->node, var_list, inter);
            result->type = R_opt;
            result->is_yield = true;
        } else
            popVarList(var_list);
    }
}

static ResultType callVMFunction(LinkValue *func_value, Argument *arg, long int line, char *file, int pt_sep, FUNC_NT) {
    Argument *bak;
    VarList *var_func = NULL;
    Statement *st_func = NULL;
    Parameter *pt_func = func_value->value->data.function.pt;
    bool yield_run = false;
    setResultCore(result);
    st_func = func_value->value->data.function.function;

    if (st_func == NULL) {
        setResult(result, inter);
        return result->type;
    }

    gc_addTmpLink(&func_value->gc_status);
    {
        VarList *out_var;
        if (func_value->value->object.out_var == NULL)
            out_var = var_list;  // 当out_var等于空的时候为内联函数
        else
            out_var = func_value->value->object.out_var;
        yield_run = popYieldVarList(st_func, &var_func, out_var, inter);
    }
    if (yield_run)
        st_func = st_func->info.node;

    gc_freeze(inter, var_list, var_func, true);
    setFunctionArgument(&arg, &bak, func_value, line, file, pt_sep, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    gc_addTmpLink(&var_func->hashtable->gc_status);
    setParameterCore(line, file, arg, pt_func, var_func, CFUNC_NT(var_list, result, func_value));
    freeFunctionArgument(arg, bak);
    gc_freeTmpLink(&var_func->hashtable->gc_status);

    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    functionSafeInterStatement(CFUNC(st_func, var_func, result, func_value));  // 运行函数的时候, belong调整为函数本身

    return_:
    gc_freeze(inter, var_list, var_func, false);
    setFunctionResult(func_value, yield_run, result, CFUNC_CORE(var_func));
    gc_freeTmpLink(&func_value->gc_status);
    return result->type;
}

ResultType callBackCore(LinkValue *function_value, Argument *arg, fline line, char *file, int pt_sep, FUNC_NT) {
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);
    if (function_value->value->type == V_func && function_value->value->data.function.type == vm_func)
        callVMFunction(function_value, arg, line, file, pt_sep, CNEXT_NT);
    else if (function_value->value->type == V_func && function_value->value->data.function.type == c_func)
        callCFunction(function_value, arg, line, file, pt_sep, CNEXT_NT);
    else if (function_value->value->type == V_class)
        callClass(function_value, arg, line, file, pt_sep, CNEXT_NT);
    else
        callObject(function_value, arg, line, file, pt_sep, CNEXT_NT);
    setResultError(E_BaseException, NULL, line, file, false, CNEXT_NT);

    gc_freeTmpLink(&function_value->gc_status);
    return result->type;
}

ResultType setDecoration(DecorationStatement *ds, LinkValue *value, FUNC_NT) {
    LinkValue *decall = NULL;
    Parameter *pt = NULL;
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);
    for (PASS; ds != NULL; ds = ds->next){
        freeResult(result);
        if (operationSafeInterStatement(CFUNC(ds->decoration, var_list, result, belong)))
            break;
        pt = makeValueParameter(makeBaseLinkValueStatement(value, ds->decoration->line, ds->decoration->code_file));
        decall = result->value;
        result->value = NULL;

        freeResult(result);
        callBackCorePt(decall, pt, ds->decoration->line, ds->decoration->code_file, CNEXT_NT);
        gc_freeTmpLink(&decall->gc_status);
        freeParameter(pt, true);
        if (!CHECK_RESULT(result))
            break;

        gc_freeTmpLink(&value->gc_status);
        value = result->value;
        gc_addTmpLink(&value->gc_status);
    }
    gc_freeTmpLink(&value->gc_status);
    return result->type;
}