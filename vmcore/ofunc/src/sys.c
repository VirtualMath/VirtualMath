#include "__ofunc.h"

ResultType vm_super(O_FUNC){
    Value *arg_father = NULL;
    Value *arg_child = NULL;
    LinkValue *next_father = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"class_", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"obj_", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    arg_father = ap[0].value->value;
    arg_child = ap[1].value->value;
    if (arg_child == arg_father) {
        if (arg_child->object.inherit != NULL){
            result->value = COPY_LINKVALUE(arg_child->object.inherit->value, inter);
            result->type = R_opt;
            gc_addTmpLink(&result->value->gc_status);
        } else
            setResultError(E_SuperException, L"object has no next father", LINEFILE, true, CNEXT_NT);
        return result->type;
    }

    for (Inherit *self_father = arg_child->object.inherit; self_father != NULL; self_father = self_father->next) {
        if (self_father->value->value == arg_father) {
            if (self_father->next != NULL)
                next_father = COPY_LINKVALUE(self_father->next->value, inter);
            break;
        }
    }

    if (next_father != NULL){
        result->value = next_father;
        result->type = R_opt;
        gc_addTmpLink(&result->value->gc_status);
    }
    else
        setResultError(E_SuperException, L"object has no next father", LINEFILE, true, CNEXT_NT);

    return result->type;
}

ResultType vm_setNowRunCore(O_FUNC, bool type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false}, {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    function_value->value->data.function.function_data.run = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

ResultType vm_setMethodCore(O_FUNC, enum FunctionPtType type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false}, {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    function_value->value->data.function.function_data.pt_type = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

ResultType vm_cls(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls);
}

ResultType vm_func_cls(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls);
}

ResultType vm_no_(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_no_);
}

ResultType vm_func_(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_);
}

ResultType vm_func_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_class);
}

ResultType vm_func_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_obj);
}

ResultType vm_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_class);
}

ResultType vm_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_obj);
}

ResultType vm_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_all);
}

ResultType vm_func_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_all);
}

ResultType vm_cls_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_obj);
}

ResultType vm_cls_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_class);
}

ResultType vm_cls_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_all);
}

ResultType vm_func_cls_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_obj);
}

ResultType vm_func_cls_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_class);
}

ResultType vm_func_cls_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_all);
}

ResultType vm_isnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType vm_disnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), false);
}

ResultType vm_quit(O_FUNC){
    if (arg != NULL)
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
    else
        setResultError(E_QuitException, L"vmcore quit()", LINEFILE, true, CNEXT_NT);
    return R_error;
}

ResultType vm_open(O_FUNC){
    return callBackCore(inter->data.base_obj[B_FILE], arg, LINEFILE, 0, CNEXT_NT);
}

ResultType vm_setAssert(O_FUNC, enum AssertRunType type){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.assert_run = type;
    setResult(result, inter);
    return result->type;
}

ResultType vm_assertignore(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_ignore);
}

ResultType vm_assertrun(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_run);
}

ResultType vm_assertraise(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_raise);
}

ResultType vm_selfunCore(O_FUNC, bool type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false}, {.must=-1}};
    bool push;
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    function_value = ap[0].value;
    push = function_value->value->data.function.function_data.push;
    if (push && !type) {  // 原本是push, 现在设定为非push
        function_value->value->data.function.function_data.push = false;
        if (function_value->value->object.out_var != NULL)
            function_value->value->object.out_var = pushVarList(function_value->value->object.out_var, inter);
    } else if (!push && type){
        if (function_value->value->object.out_var != NULL)
            function_value->value->object.out_var = popVarList(function_value->value->object.out_var);
    }

    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

ResultType vm_selfun(O_FUNC){
    return vm_selfunCore(CO_FUNC(arg, var_list, result, belong), false);
}

ResultType vm_nselfun(O_FUNC){
    return vm_selfunCore(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType vm_setfreemode(O_FUNC, bool type){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.free_mode = type;
    setResult(result, inter);
    return result->type;
}

ResultType vm_freemode(O_FUNC){
    return vm_setfreemode(CO_FUNC(arg, var_list, result, belong), true);
}

ResultType vm_nfreemode(O_FUNC){
    return vm_setfreemode(CO_FUNC(arg, var_list, result, belong), false);
}

ResultType vm_opt_mode(O_FUNC, enum OptMode mode){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.opt_mode = mode;
    setResult(result, inter);
    return result->type;
}

ResultType vm_free_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_free);
}

ResultType vm_normal_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_normal);
}

ResultType vm_simple_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_simple);
}

ResultType vm_exec(O_FUNC){
    ArgumentParser ap[] = {{.type=name_value, .name=L"cm", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"var", .must=0, .long_arg=false},
                           {.type=name_value, .name=L"out", .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *str;
    LinkValue *var;
    LinkValue *out;
    Statement *new_st;
    VarList *run;

    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    str = ap[0].value;
    var = ap[1].value;
    out = ap[2].value;
    if (str->value->type != V_str) {
        setResultError(E_TypeException, ONLY_ACC(cm, str), LINEFILE, true, CNEXT_NT);
        return result->type;
    }

    if (var != NULL && var->value->type != V_dict) {
        setResultError(E_TypeException, ONLY_ACC(var, dict), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    if (out != NULL) {
        if (out->value->type != V_bool) {
            setResultError(E_TypeException, ONLY_ACC(out, bool), LINEFILE, true, CNEXT_NT);
            return R_error;
        } else if (var == NULL) {
            setResultError(E_TypeException, L"missing parameters: var", LINEFILE, true, CNEXT_NT);
            return R_error;
        }
    } else
        out = false;

    {
        ParserMessage *pm = makeParserMessageStr(str->value->data.str.str, false);
        new_st = makeStatement(0, "exec");
        parserCommandList(pm, inter, true, new_st);

        if (pm->status == int_error) {
            setResultError(E_KeyInterrupt, KEY_INTERRUPT, LINEFILE, true, CNEXT_NT);
            return R_error;
        }
        else if (pm->status != success) {
            wchar_t *wcs_message = memStrToWcs(pm->status_message, false);
            setResultError(E_TypeException, wcs_message, LINEFILE, true, CNEXT_NT);
            memFree(wcs_message);
            return R_error;
        }

        freeParserMessage(pm, true);
    }

    if (var != NULL) {
        run = makeVarList(inter, false, var->value->data.dict.dict);
        if (out)
            run->next = var_list;
    } else
        run = var_list;

    includeSafeInterStatement(CFUNC(new_st, run, result, belong));
    freeStatement(new_st);

    if (var != NULL)
        freeVarList(run);

    return result->type;
}

void registeredSysFunction(R_FUNC){
    NameFunc tmp[] = {{L"super",              vm_super,        fp_no_, .var=nfv_notpush},
                      {L"static_method",         vm_no_,          fp_no_, .var=nfv_notpush},
                      {L"func_method",       vm_func_,        fp_no_, .var=nfv_notpush},
                      {L"func_class_method",  vm_func_class,   fp_no_, .var=nfv_notpush},
                      {L"func_obj_method", vm_func_obj,     fp_no_, .var=nfv_notpush},
                      {L"class_method",        vm_class,        fp_no_, .var=nfv_notpush},
                      {L"obj_method",       vm_obj,          fp_no_, .var=nfv_notpush},
                      {L"simple_method",       vm_all,          fp_no_, .var=nfv_notpush},
                      {L"func_simple_method", vm_func_all,     fp_no_, .var=nfv_notpush},
                      {L"cls_method",          vm_cls,          fp_no_, .var=nfv_notpush},
                      {L"func_cls_method",    vm_func_cls,     fp_no_, .var=nfv_notpush},
                      {L"cls_obj_method", vm_cls_obj,     fp_no_, .var=nfv_notpush},
                      {L"cls_class_method",          vm_cls_class,          fp_no_, .var=nfv_notpush},
                      {L"cls_simple_method",    vm_cls_all,     fp_no_, .var=nfv_notpush},
                      {L"func_cls_obj_method", vm_func_cls_obj,     fp_no_, .var=nfv_notpush},
                      {L"func_cls_obj_method",          vm_func_cls_class,          fp_no_, .var=nfv_notpush},
                      {L"func_cls_simple_method",    vm_func_cls_all,     fp_no_, .var=nfv_notpush},

                      {L"is_now_run",           vm_isnowrun,     fp_no_, .var=nfv_notpush},
                      {L"dis_now_run",          vm_disnowrun,    fp_no_, .var=nfv_notpush},
                      {L"quit",               vm_quit,         fp_no_, .var=nfv_notpush},
                      {L"exec",               vm_exec,         fp_no_, .var=nfv_notpush},
                      {L"open",               vm_open,         fp_no_, .var=nfv_notpush},
                      {L"assert_ignore",      vm_assertignore, fp_no_, .var=nfv_notpush},
                      {L"assert_run",         vm_assertrun,    fp_no_, .var=nfv_notpush},
                      {L"assert_raise",       vm_assertraise,  fp_no_, .var=nfv_notpush},
                      {L"self_fun",             vm_selfun,           fp_no_, .var=nfv_notpush},
                      {L"no_self_fun",            vm_nselfun,          fp_no_, .var=nfv_notpush},
                      {L"free_mode", vm_freemode,                 fp_no_, .var=nfv_notpush},
                      {L"normal_mode", vm_nfreemode,              fp_no_, .var=nfv_notpush},
                      {L"free_opt", vm_free_opt,                  fp_no_, .var=nfv_notpush},
                      {L"normal_opt", vm_normal_opt,              fp_no_, .var=nfv_notpush},
                      {L"simple_opt", vm_simple_opt,              fp_no_, .var=nfv_notpush},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}
