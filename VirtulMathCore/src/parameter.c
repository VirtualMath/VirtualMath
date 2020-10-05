#include "__run.h"


#define returnResult(result) do{ \
if (!CHECK_RESULT(result)) { \
goto return_; \
} \
}while(0)

Argument *makeArgument(void){
    Argument *tmp = memCalloc(1, sizeof(Argument));
    tmp->type = value_arg;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->data.name_ = NULL;
    tmp->name_type = name_st;
    tmp->next = NULL;
    return tmp;
}

Argument *makeValueArgument(LinkValue *value){
    Argument *tmp = makeArgument();
    tmp->data.value = value;
    gc_addTmpLink(&value->gc_status);
    return tmp;
}

Argument *makeStatementNameArgument(LinkValue *value, Statement *name){
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->data.value = value;
    tmp->data.name = name;
    gc_addTmpLink(&value->gc_status);
    return tmp;
}

Argument *makeCharNameArgument(LinkValue *value, LinkValue *name_value, wchar_t *name) {
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->name_type = name_char;
    tmp->data.value = value;
    tmp->data.name_ = memWidecpy(name);
    tmp->data.name_value = name_value;
    gc_addTmpLink(&value->gc_status);
    gc_addTmpLink(&name_value->gc_status);
    return tmp;
}

Argument *connectArgument(Argument *new, Argument *base){
    Argument *tmp = base;
    if (base == NULL)
        return new;
    for (PASS; base->next != NULL; base = base->next)
        PASS;
    base->next = new;
    return tmp;
}

Argument *connectValueArgument(LinkValue *value, Argument *base){
    Argument *new = makeValueArgument(value);
    return connectArgument(new, base);
}

Argument *connectStatementNameArgument(LinkValue *value, Statement *name, Argument *base){
    Argument *new = makeStatementNameArgument(value, name);
    return connectArgument(new, base);
}

Argument *connectCharNameArgument(LinkValue *value, LinkValue *name_value, wchar_t *name, Argument *base) {
    Argument *new = makeCharNameArgument(value, name_value, name);
    return connectArgument(new, base);
}

void freeArgument(Argument *at, bool free_st) {
    for (Argument *tmp=NULL; at != NULL;at = tmp){
        tmp = at->next;
        if (free_st)
            freeStatement(at->data.name);
        memFree(at->data.name_);

        if (at->data.name_value != NULL)
            gc_freeTmpLink(&at->data.name_value->gc_status);
        if (at->data.value != NULL)
            gc_freeTmpLink(&at->data.value->gc_status);

        memFree(at);
    }
}

Parameter *makeParameter(void){
    Parameter *tmp = memCalloc(1, sizeof(Parameter));
    tmp->type = value_par;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->data.is_sep = false;
    tmp->next = NULL;
    return tmp;
}

Parameter *copyenOneParameter(Parameter *base){
    Parameter *tmp = makeParameter();
    tmp->data.value = copyStatement(base->data.value);
    tmp->data.name = copyStatement(base->data.name);
    tmp->data.is_sep = base->data.is_sep;
    tmp->type = base->type;
    return tmp;
}

Parameter *copyParameter(Parameter *base){
    Parameter *base_tmp = NULL;
    Parameter **tmp = &base_tmp;
    for (PASS; base != NULL; tmp = &(*tmp)->next,base = base->next)
        *tmp = copyenOneParameter(base);
    return base_tmp;
}

Parameter *makeValueParameter(Statement *st){
    Parameter *tmp = makeParameter();
    tmp->data.value = st;
    return tmp;
}

Parameter *makeNameParameter(Statement *value, Statement *name){
    Parameter *tmp = makeParameter();
    tmp->type = name_par;
    tmp->data.value = value;
    tmp->data.name = name;
    return tmp;
}

Parameter *makeArgsParameter(Statement *st){
    Parameter *tmp = makeParameter();
    tmp->type = args_par;
    tmp->data.value = st;
    return tmp;
}

Parameter *makeKwrgsParameter(Statement *st){
    Parameter *tmp = makeParameter();
    tmp->type = kwargs_par;
    tmp->data.value = st;
    return tmp;
}

Parameter *connectParameter(Parameter *new, Parameter *base){
    if (base == NULL)
        return new;
    Parameter *tmp = base;
    while (base->next != NULL)
        base = base->next;
    base->next = new;
    return tmp;
}

Parameter *connectValueParameter(Statement *st, Parameter *base, bool is_sep) {
    Parameter *new = makeValueParameter(st);
    new->data.is_sep = is_sep;
    return connectParameter(new, base);
}

Parameter *connectNameParameter(Statement *value, Statement *name, Parameter *base){
    Parameter *new = makeNameParameter(value, name);
    return connectParameter(new, base);
}

Parameter *connectArgsParameter(Statement *st, Parameter *base, bool is_sep) {
    Parameter *new = makeArgsParameter(st);
    new->data.is_sep = is_sep;
    return connectParameter(new, base);
}

Parameter *connectKwargsParameter(Statement *st, Parameter *base){
    Parameter *new = makeKwrgsParameter(st);
    return connectParameter(new, base);
}

void freeParameter(Parameter *pt, bool free_st) {
    for (Parameter *tmp=NULL;pt != NULL;pt = tmp){
        tmp = pt->next;
        if (free_st) {
            freeStatement(pt->data.value);
            freeStatement(pt->data.name);
        }
        memFree(pt);
    }
}

Argument *listToArgument(LinkValue *list_value, long line, char *file, FUNC_NT){
    Argument *at = NULL;
    LinkValue *iter = NULL;
    setResultCore(result);
    getIter(list_value, 1, line, file, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return NULL;
    iter = result->value;
    result->value = NULL;
    while (true) {
        freeResult(result);
        getIter(iter, 0, line, file, CNEXT_NT);
        if (is_iterStop(result->value, inter)){
            freeResult(result);
            break;
        }
        else if (!CHECK_RESULT(result)) {
            freeArgument(at, true);
            at = NULL;
            goto return_;
        }
        at = connectValueArgument(result->value, at);
    }
    setResult(result, inter);
    return_:
    gc_freeTmpLink(&iter->gc_status);
    return at;
}

Argument *dictToArgument(LinkValue *dict_value, long line, char *file, FUNC_NT) {
    Argument *at = NULL;
    LinkValue *iter = NULL;
    setResultCore(result);
    getIter(dict_value, 1, line, file, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return NULL;
    iter = result->value;
    result->value = NULL;
    while (true) {
        LinkValue *name_ = NULL;
        wchar_t *name = NULL;

        freeResult(result);
        getIter(iter, 0, line, file, CNEXT_NT);
        if (is_iterStop(result->value, inter)){
            freeResult(result);
            break;
        }
        else if (!CHECK_RESULT(result)) {
            freeArgument(at, true);
            at = NULL;
            goto return_;
        }
        name_ = result->value;
        result->value = NULL;
        freeResult(result);

        getElement(iter, name_, line, file, CNEXT_NT);
        if (!CHECK_RESULT(result)) {
            gc_freeTmpLink(&name_->gc_status);
            freeArgument(at, true);
            at = NULL;
            goto return_;
        }
        name = getNameFromValue(name_->value, inter);
        at = connectCharNameArgument(result->value, name_, name, at);
        gc_freeTmpLink(&name_->gc_status);
        memFree(name);
    }
    setResult(result, inter);
    return_:
    gc_freeTmpLink(&iter->gc_status);
    return at;
}

/**
 * 设置形式参数的默认值
 * 仅支持name_value
 * @param function_ad
 * @param inter
 * @param var_list
 * @param num
 * @return
 */
ResultType defaultParameter(Parameter **function_ad, vint *num, FUNC_NT) {
    Parameter *function = *function_ad;
    setResultCore(result);

    for (*num = 0; function != NULL && function->type == name_par; (*num)++, function = function->next){
        LinkValue *value = NULL;
        if(operationSafeInterStatement(CFUNC(function->data.value, var_list, result, belong)))
            goto return_;

        value = result->value;
        result->value = NULL;
        freeResult(result);
        assCore(function->data.name, value, false, false, CNEXT_NT);
        gc_freeTmpLink(&value->gc_status);
        if (!CHECK_RESULT(result))
            goto return_;
        freeResult(result);
    }
    setResult(result, inter);

    return_:
    *function_ad = function;
    return result->type;
}

/**
 * 设置实际参数的默认值, 仅支持name_base_value
 * @param call_ad
 * @param inter
 * @param var_list
 * @param num
 * @return
 */
ResultType argumentToVar(Argument **call_ad, vint *num, FUNC_NT) {
    Argument *call = *call_ad;
    setResultCore(result);

    for (*num = 0; call != NULL && call->type == name_arg; (*num)++, call = call->next){
        if (call->name_type == name_char){
            addFromVarList(call->data.name_, call->data.name_value, 0, call->data.value, CFUNC_CORE(var_list));  // 参数赋值不需要处理变量式参数
            continue;
        }
        freeResult(result);
        assCore(call->data.name, call->data.value, false, false, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto return_;
    }
    setResult(result, inter);

    return_:
    *call_ad = call;
    return result->type;
}

/**
 * 形式参数从变量空间中获取值
 * @param function_ad
 * @param function_var
 * @param inter
 * @param var_list
 * @param num
 * @return
 */
ResultType parameterFromVar(Parameter **function_ad, VarList *function_var, vint *num, vint max, bool *status,
                            FUNC_NT) {
    Parameter *function = *function_ad;
    bool get = true;
    setResultCore(result);

    for (*num = 0, *status = false; function != NULL; function = function->next){
        int int_times;
        wchar_t *str_name = NULL;
        Statement *name = function->type == name_par ? function->data.name : function->data.value;
        LinkValue *value = NULL;
        get = true;

        if (function->type == kwargs_par){
            makeDictValue(NULL, false, LINEFILE, CNEXT_NT);
            if (!CHECK_RESULT(result))
                return result->type;

            value = result->value;
            result->value = NULL;
            freeResult(result);

            value->value->data.dict.dict = var_list->hashtable;
            value->value->data.dict.size = max - *num;
            *status = true;
            gc_freeTmpLink(&value->gc_status);
            goto not_return;
        }

        getVarInfo(&str_name, &int_times, CFUNC(name, var_list, result, belong));
        if (!CHECK_RESULT(result)) {
            memFree(str_name);
            *function_ad = function;
            return result->type;
        }

        freeResult(result);
        value = findFromVarList(str_name, int_times, del_var, CFUNC_CORE(var_list));  // 形式参数取值不需要执行变量式函数
        memFree(str_name);

        if(value == NULL) {
            get = false;
            if (function->type == name_par && !operationSafeInterStatement(CFUNC(function->data.value, var_list, result, belong))) {
                value = result->value;
                goto not_return;
            }
            setResultErrorSt(E_ArgumentException, FEW_ARG, true, name, CNEXT_NT);
            goto return_;
        }
        else if (!checkAut(name->aut, value->aut, name->line, name->code_file, NULL, false, CNEXT_NT))
            goto return_;

        not_return:
        freeResult(result);
        assCore(name, value, false, false, CFUNC_NT(function_var, result, belong));

        if (!CHECK_RESULT(result)) {
            *function_ad = function;
            return result->type;
        }
        freeResult(result);

        if (get)
            (*num)++;
    }

    setResult(result, inter);
    return_:
    *function_ad = function;
    return result->type;
}

/**
 * 对位赋值
 * @param call_ad
 * @param function_ad
 * @param function_var
 * @param inter
 * @param var_list
 * @return
 */
ResultType argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, FUNC_NT){
    Argument *call = *call_ad;
    Parameter *function = *function_ad;
    setResultCore(result);

    for (PASS; call != NULL && function != NULL && (call->type == value_arg) && function->type != args_par; call = call->next, function = function->next){
        Statement *name = function->type == value_par ? function->data.value : function->data.name;
        assCore(name, call->data.value, false, false, CFUNC_NT(function_var, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        freeResult(result);
    }

    setResult(result, inter);
    return_:
    *call_ad = call;
    *function_ad = function;
    return result->type;
}

/**
 * 把所有实际参数的值计算出来
 * @param call_ad
 * @param inter
 * @param var_list
 * @return
 */
ResultType iterParameter(Parameter *call, Argument **base_ad, bool is_dict, FUNC_NT){
    Argument *base = *base_ad;
    setResultCore(result);

    for (PASS; call != NULL; call = call->next){
        if(operationSafeInterStatement(CFUNC(call->data.value, var_list, result, belong)))
            goto return_;

        if (call->type == value_par)
            base = connectValueArgument(result->value, base);
        else if (call->type == name_par){
            if (is_dict){
                LinkValue *value = result->value;
                setResultCore(result);
                if(operationSafeInterStatement(CFUNC(call->data.name, var_list, result, belong))) {
                    gc_freeTmpLink(&value->gc_status);
                    goto return_;
                }
                wchar_t *name_str = getNameFromValue(result->value->value, inter);
                base = connectCharNameArgument(value, result->value, name_str, base);
                memFree(name_str);
                gc_freeTmpLink(&value->gc_status);
            }
            else
                base = connectStatementNameArgument(result->value, call->data.name, base);
        }
        else if (call->type == args_par){
            LinkValue *start = NULL;
            Argument *tmp_at = NULL;
            start = result->value;
            result->value = NULL;
            freeResult(result);
            tmp_at = listToArgument(start, LINEFILE, CNEXT_NT);
            gc_freeTmpLink(&start->gc_status);
            if (!CHECK_RESULT(result))
                goto return_;
            base = connectArgument(tmp_at, base);
        }
        else if (call->type == kwargs_par){
            LinkValue *start = NULL;
            Argument *tmp_at = NULL;
            start = result->value;
            result->value = NULL;
            freeResult(result);
            tmp_at = dictToArgument(start, LINEFILE, CNEXT_NT);
            gc_freeTmpLink(&start->gc_status);
            if (!CHECK_RESULT(result))
                goto return_;
            base = connectArgument(tmp_at, base);
        }
        freeResult(result);
    }
    setResult(result, inter);

    return_:
    *base_ad = base;
    return result->type;
}

Argument * getArgument(Parameter *call, bool is_dict, FUNC_NT) {
    Argument *new_arg = NULL;
    setResultCore(result);
    iterParameter(call, &new_arg, is_dict, CNEXT_NT);
    return new_arg;
}

bool checkIsSep(Parameter *pt) {
    for (PASS; pt != NULL; pt = pt->next) {
        if (pt->data.is_sep == true)
            return true;
    }
    return false;
}

/**
 * 参数表:
 |实参 \ 形参| name | value | arg | kwarg | null |
 ----------------------------------------
 |name     | p_3  |  p_3  | p_4 |  p_3! | error |
 |value    | p_1  |  p_1  | p_4 | error | error |
 |null     | p_2  | error | p_4 |  p_5  | okay  |
 ----------------------------------------
 * 注解: @p_1 match_status; @p_2 default_status; @p_3 self_ass; @p_4 mul_par; @p_5 pow_par; @p_3! 通过p_3处理**kwargs
 * @param call
 * @param function
 * @param func_var
 * @param inter
 * @param var_list
 * @return
 */
ResultType setParameterCore(fline line, char *file, Argument *call, Parameter *function_base, VarList *function_var,
                            FUNC_NT) {
    Parameter *function = NULL;
    Parameter *tmp_function = NULL;  // 释放使用
    enum {
        match_status = 1,
        default_status = 2,
        self_ass = 3,
        mul_par = 4,
        space_kwargs = 5,
        error_to_less = -1,
        error_to_more = -2,
        error_kw = -3,
        error_unknown = -4,
        finished = 0,
    } status;
    function = copyParameter(function_base);
    tmp_function = function;
    setResultCore(result);
    gc_freeze(inter, function_var, NULL, true);
    gc_freeze(inter, var_list, NULL, true);

    while (true){
        if (call == NULL && function == NULL)
            status = finished;
        else if (call != NULL && function == NULL)
            status = error_to_more;
        else if ((call == NULL && function->type == value_par))
            status = error_to_less;
        else if (call != NULL && call->type == value_par && function->type == kwargs_par)
            status = error_kw;
        else if (call == NULL && function->type == name_par)  // 根据前面的条件, 已经决定function不会为NULL
            status = default_status;
        else if (call == NULL && function->type == kwargs_par)
            status = space_kwargs;
        else if (function->type == args_par)
            status = mul_par;
        else if (call->type == value_arg)  // 根据前面的条件, 已经决定call不会为NULL
            status = match_status;
        else if (call->type == name_arg) {
            if (checkIsSep(function))
                status = error_to_less;
            else
                status = self_ass;
        } else
            status = error_unknown;

        freeResult(result);
        switch (status) {
            case match_status: {
                argumentToParameter(&call, &function, function_var, CNEXT_NT);
                returnResult(result);
                break;
            }
            case default_status: {
                vint num = 0;
                defaultParameter(&function, &num, CFUNC_NT(function_var, result, belong));
                returnResult(result);
                break;
            }
            case self_ass: {
                vint set_num = 0;
                vint get_num = 0;
                bool dict_status = false;
                VarList *tmp = makeVarList(inter, true);

                argumentToVar(&call, &set_num, CFUNC_NT(tmp, result, belong));
                if (!CHECK_RESULT(result)) {
                    freeVarList(tmp);
                    goto return_;
                }

                freeResult(result);
                parameterFromVar(&function, function_var, &get_num, set_num, &dict_status, CFUNC_NT(tmp, result, belong));
                freeVarList(tmp);
                if (!CHECK_RESULT(result))
                    goto return_;

                if (!dict_status && set_num > get_num)
                    goto to_more;
                break;
            }
            case mul_par: {
                LinkValue *tmp;

                if (call != NULL) {
                    Argument *backup;
                    Argument *base = call;
                    for (PASS; call->next != NULL && call->next->type == value_arg; call = call->next)
                            PASS;
                    backup = call->next;
                    call->next = NULL;
                    makeListValue(base, LINEFILE, L_tuple, CNEXT_NT);
                    call->next = backup;
                    call = backup;
                } else
                    makeListValue(NULL, LINEFILE, L_tuple, CNEXT_NT);

                returnResult(result);
                tmp = result->value;
                result->value = NULL;
                freeResult(result);

                assCore(function->data.value, tmp, false, false, CFUNC_NT(function_var, result, belong));
                gc_freeTmpLink(&tmp->gc_status);
                returnResult(result);
                function = function->next;
                break;
            }
            case space_kwargs:{
                LinkValue *tmp;
                makeDictValue(NULL, true, LINEFILE, CNEXT_NT);
                returnResult(result);
                tmp = result->value;
                result->value = NULL;
                freeResult(result);

                assCore(function->data.value, tmp, false, false, CFUNC_NT(function_var, result, belong));
                gc_freeTmpLink(&tmp->gc_status);
                returnResult(result);
                function = function->next;
                break;
            }
            case error_to_less:
                setResultError(E_ArgumentException, FEW_ARG, line, file, true, CNEXT_NT);
                goto return_;
            case error_to_more:
            to_more:
                setResultError(E_ArgumentException, MANY_ARG, line, file, true, CNEXT_NT);
                goto return_;
            case error_kw:
                setResultError(E_ArgumentException, OBJ_NOTSUPPORT(**), line, file, true, CNEXT_NT);
                goto return_;
            case error_unknown:
                setResultError(E_ArgumentException, L"Argument Unknown Exception", line, file, true, CNEXT_NT);
                goto return_;
            default:
                goto break_;
        }
    }
    break_:
    setResult(result, inter);

    return_:
    gc_freeze(inter, function_var, NULL, false);
    gc_freeze(inter, var_list, NULL, false);
    freeParameter(tmp_function, true);
    return result->type;
}

Inherit *setFather(Argument *call) {
    Inherit *father_tmp = NULL;
    for (Argument *tmp = call; tmp != NULL && tmp->type == value_arg; tmp = tmp->next)
        if (tmp->data.value->value->type == V_class) {
            father_tmp = connectInherit(father_tmp, makeInherit(tmp->data.value));
            father_tmp = connectInherit(father_tmp, copyInherit(tmp->data.value->value->object.inherit));
        }
    return setFatherCore(father_tmp);
}

Inherit *setFatherCore(Inherit *father_tmp) {
    Inherit *base_father = NULL;
    while (father_tmp != NULL){
        Inherit *next = father_tmp->next;
        father_tmp->next = NULL;
        base_father = connectSafeInherit(base_father, father_tmp);
        father_tmp = next;
    }
    return base_father;
}

bool checkFormal(Parameter *pt) {
    enum {
        Formal_1,
        Formal_2,
    } status = Formal_1;
    for (PASS; pt != NULL; pt = pt->next){
        if (status == Formal_1 && (pt->type == name_par || pt->type == args_par))
                status = Formal_2;
        else if (status == Formal_2 && (pt->type == value_par || pt->type == args_par) || pt->type == kwargs_par && pt->next != NULL)
            return false;
    }
    return true;
}

int parserArgumentUnion(ArgumentParser ap[], Argument *arg, FUNC_NT){
    setResultCore(result);
    if (ap->type != only_name){
        ArgumentParser *bak = NULL;
        int status = 1;
        arg = parserValueArgument(ap, arg, &status, &bak);
        if (status != 1){
            setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
            return 0;
        }
        ap = bak;
    }
    if (ap->must != -1){
        ArgumentParser *bak = NULL;
        int status;

        if (arg != NULL && arg->type != name_arg) {
            setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
            return -6;
        }

        status = parserNameArgument(ap, arg, &bak, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return -1;
        if (status == -3){
            if (parserArgumentNameDefault(ap)->must != -1){
                setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
                return -7;
            }
        }
        else if (status == 0){
            setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
            return -2;
        } else if (status == -4){
            setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
            return -3;
        }
    } else{
        if (arg != NULL) {
            setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
            return -4;
        }
    }
    return 1;
}

Argument *parserValueArgument(ArgumentParser *ap, Argument *arg, int *status, ArgumentParser **bak){
    *status = 1;
    for (PASS; ap->must != -1 && (ap->type == only_value || ap->type == name_value); ap++){
        if (arg == NULL || arg->name_type != value_arg) {  // 形参进入key=value模式
            if ((ap = parserArgumentValueDefault(ap))->must != -1 && ap->type == only_value)  // 检查剩余的是否.must=1
                *status = 0;  // 存在.must=1则返回 0
            break;  // 正常情况返回 1
        }
        arg = parserArgumentValueCore(arg, ap);
    }
    if (bak != NULL)
        *bak = ap;
    return arg;
}

int parserNameArgument(ArgumentParser ap[], Argument *arg, ArgumentParser **bak, FUNC_NT){
    VarList *tmp = NULL;
    vint set_num = 0;
    vint get_num = 0;
    int return_;
    setResultCore(result);

    gc_freeze(inter, var_list, NULL, true);
    for (PASS; arg != NULL && arg->type != name_arg; arg = arg->next)
        PASS;
    if (arg == NULL) {
        return_ = -3;  // 参数缺失
        goto return_;
    }

    tmp = makeVarList(inter, true);
    argumentToVar(&arg, &set_num, CFUNC_NT(tmp, result, belong));
    if (!CHECK_RESULT(result)) {
        return_ = -1;
        goto return_;
    }
    setResult(result, inter);

    for (PASS; ap->must != -1 && (ap->type == only_name || ap->type == name_value); ap++) {
        int status = parserArgumentVar(ap, inter, tmp);
        if (status == 1)
            get_num ++;
        else{
            return_ = -2;  // 参数缺失
            goto return_;
        }
    }
    return_ = (get_num < set_num) ? 0 : ((get_num > set_num) ? -4 : 1);

    return_:
    freeVarList(tmp);
    gc_freeze(inter, var_list, NULL, false);
    if (bak != NULL)
        *bak = ap;
    return return_;
}

Argument *parserArgumentValueCore(Argument *arg, ArgumentParser *ap){
    int count = 1;
    ap->arg = arg;
    ap->value = arg->data.value;
    arg = arg->next;
    if (ap->long_arg)
        for (PASS; arg != NULL && arg->type == value_arg; arg = arg->next, count++)
            PASS;
    ap->c_count = count;
    return arg;
}

int parserArgumentVar(ArgumentParser *ap, Inter *inter, VarList *var_list){
    LinkValue *value = NULL;
    findStrVarOnly(ap->name, false, CFUNC_CORE(var_list));  // 参数取值不执行变量式函数
    ap->value = value;
    if (value != NULL)
        return 1;
    else if (ap->must)
        return -1;
    return 0;
}

ArgumentParser *parserArgumentValueDefault(ArgumentParser *ap){
    for (PASS; ap->type == only_value && ap->must == 0; ap++) {
        ap->arg = NULL;
        ap->value = NULL;
        ap->c_count = 0;
    }
    return ap;
}

ArgumentParser *parserArgumentNameDefault(ArgumentParser *ap){
    for (PASS; ap->must == 0; ap++) {
        ap->arg = NULL;
        ap->value = NULL;
        ap->c_count = 0;
    }
    return ap;
}

void setArgumentFFICore(ArgumentFFI *af) {
    af->type = NULL;
    af->arg = NULL;
    af->arg_v = NULL;
    af->size = 0;
}

void setArgumentFFI(ArgumentFFI *af, unsigned int size) {
    af->type = memCalloc((size_t)size, sizeof(enum ArgumentFFIType));
    af->arg = memCalloc((size_t)size, sizeof(ffi_type *));
    af->arg_v = memCalloc((size_t)size, sizeof(void *));
    af->size = size;
    memset(af->type, 0, (size_t)size);
    memset(af->arg, 0, (size_t)size);
    memset(af->arg_v, 0, (size_t)size);
}

void freeArgumentFFI(ArgumentFFI *af) {
    for (unsigned int i=0; i < af->size; i++) {
        switch (af->type[i]) {
            case af_double:
            case af_int:
                memFree(af->arg_v[i]);
                break;
            case af_str:
                if (af->arg_v[i] != NULL)
                    memFree(*(char **)af->arg_v[i]);
                memFree(af->arg_v[i]);
                break;
            default:
                break;
        }
    }

    memFree(af->type);
    memFree(af->arg);
    memFree(af->arg_v);
}

unsigned int checkArgument(Argument *arg, enum ArgumentType type) {
    unsigned int count;
    for (count = 0; arg != NULL; arg = arg->next, count ++) {
        if (arg->type != type)
            return -1;
    }
    return count;
}

bool setArgumentToFFI(ArgumentFFI *af, Argument *arg) {
    for (unsigned int i=0; arg != NULL && i < af->size; arg = arg->next, i++) {
        if (af->arg[i] == NULL) {
            switch (arg->data.value->value->type) {
                case V_int:
                    af->arg[i] = &ffi_type_sint;  // af->arg是ffi_type **arg, 即*arg[]
                    af->type[i] = af_int;

                    af->arg_v[i] = (int *)memCalloc(1, sizeof(int));  // af->arg_v是ffi_type **arg_v, 即 *arg_v[]
                    *(int *)(af->arg_v[i]) = (int)arg->data.value->value->data.int_.num;
                    break;
                case V_dou:
                    af->arg[i] = &ffi_type_double;  // af->arg是ffi_type **arg, 即*arg[]
                    af->type[i] = af_double;

                    af->arg_v[i] = (double *)memCalloc(1, sizeof(double));  // af->arg_v是ffi_type **arg_v, 即 *arg_v[]
                    *(double *)(af->arg_v[i]) = (double)arg->data.value->value->data.dou.num;
                    break;
                case V_str:
                    af->arg[i] = &ffi_type_pointer;  // af->arg是ffi_type **arg, 即*arg[]
                    af->type[i] = af_str;

                    af->arg_v[i] = (char **)memCalloc(1, sizeof(char **));  // af->arg_v是ffi_type **arg_v, 即 *arg_v[]
                    *(char **)(af->arg_v[i]) = (char *)memWcsToStr(arg->data.value->value->data.str.str, false);
                    break;
                default:
                    return false;
            }
        } else
            return false;
    }
    return arg == NULL ? true : false;
}

ffi_type *getFFIType(wchar_t *str, enum ArgumentFFIType *aft) {
    ffi_type *return_ = NULL;
    if (eqWide(str, L"int")) {
        return_ = &ffi_type_sint;
        *aft = af_int;
    } else if (eqWide(str, L"dou")) {
        return_ = &ffi_type_double;
        *aft = af_double;
    } else if (eqWide(str, L"str")) {
        return_ = &ffi_type_pointer;
        *aft = af_str;
    } else if (eqWide(str, L"void")) {
        return_ = &ffi_type_void;
        *aft = af_void;
    } else if (eqWide(str, L"char")) {
        return_ = &ffi_type_schar;
        *aft = af_char;
    }
    return return_;
}
