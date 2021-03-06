#include "__ofunc.h"

static ResultType vm_super(O_FUNC){
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
        if (arg_child->object.inherit != NULL) {
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

    if (next_father != NULL) {
        result->value = next_father;
        result->type = R_opt;
        gc_addTmpLink(&result->value->gc_status);
    } else
        setResultError(E_SuperException, L"object has no next father", LINEFILE, true, CNEXT_NT);

    return result->type;
}

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

static ResultType vm_setMethodCore(O_FUNC, enum FunctionPtType type){
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
    function_value->value->data.function.function_data.pt_type = type;
    result->value = function_value;
    gc_addTmpLink(&result->value->gc_status);
    result->type = R_opt;
    return R_opt;
}

static ResultType vm_cls(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls);
}

static ResultType vm_func_cls(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls);
}

static ResultType vm_no_(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_no_);
}

static ResultType vm_func_(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_);
}

static ResultType vm_func_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_class);
}

static ResultType vm_func_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_obj);
}

static ResultType vm_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_class);
}

static ResultType vm_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_obj);
}

static ResultType vm_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_all);
}

static ResultType vm_func_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_all);
}

static ResultType vm_cls_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_obj);
}

static ResultType vm_cls_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_class);
}

static ResultType vm_cls_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_cls_all);
}

static ResultType vm_func_cls_obj(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_obj);
}

static ResultType vm_func_cls_class(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_class);
}

static ResultType vm_func_cls_all(O_FUNC){
    return vm_setMethodCore(CO_FUNC(arg, var_list, result, belong), fp_func_cls_all);
}

static ResultType vm_isnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), true);
}

static ResultType vm_disnowrun(O_FUNC){
    return vm_setNowRunCore(CO_FUNC(arg, var_list, result, belong), false);
}

static ResultType vm_open(O_FUNC){
    return callBackCore(inter->data.base_obj[B_FILE], arg, LINEFILE, 0, CNEXT_NT);
}

static ResultType vm_getValuePointer(O_FUNC){
    ArgumentParser ap[] = {{.type=name_value, .name=L"value", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    makePointerValue((void *)ap[0].value->value, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType vm_getLinkValuePointer(O_FUNC){
    ArgumentParser ap[] = {{.type=name_value, .name=L"link", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    makePointerValue((void *)ap[0].value, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType vm_quit(O_FUNC){
    if (arg != NULL)
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
    else
        setResultError(E_QuitException, L"vmcore quit()", LINEFILE, true, CNEXT_NT);
    return R_error;
}

static ResultType vm_raise(O_FUNC){
    ArgumentParser ap[] = {{.type=name_value, .name=L"signal", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    if (ap[0].value->value->type != V_int) {
        setResultError(E_TypeException, ONLY_ACC(signal, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    raise(ap[0].value->value->data.int_.num);
    setResult(result, inter);
    return result->type;
}

static ResultType vm_signal(O_FUNC){
    ArgumentParser ap[] = {{.type=name_value, .name=L"signal", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"func", .must=1, .long_arg=false},
                           {.must=-1}};
    SignalList *sl = inter->sig_list;
    SignalList *tmp;
    vsignal sig;
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    if (ap[0].value->value->type != V_int) {
        setResultError(E_TypeException, ONLY_ACC(signal, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    sig = ap[0].value->value->data.int_.num;
    if ((tmp = checkSignalList(sig, inter->sig_list)) == NULL) {
        inter->sig_list = makeSignalList(ap[0].value->value->data.int_.num, ap[1].value);  // 添加新的节点
        inter->sig_list->next = sl;  // 重新拼接
        setResult(result, inter);  // 返回值为null
    } else {
        LinkValue *old = exchangeSignalFunc(tmp, ap[1].value);  // 此处会为 old 添加 tmp_link
        setResultOperation(result, old);  // 更换函数, 旧函数作为返回值 （此处也会为 old 添加 tmp_link）
        gc_freeTmpLink(&old->gc_status);  // 释放掉 exchangeSignalFunc 为 old 添加到 tmp_link
    }
    signal(sig, vmSignalHandler);  // 绑定函数
    return result->type;
}

static ResultType vm_signal_core(O_FUNC, int status){
    ArgumentParser ap[] = {{.type=name_value, .name=L"signal", .must=1, .long_arg=false},
                           {.must=-1}};
    vsignal sig;
    LinkValue *old;
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    if (ap[0].value->value->type != V_int) {
        setResultError(E_TypeException, ONLY_ACC(signal, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    sig = ap[0].value->value->data.int_.num;
    if ((old = delSignalList(sig, &(inter->sig_list))) != NULL) {
        setResultOperation(result, old);
        gc_freeTmpLink(&old->gc_status);  // 释放 delSignalList 中设置 old 的 tmp_link
    } else
        setResult(result, inter);  // 默认返回none
    switch (status) {
        default:
        case 1:
            signal(sig, SIG_IGN);  // vm 不捕捉该信号
            break;
        case 2:
            signal(sig, vmSignalHandler);  // 用 vm 的默认方式处理信号
            break;
        case 3:
            signal(sig, SIG_ERR);  // 按错误处理
            break;
    }
    return result->type;
}

static ResultType vm_signal_ignore(O_FUNC) {
    return vm_signal_core(CO_FUNC(arg, var_list, result, belong), 1);
}

static ResultType vm_signal_default(O_FUNC) {
    return vm_signal_core(CO_FUNC(arg, var_list, result, belong), 2);
}

static ResultType vm_signal_err(O_FUNC) {
    return vm_signal_core(CO_FUNC(arg, var_list, result, belong), 3);
}

void registeredSysFunction(R_FUNC){
    NameFunc tmp[] = {{L"super",                  vm_super,          fp_no_, .var=nfv_notpush},
                      {L"static_method",          vm_no_,            fp_no_, .var=nfv_notpush},
                      {L"func_method",            vm_func_,          fp_no_, .var=nfv_notpush},
                      {L"func_class_method",      vm_func_class,     fp_no_, .var=nfv_notpush},
                      {L"func_obj_method",        vm_func_obj,       fp_no_, .var=nfv_notpush},
                      {L"class_method",           vm_class,          fp_no_, .var=nfv_notpush},
                      {L"obj_method",             vm_obj,            fp_no_, .var=nfv_notpush},
                      {L"simple_method",          vm_all,            fp_no_, .var=nfv_notpush},
                      {L"func_simple_method",     vm_func_all,       fp_no_, .var=nfv_notpush},
                      {L"cls_method",             vm_cls,            fp_no_, .var=nfv_notpush},
                      {L"func_cls_method",        vm_func_cls,       fp_no_, .var=nfv_notpush},
                      {L"cls_obj_method",         vm_cls_obj,        fp_no_, .var=nfv_notpush},
                      {L"cls_class_method",       vm_cls_class,      fp_no_, .var=nfv_notpush},
                      {L"cls_simple_method",      vm_cls_all,        fp_no_, .var=nfv_notpush},
                      {L"func_cls_obj_method",    vm_func_cls_obj,   fp_no_, .var=nfv_notpush},
                      {L"func_cls_obj_method",    vm_func_cls_class, fp_no_, .var=nfv_notpush},
                      {L"func_cls_simple_method", vm_func_cls_all,   fp_no_, .var=nfv_notpush},

                      {L"is_now_run",             vm_isnowrun,       fp_no_, .var=nfv_notpush},
                      {L"dis_now_run",            vm_disnowrun,      fp_no_, .var=nfv_notpush},
                      {L"open",                   vm_open,           fp_no_, .var=nfv_notpush},

                      {L"getValuePointer",        vm_getValuePointer,fp_no_, .var=nfv_notpush},
                      {L"getLinkValuePointer",    vm_getLinkValuePointer,fp_no_, .var=nfv_notpush},

                      {L"quit",                   vm_quit,           fp_no_, .var=nfv_notpush},
                      {L"raise",                  vm_raise,          fp_no_, .var=nfv_notpush},
                      {L"signal",                 vm_signal,         fp_no_, .var=nfv_notpush},
                      {L"signal_ignore",          vm_signal_ignore,  fp_no_, .var=nfv_notpush},
                      {L"signal_default",         vm_signal_default, fp_no_, .var=nfv_notpush},
                      {L"signal_err",             vm_signal_err,     fp_no_, .var=nfv_notpush},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}
