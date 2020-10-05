#include "__run.h"

ResultType setClass(FUNC) {
    Argument *call = NULL;
    LinkValue *tmp = NULL;
    Inherit *class_inherit = NULL;
    setResultCore(result);

    call = getArgument(st->u.set_class.father, false, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto error_return;

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
    error_return:
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
    if (CHECK_RESULT(result))  // 若没有出现错误则设定none
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

static bool makeFFIReturn(enum ArgumentFFIType af, void **re_v) {
    switch (af) {
        case af_char:
        case af_uchar:
        case af_usint:
        case af_sint:
        case af_uint:
        case af_int:
        case af_ulint:
        case af_lint:
            *re_v = memCalloc(1, sizeof(int64_t));  // 无论是int32或者是int64，都申请int64_t的内存 (否则libffi会提升类型，导致内存溢出)
            break;

        case af_float:
        case af_ldouble:
        case af_double:
            *re_v = memCalloc(1, sizeof(long double));  // 理由同上
            break;

        case af_pointer:
        case af_str:
        case af_wstr:
            *re_v = memCalloc(1, sizeof(void *));  // 所有指针数据大小都相同
            break;

        case af_void:
            *re_v = NULL;
            break;
        default:
            return false;
    }
    return true;
}

static bool FFIReturnValue(enum ArgumentFFIType aft, void *re_v, fline line, char *file, FUNC_NT) {
    switch (aft) {  // 应用返回值函数
        case af_usint:
        case af_sint:
        case af_uint:
        case af_int:
        case af_ulint:
        case af_lint:
            makeIntValue((vint)*(int64_t *)re_v, line, file, CNEXT_NT);  // 先以(int64_t)读取void *类型的数据, 再转换成(vint)类型 (避免大端和小端模式的行为不同)
            break;

        case af_float:
        case af_ldouble:
        case af_double:
            makeDouValue((vdou)*(long double *)re_v, line, file, CNEXT_NT);
            break;

        case af_str: {
            wchar_t *tmp = memStrToWcs(*(char **)re_v, false);
            makeStringValue(tmp, line, file, CNEXT_NT);
            memFree(tmp);
            break;
        }

        case af_wstr: {
            wchar_t *tmp = memWidecpy(*(wchar_t **)re_v);
            makeStringValue(tmp, line, file, CNEXT_NT);
            memFree(tmp);
            break;
        }

        case af_pointer:
            makePointerValue(*(void **)re_v, line, file, CNEXT_NT);
            break;

        case af_uchar:
        case af_char: {
            wchar_t tmp[] = {(wchar_t)(*(int64_t *)re_v), WNUL};
            makeStringValue(tmp, line, file, CNEXT_NT);
            break;
        }

        case af_void:
            setResult(result, inter);
            break;
        default:
            setResultError(E_ArgumentException, (wchar_t *) L"no-support return type for C function", line, file, true, CNEXT_NT);
            return false;
    }
    return true;
}

static ffi_type *getRearg(LinkValue *function_value, enum ArgumentFFIType *aft, fline line, char *file, FUNC_NT) {
    ffi_type *re;
    LinkValue *re_var = findAttributes(L"rearg", false, line, file, true, CFUNC_NT(var_list, result, function_value));
    if (!CHECK_RESULT(result))
        return NULL;
    freeResult(result);

    if (re_var != NULL) {
        if (re_var->value->type != V_str) {
            setResultError(E_TypeException, ONLY_ACC(rearg, str), line, file, true, CNEXT_NT);
            return NULL;
        }
        re = getFFIType(re_var->value->data.str.str, aft);
        if (re == NULL) {
            setResultError(E_ArgumentException, (wchar_t *) L"no-support argument type for C function", line, file, true, CNEXT_NT);
            return NULL;
        }
    } else
        re = &ffi_type_sint32;
    return re;
}

static ResultType getFuncargs(LinkValue *function_value, ArgumentFFI *af, fline line, char *file, FUNC_NT) {
    LinkValue *arg_var = NULL;
    setResultCore(result);

    arg_var = findAttributes(L"funcargs", false, line, file, true, CFUNC_NT(var_list, result, function_value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (arg_var != NULL) {
        if (arg_var->value->type != V_list) {
            setResultError(E_TypeException, ONLY_ACC(funcargs, list), line, file, true, CNEXT_NT);
            return R_error;
        }
        if (!listToArgumentFFI(af, arg_var->value->data.list.list, arg_var->value->data.list.size)) {
            setResultError(E_ArgumentException, (wchar_t *) L"no-support argument type for C function", line, file, true, CNEXT_NT);
            return R_error;
        }
    }
    return result->type;
}

static ResultType callFFunction(LinkValue *function_value, Argument *arg, long int line, char *file, int pt_sep, FUNC_NT){
    ffi_cif cif;
    ffi_type *re;
    unsigned int size;
    ArgumentFFI af;
    enum ArgumentFFIType aft = af_int;
    void *re_v = NULL;  // 存放返回值的函數

    setResultCore(result);
    setArgumentFFICore(&af);
    if (pt_sep != 0 || (size = checkArgument(arg, value_arg)) == -1) {
        setResultError(E_ArgumentException, (wchar_t *) L"does not support key-value arguments", line, file, true, CNEXT_NT);
        return R_error;
    }

    re = getRearg(function_value, &aft, line, file, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;

    setArgumentFFI(&af, size);
    getFuncargs(function_value, &af, line, file, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;

    if (!setArgumentToFFI(&af, arg)) {
        setResultError(E_ArgumentException, (wchar_t *) L"parameter exception when calling C function", line, file, true, CNEXT_NT);
        goto return_;
    }

    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, af.size, re, af.arg);
    if (makeFFIReturn(aft, &re_v)) {
        ffi_call(&cif, function_value->value->data.function.ffunc, re_v, af.arg_v);
        FFIReturnValue(aft, re_v, line, file, CNEXT_NT);
        memFree(re_v);
    } else
        setResultError(E_ArgumentException, (wchar_t *) L"no-support return type for C function", line, file, true, CNEXT_NT);

    return_:
    freeArgumentFFI(&af);
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
    if (function_value->value->type == V_func) {
        switch (function_value->value->data.function.type) {
            case vm_func:
                callVMFunction(function_value, arg, line, file, pt_sep, CNEXT_NT);
                break;
            case c_func:
                callCFunction(function_value, arg, line, file, pt_sep, CNEXT_NT);
                break;
            case f_func:
                callFFunction(function_value, arg, line, file, pt_sep, CNEXT_NT);
                break;
            default:
                setResultError(E_SystemException, L"function type error", line, file, true, CNEXT_NT);
                goto return_;
        }
    } else if (function_value->value->type == V_class)
        callClass(function_value, arg, line, file, pt_sep, CNEXT_NT);
    else
        callObject(function_value, arg, line, file, pt_sep, CNEXT_NT);
    setResultError(E_BaseException, NULL, line, file, false, CNEXT_NT);

    return_:
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