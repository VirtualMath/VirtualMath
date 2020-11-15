#include "__ofunc.h"

static ResultType vm_setNowRunCore(O_FUNC, bool type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false},
                           {.must=-1}};
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

static ResultType vm_isnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), true);
}

static ResultType vm_disnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), false);
}

static ResultType vm_quit(O_FUNC){
    if (arg != NULL)
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
    else
        setResultError(E_QuitException, L"vmcore quit()", LINEFILE, true, CNEXT_NT);
    return R_error;
}

static ResultType vm_setAssert(O_FUNC, enum AssertRunType type){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.assert_run = type;
    setResult(result, inter);
    return result->type;
}

static ResultType vm_assertignore(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_ignore);
}

static ResultType vm_assertrun(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_run);
}

static ResultType vm_assertraise(O_FUNC){
    return vm_setAssert(CO_FUNC(arg, var_list, result, belong), assert_raise);
}

static ResultType vm_selfunCore(O_FUNC, bool type){
    LinkValue *function_value = NULL;
    ArgumentParser ap[] = {{.type=name_value, .name=L"func", .must=1, .long_arg=false},
                           {.must=-1}};
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
    } else if (!push && type) {
        if (function_value->value->object.out_var != NULL)
            function_value->value->object.out_var = popVarList(function_value->value->object.out_var);
    }

    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

static ResultType vm_selfun(O_FUNC){
    return vm_selfunCore(CO_FUNC(arg, var_list, result, belong), false);
}

static ResultType vm_nselfun(O_FUNC){
    return vm_selfunCore(CO_FUNC(arg, var_list, result, belong), true);
}

static ResultType vm_setfreemode(O_FUNC, bool type){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.free_mode = type;
    setResult(result, inter);
    return result->type;
}

static ResultType vm_freemode(O_FUNC){
    return vm_setfreemode(CO_FUNC(arg, var_list, result, belong), true);
}

static ResultType vm_nfreemode(O_FUNC){
    return vm_setfreemode(CO_FUNC(arg, var_list, result, belong), false);
}

static ResultType vm_opt_mode(O_FUNC, enum OptMode mode){
    setResultCore(result);
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    inter->data.opt_mode = mode;
    setResult(result, inter);
    return result->type;
}

static ResultType vm_free_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_free);
}

static ResultType vm_normal_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_normal);
}

static ResultType vm_simple_opt(O_FUNC){
    return vm_opt_mode(CO_FUNC(arg, var_list, result, belong), om_simple);
}

static ResultType vm_exec(O_FUNC){
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
        } else if (pm->status != success) {
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

void registered(R_FUNC){
    NameFunc tmp[] = {{L"is_now_run",             vm_isnowrun,       fp_no_, .var=nfv_notpush},
                      {L"dis_now_run",            vm_disnowrun,      fp_no_, .var=nfv_notpush},
                      {L"quit",                   vm_quit,           fp_no_, .var=nfv_notpush},
                      {L"exec",                   vm_exec,           fp_no_, .var=nfv_notpush},
                      {L"assert_ignore",          vm_assertignore,   fp_no_, .var=nfv_notpush},
                      {L"assert_run",             vm_assertrun,      fp_no_, .var=nfv_notpush},
                      {L"assert_raise",           vm_assertraise,    fp_no_, .var=nfv_notpush},
                      {L"self_fun",               vm_selfun,         fp_no_, .var=nfv_notpush},
                      {L"no_self_fun",            vm_nselfun,        fp_no_, .var=nfv_notpush},
                      {L"free_mode",              vm_freemode,       fp_no_, .var=nfv_notpush},
                      {L"normal_mode",            vm_nfreemode,      fp_no_, .var=nfv_notpush},
                      {L"free_opt",               vm_free_opt,       fp_no_, .var=nfv_notpush},
                      {L"normal_opt",             vm_normal_opt,     fp_no_, .var=nfv_notpush},
                      {L"simple_opt",             vm_simple_opt,     fp_no_, .var=nfv_notpush},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}
