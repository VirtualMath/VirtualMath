#include "__virtualmath.h"
#define returnResult(result) do{ \
if (!run_continue(result)) { \
return result; \
} \
}while(0)

Argument *makeArgument(){
    Argument *tmp = memCalloc(1, sizeof(Argument));
    tmp->type = value_arg;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->next = NULL;
    return tmp;
}

Argument *makeOnlyValueArgument(LinkValue *value){
    Argument *tmp = makeArgument();
    tmp->data.value = value;
    return tmp;
}

Argument *makeNameValueArgument(LinkValue *value, Statement *name){
    Argument *tmp = makeArgument();
    tmp->type = name_arg;
    tmp->data.value = value;
    tmp->data.name = name;
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

Argument *connectOnlyValueArgument(LinkValue *value, Argument *base){
    Argument *new = makeOnlyValueArgument(value);
    return connectArgument(new, base);
}

Argument *connectNameValueArgument(LinkValue *value, Statement *name, Argument *base){
    Argument *new = makeNameValueArgument(value, name);
    return connectArgument(new, base);
}

void freeArgument(Argument *pt, bool free_st) {
    while (pt != NULL){
        if (free_st)
            freeStatement(pt->data.name);
        Argument *tmp = pt->next;
        memFree(pt);
        pt = tmp;
    }
}

Parameter *makeParameter(){
    Parameter *tmp = memCalloc(1, sizeof(Parameter));
    tmp->type = only_value;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->next = NULL;
    return tmp;
}

Parameter *copyParameter(Parameter *base){
    if (base == NULL)
        return NULL;
    Parameter *tmp = makeParameter(), *base_tmp = tmp;
    tmp->data = base->data;
    tmp->type = base->type;
    while (base->next != NULL){
        tmp->next = makeParameter();
        tmp = tmp->next;
        base = base->next;
        tmp->data = base->data;
        tmp->type = base->type;
    }
    return base_tmp;
}

Parameter *makeOnlyValueParameter(Statement *st){
    Parameter *tmp = makeParameter();
    tmp->data.value = st;
    return tmp;
}

Parameter *makeNameValueParameter(Statement *value, Statement *name){
    Parameter *tmp = makeParameter();
    tmp->type = name_value;
    tmp->data.value = value;
    tmp->data.name = name;
    return tmp;
}

Parameter *makeOnlyArgsParameter(Statement *st){
    Parameter *tmp = makeParameter();
    tmp->type = only_args;
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

Parameter *connectOnlyValueParameter(Statement *st, Parameter *base){
    Parameter *new = makeOnlyValueParameter(st);
    return connectParameter(new, base);
}

Parameter *connectNameValueParameter(Statement *value, Statement *name, Parameter *base){
    Parameter *new = makeNameValueParameter(value, name);
    return connectParameter(new, base);
}

Parameter *connectOnlyArgsParameter(Statement *st, Parameter *base){
    Parameter *new = makeOnlyArgsParameter(st);
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
    for (int i=0;i<list_value->value->data.list.size;i++){
        at = connectOnlyValueArgument(list_value->value->data.list.list[i], at);
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
    while (function != NULL && function->type == name_value){
        Result tmp, tmp_ass;
        if(operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(function->data.value, var_list))) {
            *function_ad = function;
            return tmp;
        }

        tmp_ass = assCore(function->data.name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (tmp_ass.type == error_return) {
            *function_ad = function;
            return tmp_ass;
        }
        (*num)++;
        function = function->next;
    }
    setResult(&result, true, inter);

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
Result argumentToVar(Argument **call_ad, struct Inter *inter, struct VarList *var_list, int *num) {
    Argument *call = *call_ad;
    Result result;
    while (call != NULL && call->type == name_arg){
        Result tmp_ass;
        tmp_ass = assCore(call->data.name, call->data.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        if (tmp_ass.type == error_return) {
            *call_ad = call;
            return tmp_ass;
        }
        (*num)++;
        call = call->next;
    }
    setResult(&result, true, inter);

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
Result parameterFromVar(Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_CORE, int *num){
    Parameter *function = *function_ad;
    Result result;
    bool get;
    while (function != NULL){
        Result tmp, tmp_ass;
        get = true;
        Statement *name = function->type == only_value ? function->data.value : function->data.name;
        if(operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(name, var_list))) {
            get = false;
            if (function->type == name_value && !operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(function->data.value, var_list)))
                goto not_return;
            *function_ad = function;
            return tmp;
        }
        not_return:
        tmp_ass = assCore(name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
        if (tmp_ass.type == error_return) {
            *function_ad = function;
            return tmp_ass;
        }
        if (get)
            (*num)++;
        function = function->next;
    }
    setResult(&result, true, inter);

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
    while (call != NULL && function != NULL && (call->type == value_arg) && function->type != only_args){
        Result tmp_ass;
        Statement *name = function->type == only_value ? function->data.value : function->data.name;
        tmp_ass = assCore(name, call->data.value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
        if (tmp_ass.type == error_return) {
            *call_ad = call;
            *function_ad = function;
            return tmp_ass;
        }

        call = call->next;
        function = function->next;
    }
    setResult(&result, true, inter);
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
    Result result;
    Argument *base = *base_ad;
    while (call != NULL){
        Result tmp;
        if(operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(call->data.value, var_list)))
            return tmp;

        if (call->type == only_value)
            base = connectOnlyValueArgument(tmp.value, base);
        else if (call->type == name_value)
            base = connectNameValueArgument(tmp.value, call->data.name, base);
        else if (call->type == only_args){
            Argument *tmp_at = listToArgument(tmp.value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
            base = connectArgument(tmp_at, base);
        }
        call = call->next;
    }
    setResult(&result, true, inter);
    *base_ad = base;
    return result;
}

Argument *getArgument(Parameter *call, Result *result, INTER_FUNCTIONSIG_CORE){
    Argument *new_arg = NULL;
    *result = iterParameter(call, &new_arg, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return new_arg;
}

/**
 * 参数表:
 |实参 \ 形参| name | value | arg | null |
 ----------------------------------------
 |name     | p_3  |  p_3  | p_4 | error |
 |value    | p_1  |  p_1  | p_4 | error |
 |null     | p_2  | error | p_4 | okay  |
 ----------------------------------------
 * 注解: @p_1 match_status; @p_2 default_status; @p_3 self_ass; @p_4 mul_par
 * @param call
 * @param function
 * @param function_var
 * @param inter
 * @param var_list
 * @return
 */
Result setParameter(Parameter *call_base, Parameter *function_base, VarList *function_var, INTER_FUNCTIONSIG_CORE){
    Result result;
    Argument *call, *tmp_call;
    Parameter *function = copyParameter(function_base), *tmp_function = function;  // 释放使用
    tmp_call = call = getArgument(call_base, &result, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    returnResult(result);

    enum {
        match_status = 1,
        default_status = 2,
        self_ass = 3,
        mul_par = 4,
        error = -1,
        finished = 0
    } status = match_status;
    while (true){
        if (call == NULL && function == NULL)
            status = finished;
        else if ((call != NULL && function == NULL) || (call == NULL && function != NULL && function->type == only_value))
            status = error;
        else if (call == NULL && function->type == name_value)  // 根据前面的条件, 已经决定function不会为NULL
            status = default_status;
        else if (function->type == only_args)
            status = mul_par;
        else if (call->type == value_arg)
            status = match_status;
        else if (call->type == name_arg)
            status = self_ass;

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
                VarList *tmp = makeVarList(inter);
                int set_num = 0, get_num = 0;
                result = argumentToVar(&call, CALL_INTER_FUNCTIONSIG_CORE(tmp), &set_num);
                returnResult(result);
                result = parameterFromVar(&function, function_var, CALL_INTER_FUNCTIONSIG_CORE(tmp), &get_num);
                returnResult(result);
                freeVarList(tmp, true);

                if (set_num > get_num)
                    status = error;
                break;
            }
            case mul_par: {
                Result tmp_result;
                Value *value = makeListValue(&call, inter, value_tuple);
                LinkValue *tmp = makeLinkValue(value, NULL, inter);
                tmp_result = assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_CORE(function_var));
                returnResult(tmp_result);
                function = function->next;
                break;
            }
            case error:
                writeLog(inter->debug, ERROR, "setParameter error", NULL);
                setResultError(&result, inter);
                goto return_;
            default:
                goto return_;
        }
    }
    return_:
    freeArgument(tmp_call, false);
    freeParameter(tmp_function, false);
    return result;
}
