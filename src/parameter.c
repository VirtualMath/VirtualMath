#include "__virtualmath.h"

#define returnResult(result) do{ \
if (!run_continue(result)) { \
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

Argument *makeCharNameArgument(LinkValue *value, LinkValue *name_value, char *name) {
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->name_type = name_char;
    tmp->data.value = value;
    tmp->data.name_ = memStrcpy(name);
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

Argument *connectCharNameArgument(LinkValue *value, LinkValue *name_value, char *name, Argument *base) {
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
    tmp->next = NULL;
    return tmp;
}

Parameter *copyenOneParameter(Parameter *base){
    Parameter *tmp = makeParameter();
    tmp->data.value = copyStatement(base->data.value);
    tmp->data.name = copyStatement(base->data.name);
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

Parameter *connectValueParameter(Statement *st, Parameter *base){
    Parameter *new = makeValueParameter(st);
    return connectParameter(new, base);
}

Parameter *connectNameParameter(Statement *value, Statement *name, Parameter *base){
    Parameter *new = makeNameParameter(value, name);
    return connectParameter(new, base);
}

Parameter *connectArgsParameter(Statement *st, Parameter *base){
    Parameter *new = makeArgsParameter(st);
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

Argument *listToArgument(LinkValue *list_value, INTER_FUNCTIONSIG_CORE){
    Argument *at = NULL;
    for (int i=0;i<list_value->value->data.list.size;i++)
        at = connectValueArgument(list_value->value->data.list.list[i], at);
    return at;
}

Argument *dictToArgument(LinkValue *dict_value, INTER_FUNCTIONSIG_CORE){
    Argument *at = NULL;
    Var *tmp = NULL;
    for (int i = 0; i < MAX_SIZE; i++)
        for (tmp = dict_value->value->data.dict.dict->hashtable[i]; tmp != NULL; tmp = tmp->next)
            at = connectCharNameArgument(tmp->value, tmp->name_, tmp->name, at);
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
ResultType defaultParameter(Parameter **function_ad, NUMBER_TYPE *num, INTER_FUNCTIONSIG_NOT_ST) {
    Parameter *function = *function_ad;
    setResultCore(result);

    for (*num = 0; function != NULL && function->type == name_par; (*num)++, function = function->next){
        LinkValue *value = NULL;
        if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(function->data.value, var_list, result, father)))
            goto return_;

        value = result->value;
        freeResult(result);
        assCore(function->data.name, value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
        if (!run_continue(result))
            goto return_;
    }
    setResult(result, inter, father);

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
ResultType argumentToVar(Argument **call_ad, NUMBER_TYPE *num, INTER_FUNCTIONSIG_NOT_ST) {
    Argument *call = *call_ad;
    setResultCore(result);

    for (*num = 0; call != NULL && call->type == name_arg; (*num)++, call = call->next){
        if (call->name_type == name_char){
            addFromVarList(call->data.name_, call->data.name_value, 0, call->data.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            continue;
        }
        freeResult(result);
        assCore(call->data.name, call->data.value, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
        if (!run_continue(result))
            goto return_;
    }
    setResult(result, inter, father);

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
ResultType parameterFromVar(Parameter **function_ad, VarList *function_var, NUMBER_TYPE *num, NUMBER_TYPE max, bool *status,
                            INTER_FUNCTIONSIG_NOT_ST) {
    Parameter *function = *function_ad;
    bool get = true;
    setResultCore(result);

    for (*num = 0, *status = false; function != NULL; function = function->next){
        int int_times;
        char *str_name = NULL;
        Statement *name = function->type == name_par ? function->data.name : function->data.value;
        LinkValue *value = NULL;
        get = true;

        if (function->type == kwargs_par){
            value = makeLinkValue(makeDictValue(NULL, false, father, NULL, inter, var_list), father, inter);
            value->value->data.dict.dict = var_list->hashtable;
            value->value->data.dict.size = max - *num;
            *status = true;
            goto not_return;
        }

        freeResult(result);
        getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result, father));
        if (!run_continue(result)) {
            memFree(str_name);
            *function_ad = function;
            return result->type;
        }

        freeResult(result);
        value = findFromVarList(str_name, int_times, true, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        memFree(str_name);

        if(value == NULL) {
            get = false;
            if (function->type == name_par && !operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(function->data.value, var_list, result, father))) {
                value = result->value;
                goto not_return;
            }
            setResultError(result, inter, "ArgumentException", "Too less Argument", name, father, true);
            goto reutnr_;
        }
        else if ((name->aut == public_aut || name->aut == auto_aut) && (value->aut != public_aut && value->aut != auto_aut)) {
            setResultError(result, inter, "PermissionsException", "Wrong Permissions: access Argument as public", name,
                           father, true);
            goto reutnr_;
        }
        else if ((name->aut == protect_aut) && (value->aut == private_aut)) {
            setResultError(result, inter, "PermissionsException", "Wrong Permissions: access variables as protect",
                           name, father, true);
            goto reutnr_;
        }

        not_return:
        freeResult(result);
        assCore(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST (function_var, result, father));

        if (!run_continue(result)) {
            *function_ad = function;
            return result->type;
        }
        else
            freeResult(result);

        if (get)
            (*num)++;
    }

    setResult(result, inter, father);
    reutnr_:
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
ResultType argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_NOT_ST){
    Argument *call = *call_ad;
    Parameter *function = *function_ad;
    setResultCore(result);

    for (PASS; call != NULL && function != NULL && (call->type == value_arg) && function->type != args_par; call = call->next, function = function->next){
        Statement *name = function->type == value_par ? function->data.value : function->data.name;
        assCore(name, call->data.value, CALL_INTER_FUNCTIONSIG_NOT_ST (function_var, result, father));
        if (!run_continue(result))
            goto return_;
        freeResult(result);
    }

    setResult(result, inter, father);
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
ResultType iterParameter(Parameter *call, Argument **base_ad, bool is_dict, INTER_FUNCTIONSIG_NOT_ST){
    Argument *base = *base_ad;
    setResultCore(result);

    for (PASS; call != NULL; call = call->next){
        if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(call->data.value, var_list, result, father)))
            goto return_;

        if (call->type == value_par)
            base = connectValueArgument(result->value, base);
        else if (call->type == name_par){
            if (is_dict){
                LinkValue *value = result->value;
                setResultCore(result);
                if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(call->data.name, var_list, result, father))) {
                    gc_freeTmpLink(&value->gc_status);
                    goto return_;
                }
                char *name_str = getNameFromValue(result->value->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
                base = connectCharNameArgument(value, result->value, name_str, base);
                memFree(name_str);
                gc_freeTmpLink(&value->gc_status);
            }
            else
                base = connectStatementNameArgument(result->value, call->data.name, base);
        }
        else if (call->type == args_par){
            Argument *tmp_at = listToArgument(result->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            base = connectArgument(tmp_at, base);
        }
        else if (call->type == kwargs_par){
            Argument *tmp_at = dictToArgument(result->value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            base = connectArgument(tmp_at, base);
        }
        freeResult(result);
    }
    setResult(result, inter, father);

    return_:
    *base_ad = base;
    return result->type;
}

Argument * getArgument(Parameter *call, bool is_dict, INTER_FUNCTIONSIG_NOT_ST) {
    Argument *new_arg = NULL;
    freeResult(result);

    iterParameter(call, &new_arg, is_dict, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    return new_arg;
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
 * @param function_var
 * @param inter
 * @param var_list
 * @return
 */
ResultType setParameter(Parameter *call_base, Parameter *function_base, VarList *function_var, LinkValue *function_father, INTER_FUNCTIONSIG_NOT_ST) {
    Argument *call = NULL;
    setResultCore(result);
    call = getArgument(call_base, false, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
    if (!run_continue(result)) {
        freeArgument(call, false);
        return result->type;
    }

    freeResult(result);
    setParameterCore(call, function_base, function_var, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, function_father));
    freeArgument(call, false);
    return result->type;
}

ResultType setParameterCore(Argument *call, Parameter *function_base, VarList *function_var,
                        INTER_FUNCTIONSIG_NOT_ST) {
    Parameter *function = NULL;
    Parameter *tmp_function = NULL;  // 释放使用
    enum {
        match_status = 1,
        default_status = 2,
        self_ass = 3,
        mul_par = 4,
        space_kwargs = 5,
        error = -1,
        finished = 0,
    } status = match_status;
    function = copyParameter(function_base);
    tmp_function = function;
    setResultCore(result);

    while (true){
        if (call == NULL && function == NULL)
            status = finished;
        else if ((call != NULL && (function == NULL || call->type == value_par && function->type == kwargs_par)) ||
                 (call == NULL && function != NULL && function->type == value_par))
            status = error;
        else if (call == NULL && function->type == name_par)  // 根据前面的条件, 已经决定function不会为NULL
            status = default_status;
        else if (call == NULL && function->type == kwargs_par)
            status = space_kwargs;
        else if (function->type == args_par)  // 根据前面的条件, 已经决定call不会为NULL
            status = mul_par;
        else if (call->type == value_arg)
            status = match_status;
        else if (call->type == name_arg)
            status = self_ass;

        freeResult(result);
        switch (status) {
            case match_status: {
                argumentToParameter(&call, &function, function_var, CALL_INTER_FUNCTIONSIG_NOT_ST (var_list, result, father));
                returnResult(result);
                break;
            }
            case default_status: {
                NUMBER_TYPE num = 0;
                defaultParameter(&function, &num, CALL_INTER_FUNCTIONSIG_NOT_ST (function_var, result, father));
                returnResult(result);
                break;
            }
            case self_ass: {
                NUMBER_TYPE set_num = 0;
                NUMBER_TYPE get_num = 0;
                bool dict_status = false;
                VarList *tmp = pushVarList(var_list, inter);

                argumentToVar(&call, &set_num, CALL_INTER_FUNCTIONSIG_NOT_ST (tmp, result, father));
                returnResult(result);
                if (!run_continue(result)) {
                    popVarList(tmp);
                    goto return_;
                }

                freeResult(result);
                parameterFromVar(&function, function_var, &get_num, set_num, &dict_status, CALL_INTER_FUNCTIONSIG_NOT_ST (tmp, result, father));
                if (!run_continue(result)) {
                    popVarList(tmp);
                    goto return_;
                }
                popVarList(tmp);

                if (!dict_status && set_num > get_num) {
                    freeResult(result);
                    goto error_;
                }
                break;
            }
            case mul_par: {
                LinkValue *tmp = makeLinkValue(makeListValue(&call, inter, value_tuple), father, inter);
                if (!run_continue(result))
                    goto return_;
                else
                    freeResult(result);

                assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST (function_var, result, father));
                returnResult(result);
                function = function->next;
                break;
            }
            case space_kwargs:{
                LinkValue *tmp = makeLinkValue(makeDictValue(NULL, true, father, result, inter, var_list), father, inter);
                assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST (function_var, result, father));
                returnResult(result);
                function = function->next;
                break;
            }
            case error:
            error_:  // Statement 处理
                setResultError(result, inter, "ArgumentException", "Set Argument error", 0, father, true);
                goto return_;
            default:
                goto break_;
        }
    }
    break_:
    setResult(result, inter, father);

    return_:
    freeParameter(tmp_function, true);
    return result->type;
}

FatherValue *setFather(Argument *call, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);

    FatherValue *father_tmp = NULL;
    for (Argument *tmp = call; tmp != NULL && tmp->type == value_arg; tmp = tmp->next)
        if (tmp->data.value->value->type == class) {
            father_tmp = connectFatherValue(father_tmp, makeFatherValue(tmp->data.value));
            father_tmp = connectFatherValue(father_tmp, copyFatherValue(tmp->data.value->value->object.father));
        }

    return setFatherCore(father_tmp);
}

FatherValue *setFatherCore(FatherValue *father_tmp) {
    FatherValue *base_father = NULL;
    while (father_tmp != NULL){
        FatherValue *next = father_tmp->next;
        father_tmp->next = NULL;
        base_father = connectSafeFatherValue(base_father, father_tmp);
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
