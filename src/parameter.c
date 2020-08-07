#include "__virtualmath.h"

#define returnResult(result) do{ \
if (!run_continue(result)) { \
goto return_; \
} \
}while(0)

Argument *makeArgument(){
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
    gcAddTmp(&value->gc_status);

    return tmp;
}

Argument *makeStatementNameArgument(LinkValue *value, Statement *name){
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->data.value = value;
    tmp->data.name = name;
    gcAddTmp(&value->gc_status);
    return tmp;
}

Argument *makeCharNameArgument(LinkValue *value, LinkValue *name_value, char *name) {
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->name_type = name_char;
    tmp->data.value = value;
    tmp->data.name_ = memStrcpy(name, 0, false, false);
    tmp->data.name_value = name_value;
    gcAddTmp(&value->gc_status);
    gcAddTmp(&name_value->gc_status);
    return tmp;
}

Argument *connectArgument(Argument *new, Argument *base){
    if (base == NULL)
        return new;
    Argument *tmp = base;
    while (base->next != NULL)
        base = base->next;
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
    while (at != NULL){
        if (free_st)
            freeStatement(at->data.name);
        memFree(at->data.name_);

        if (at->data.name_value != NULL)
            gcFreeTmpLink(&at->data.name_value->gc_status);
        if (at->data.value != NULL)
            gcFreeTmpLink(&at->data.value->gc_status);

        Argument *tmp = at->next;
        memFree(at);
        at = tmp;
    }
}

Parameter *makeParameter(){
    Parameter *tmp = memCalloc(1, sizeof(Parameter));
    tmp->type = value_par;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->next = NULL;
    return tmp;
}

Parameter *copyParameter(Parameter *base){
    if (base == NULL)
        return NULL;

    Parameter *tmp = NULL;
    Parameter *base_tmp = NULL;
    tmp = makeParameter();
    base_tmp = tmp;

    tmp->data.value = copyStatement(base->data.value);
    tmp->data.name = copyStatement(base->data.name);
    tmp->type = base->type;
    while (base->next != NULL){
        tmp->next = makeParameter();
        tmp = tmp->next;
        base = base->next;
        tmp->data.value = copyStatement(base->data.value);
        tmp->data.name = copyStatement(base->data.name);
        tmp->type = base->type;
    }
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
    while (pt != NULL){
        if (free_st) {
            freeStatement(pt->data.value);
            freeStatement(pt->data.name);
        }
        Parameter *tmp = pt->next;
        memFree(pt);
        pt = tmp;
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
    for (int i = 0; i < MAX_SIZE; i++) {
        Var *tmp = dict_value->value->data.dict.dict->hashtable[i];
        while (tmp != NULL) {
            at = connectCharNameArgument(tmp->value, tmp->name_, tmp->name, at);
            tmp = tmp->next;
        }
    }
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
Result defaultParameter(Parameter **function_ad, Inter *inter, VarList *var_list, int *num) {
    Parameter *function = *function_ad;
    Result result;
    setResultCore(&result);

    *num = 0;
    while (function != NULL && function->type == name_par){
        LinkValue *value = NULL;
        if(operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(function->data.value, var_list)))
            goto return_;

        value = result.value;
        freeResult(&result);
        result = assCore(function->data.name, value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(result))
            goto return_;

        (*num)++;
        function = function->next;
    }
    setResult(&result, inter);

    return_:
    *function_ad = function;
    return result;
}

/**
 * 设置实际参数的默认值, 仅支持name_base_value
 * @param call_ad
 * @param inter
 * @param var_list
 * @param num
 * @return
 */
Result argumentToVar(Argument **call_ad, struct Inter *inter, struct VarList *var_list, NUMBER_TYPE *num) {
    Argument *call = *call_ad;
    Result result;
    setResultCore(&result);

    *num = 0;
    while (call != NULL && call->type == name_arg){
        if (call->name_type == name_char){
            addFromVarList(call->data.name_, var_list, 0, call->data.value, call->data.name_value);
            goto next;
        }
        freeResult(&result);
        result = assCore(call->data.name, call->data.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (!run_continue(result))
            goto return_;

        next:
        (*num)++;
        call = call->next;
    }
    setResult(&result, inter);

    return_:
    *call_ad = call;
    return result;
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
Result parameterFromVar(Parameter **function_ad, VarList *function_var, struct Inter *inter, struct VarList *var_list,
                        NUMBER_TYPE *num, NUMBER_TYPE max, bool *status) {
    Parameter *function = *function_ad;
    Result result;
    setResultCore(&result);

    bool get;
    *num = 0;
    *status = false;
    while (function != NULL){
        int int_times;
        char *str_name = NULL;
        Statement *name = function->type == name_par ? function->data.name : function->data.value;
        LinkValue *value = NULL;

        get = true;

        if (function->type == kwargs_par){
            value = makeLinkValue(makeDictValue(NULL, false, NULL, inter, var_list), NULL, inter);
            value->value->data.dict.dict = var_list->hashtable;
            value->value->data.dict.size = max - *num;
            *status = true;
            goto not_return;
        }

        freeResult(&result);
        result = getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list));
        if (!run_continue(result)) {
            memFree(str_name);
            return result;
        }
        freeResult(&result);
        value = findFromVarList(str_name, var_list, int_times, true);
        memFree(str_name);

        if(value == NULL) {
            get = false;
            if (function->type == name_par && !operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(function->data.value, var_list))) {
                value = result.value;
                goto not_return;
            }
            setResultError(&result, inter, "ArgumentException", "Too less Argument", name, true);
            *function_ad = function;
            return result;
        }

        not_return:
        freeResult(&result);
        result = assCore(name, value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
        if (!run_continue(result)) {
            *function_ad = function;
            return result;
        }
        else
            freeResult(&result);

        if (get)
            (*num)++;
        function = function->next;
    }

    setResult(&result, inter);
    *function_ad = function;
    return result;
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
Result argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_CORE){
    Argument *call = *call_ad;
    Parameter *function = *function_ad;
    Result result;
    setResultCore(&result);

    while (call != NULL && function != NULL && (call->type == value_arg) && function->type != args_par){
        Statement *name = function->type == value_par ? function->data.value : function->data.name;
        result = assCore(name, call->data.value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
        if (!run_continue(result))
            goto return_;

        freeResult(&result);
        call = call->next;
        function = function->next;
    }

    setResult(&result, inter);
    return_:
    *call_ad = call;
    *function_ad = function;
    return result;
}

/**
 * 把所有实际参数的值计算出来
 * @param call_ad
 * @param inter
 * @param var_list
 * @return
 */
Result iterParameter(Parameter *call, Argument **base_ad, INTER_FUNCTIONSIG_CORE){
    Argument *base = *base_ad;
    Result result;
    setResultCore(&result);

    while (call != NULL){
        if(operationSafeInterStatement(&result, CALL_INTER_FUNCTIONSIG(call->data.value, var_list)))
            goto return_;

        if (call->type == value_par)
            base = connectValueArgument(result.value, base);
        else if (call->type == name_par)
            base = connectStatementNameArgument(result.value, call->data.name, base);
        else if (call->type == args_par){
            Argument *tmp_at = listToArgument(result.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            base = connectArgument(tmp_at, base);
        }
        else if (call->type == kwargs_par){
            Argument *tmp_at = dictToArgument(result.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            base = connectArgument(tmp_at, base);
        }
        freeResult(&result);
        call = call->next;
    }
    setResult(&result, inter);

    return_:
    *base_ad = base;
    return result;
}

Argument *getArgument(Parameter *call, Result *result, INTER_FUNCTIONSIG_CORE){
    Argument *new_arg = NULL;
    freeResult(result);
    *result = iterParameter(call, &new_arg, CALL_INTER_FUNCTIONSIG_CORE(var_list));
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
Result setParameter(Parameter *call_base, Parameter *function_base, VarList *function_var, Statement *base,
                    struct Inter *inter, struct VarList *var_list) {
    Argument *call;
    Result result;
    setResultCore(&result);
    call = getArgument(call_base, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    if (!run_continue(result)) {
        freeArgument(call, false);
        return result;
    }
    freeResult(&result);
    result = setParameterCore(call, function_base, function_var, base, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    freeArgument(call, false);
    return result;
}

Result setParameterCore(Argument *call, Parameter *function_base, VarList *function_var, Statement *base,
                        struct Inter *inter, struct VarList *var_list) {
    Parameter *function = NULL, *tmp_function = NULL;  // 释放使用
    Result result;
    setResultCore(&result);
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

        freeResult(&result);
        switch (status) {
            case match_status: {
                result = argumentToParameter(&call, &function, function_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
                returnResult(result);
                break;
            }
            case default_status: {
                int num = 0;
                result = defaultParameter(&function, CALL_INTER_FUNCTIONSIG_CORE(function_var), &num);
                returnResult(result);
                break;
            }
            case self_ass: {
                VarList *tmp = pushVarList(var_list, inter);
                NUMBER_TYPE set_num = 0, get_num = 0;
                bool dict_status = false;
                result = argumentToVar(&call, CALL_INTER_FUNCTIONSIG_CORE(tmp), &set_num);
                returnResult(result);
                if (!run_continue(result)) {
                    freeVarList(tmp, true);
                    goto return_;
                }

                freeResult(&result);
                result = parameterFromVar(&function, function_var, CALL_INTER_FUNCTIONSIG_CORE(tmp), &get_num, set_num,
                                          &dict_status);
                if (!run_continue(result)) {
                    popVarList(tmp);
                    goto return_;
                }
                popVarList(tmp);

                if (!dict_status && set_num > get_num) {
                    freeResult(&result);
                    goto error_;
                }
                break;
            }
            case mul_par: {
                LinkValue *tmp = makeLinkValue(makeListValue(&call, inter, value_tuple), NULL, inter);
                if (!run_continue(result))
                    goto return_;
                else
                    freeResult(&result);
                result = assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_CORE(function_var));
                returnResult(result);
                function = function->next;
                break;
            }
            case space_kwargs:{
                LinkValue *tmp = makeLinkValue(makeDictValue(NULL, true, &result, inter, var_list), NULL, inter);

                result = assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_CORE(function_var));
                returnResult(result);
                function = function->next;
                break;
            }
            case error:
            error_:  // Statement 处理
                writeLog(inter->data.debug, ERROR, "setParameter error\n", NULL);
                setResultError(&result, inter, "ArgumentException", "Set Argument error", 0, true);
                goto return_;
            default:
                goto break_;
        }
    }
    break_:
    setResult(&result, inter);

    return_:
    freeParameter(tmp_function, true);
    return result;
}
