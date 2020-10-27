#include "__run.h"

ResultType setClass(FUNC) {
    Argument *call = NULL;
    LinkValue *tmp = NULL;
    Inherit *class_inherit = NULL;
    setResultCore(result);

    call = getArgument(st->u.set_class.father, false, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;

    class_inherit = setFather(call);
    freeArgument(call, false);

    tmp = makeLinkValue(makeClassValue(var_list, inter, class_inherit), belong, auto_aut, inter);
    gc_freeTmpLink(&tmp->value->gc_status);
    gc_addTmpLink(&tmp->gc_status);
    freeResult(result);

    {
        enum FunctionPtType pt_type_bak = inter->data.default_pt_type;
        VarList *var_backup = tmp->value->object.var->next;
        inter->data.default_pt_type = fp_obj;
        tmp->value->object.var->next = var_list;

        // 运行类定义的时候需要调整belong
        functionSafeInterStatement(CFUNC(st->u.set_class.st, tmp->value->object.var, result, tmp));

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
        GET_RESULT(tmp, result);
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
    GET_RESULT(func, result);

    {
        enum FunctionPtType pt_type_bak = inter->data.default_pt_type;
        VarList *var_backup = func->value->object.var->next;
        inter->data.default_pt_type = fp_obj;
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
        GET_RESULT(func, result);
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
    GET_RESULT(element, result);

    func_name = st->u.slice_.type == SliceType_down_ ? inter->data.mag_func[M_DOWN] : inter->data.mag_func[M_SLICE];
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
    LinkValue *func = NULL;
    setResultCore(result);
    if (operationSafeInterStatement(CFUNC(st->u.call_function.function, var_list, result, belong)))
        return result->type;
    GET_RESULT(func, result);
    callBackCorePt(func, st->u.call_function.parameter, st->line, st->code_file, CNEXT_NT);
    setResultErrorSt(E_BaseException, NULL, false, st, CNEXT_NT);  // 显式执行函数才进行错误回溯
    gc_freeTmpLink(&func->gc_status);
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

    _new_ = findAttributes(inter->data.mag_func[M_NEW], false, LINEFILE, true, CFUNC_NT(var_list, result, class_value));
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

    _call_ = findAttributes(inter->data.mag_func[M_CALL], false, LINEFILE, true, CFUNC_NT(var_list, result, object_value));
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

static ResultType callCFunction(LinkValue *func_value, Argument *arg, long int line, char *file, int pt_sep, FUNC_NT){
    VarList *function_var = NULL;
    OfficialFunction of = NULL;
    Argument *bak;
    bool push = func_value->value->data.function.function_data.push;
    setResultCore(result);
    gc_addTmpLink(&func_value->gc_status);

    setFunctionArgument(&arg, &bak, func_value, line, file, pt_sep, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;

    of = func_value->value->data.function.of;
    if (push)
        function_var = pushVarList(func_value->value->object.out_var != NULL ? func_value->value->object.out_var : var_list, inter);
    else
        function_var = func_value->value->object.out_var != NULL ? func_value->value->object.out_var : var_list;
    freeResult(result);
    of(CO_FUNC(arg, function_var, result, func_value->belong));  // belong设置为func的belong, 方便权限的认定
    if (result->type == R_func)
        result->type = R_opt;
    else if (result->type != R_opt && result->type != R_error)
        setResult(result, inter);

    if (push)
        popVarList(function_var);
    freeFunctionArgument(arg, bak);

    return_:
#if START_GC
    if (SHOULD_RUNGC(inter))
        gc_run(inter, var_list);
#endif
    gc_freeTmpLink(&func_value->gc_status);
    return result->type;
}

static bool makeFFIReturn(enum ArgumentFFIType af, void **re_v) {
    switch (af) {
        case af_uchar:
        case af_usint:
        case af_uint:
        case af_ulint:
            *re_v = memCalloc(1, sizeof(uint64_t));  // 无论是int32或者是int64，都申请int64_t的内存 (否则libffi会提升类型，导致内存溢出)
            break;

        case af_char:
        case af_sint:
        case af_int:
        case af_lint:
            *re_v = memCalloc(1, sizeof(int64_t));  // 无论是int32或者是int64，都申请int64_t的内存 (否则libffi会提升类型，导致内存溢出)
            break;

        case af_float:
        case af_double:
            *re_v = memCalloc(1, sizeof(double));  // 理由同上
            break;

        case af_ldouble:
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
        case af_uint:
        case af_ulint:
            makeIntValue((vint)*(uint64_t *)re_v, line, file, CNEXT_NT);  // 先以(int64_t)读取void *类型的数据, 再转换成(vint)类型 (避免大端和小端模式的行为不同)
            break;

        case af_sint:
        case af_int:
        case af_lint:
            makeIntValue((vint)*(int64_t *)re_v, line, file, CNEXT_NT);  // 先以(int64_t)读取void *类型的数据, 再转换成(vint)类型 (避免大端和小端模式的行为不同)
            break;

        case af_float:
        case af_double:
            makeDouValue((vdou)*(double *)re_v, line, file, CNEXT_NT);
            break;

        case af_ldouble:
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
    LinkValue *vaarg_var = NULL;
    setResultCore(result);

    arg_var = findAttributes(L"funcargs", false, line, file, true, CFUNC_NT(var_list, result, function_value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    vaarg_var = findAttributes(L"vaargs", false, line, file, true, CFUNC_NT(var_list, result, function_value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (arg_var != NULL) {
        LinkValue **valist = vaarg_var != NULL ? vaarg_var->value->data.list.list : NULL;
        vint vasize = vaarg_var != NULL ? vaarg_var->value->data.list.size : 0;
        af->b_va = arg_var->value->data.list.size;

        if (arg_var->value->type != V_list || vaarg_var != NULL && vaarg_var->value->type != V_list) {
            setResultError(E_TypeException, ONLY_ACC(funcargs / vaarg_var, list), line, file, true, CNEXT_NT);
            return R_error;
        }

        if (!listToArgumentFFI(af, arg_var->value->data.list.list, arg_var->value->data.list.size, valist, vasize)) {
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
    if (function_value->value->data.function.function_data.cls->value->type != V_lib) {
        setResultError(E_ArgumentException, (wchar_t *) L"c function source is not clear", line, file, true, CNEXT_NT);
        return R_error;
    }

    if (function_value->value->data.function.function_data.cls->value->data.lib.handle == NULL) {
        setResultError(E_ArgumentException, (wchar_t *) L"dynamic library is closed", line, file, true, CNEXT_NT);
        return R_error;
    }

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

    if (af.size == af.b_va)
        ffi_prep_cif(&cif, FFI_DEFAULT_ABI, af.size, re, af.arg);
    else
        ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, af.b_va, af.size, re, af.arg);

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

static void newFunctionYield(Statement *func_st, Statement *node, bool push, VarList *new_var, Inter *inter){
    if (push)
        new_var->next = NULL;
    func_st->info.var_list = new_var;
    func_st->info.node = node->type == yield_code ? node->next : node;
    func_st->info.have_info = true;
    func_st->info.branch.func.push = push;
}

static void setFunctionResult(LinkValue *func_value, bool yield_run, bool push, Result *result, FUNC_CORE) {
    Statement *st_func = func_value->value->data.function.function;
    if (yield_run) {
        if (result->type == R_yield) {
            updateFunctionYield(st_func, result->node);
            result->type = R_opt;
            result->is_yield = true;
        } else {
            if (st_func->info.var_list != NULL && push)
                freeVarList(st_func->info.var_list);
            setRunInfo(st_func);
        }
    } else {
        if (result->type == R_yield) {
            newFunctionYield(st_func, result->node, push, var_list, inter);
            result->type = R_opt;
            result->is_yield = true;
        } else {
            if (push)
                popVarList(var_list);
        }
    }
}

static bool popFuncYieldVarList(Statement *st, VarList **return_, VarList *out_var, bool *push, Inter *inter) {
    bool yield_run;
    if ((yield_run = st->info.have_info)) {
        *push = st->info.branch.func.push;
        *return_ = st->info.var_list;
        if (*push)
            (*return_)->next = out_var;  // 若是push进来的var_list, 则需要重新链接
    } else if (*push)
        *return_ = pushVarList(out_var, inter);
    else
        *return_ = out_var;
    return yield_run;
}

static ResultType callVMFunction(LinkValue *func_value, Argument *arg, long int line, char *file, int pt_sep, FUNC_NT) {
    Argument *bak;
    VarList *var_func = NULL;
    Statement *st_func = NULL;
    Parameter *pt_func = func_value->value->data.function.pt;
    bool yield_run = false;
    bool push = func_value->value->data.function.function_data.push;
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
        yield_run = popFuncYieldVarList(st_func, &var_func, out_var, &push, inter);
    }

    if (yield_run)
        st_func = st_func->info.node;

    setFunctionArgument(&arg, &bak, func_value, line, file, pt_sep, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    setParameterCore(line, file, arg, pt_func, var_func, CFUNC_NT(var_list, result, func_value));
    freeFunctionArgument(arg, bak);

    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    functionSafeInterStatement(CFUNC(st_func, var_func, result, func_value->belong));  // belong设置为函数的belong，方便权限校对

    return_:
    setFunctionResult(func_value, yield_run, push, result, CFUNC_CORE(var_func));
    gc_freeTmpLink(&func_value->gc_status);
    return result->type;
}

ResultType callBackCore(LinkValue *function_value, Argument *arg, fline line, char *file, int pt_sep, FUNC_NT) {
    bool folding_bak[] = {inter->data.value_folding /*[0]*/, inter->data.var_folding /*[1]*/, inter->data.opt_folding /*[2]*/};
    bool set_folding = inter->data.func_folding;
    if (set_folding)
        inter->data.value_folding = inter->data.var_folding = inter->data.opt_folding = true;  // 全部启动
    setResultCore(result);
    gc_addTmpLink(&function_value->gc_status);
    switch (function_value->value->type) {
        case V_func:
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
                    break;
            }
            break;
        case V_class:
            callClass(function_value, arg, line, file, pt_sep, CNEXT_NT);
            break;
        default :
            callObject(function_value, arg, line, file, pt_sep, CNEXT_NT);
            break;
    }
    // callBackCore不执行错误回溯

    if (set_folding) {
        inter->data.value_folding = folding_bak[0];
        inter->data.var_folding = folding_bak[1];
        inter->data.opt_folding = folding_bak[2];
    }
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
        GET_RESULT(decall, result);
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