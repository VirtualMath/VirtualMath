#include "__virtualmath.h"

Parameter *makeParameter(){
    Parameter *tmp = memCalloc(1, sizeof(Parameter));
    tmp->type = only_value;
    tmp->data.value = NULL;
    tmp->data.name = NULL;
    tmp->next = NULL;
    return tmp;
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

void freeParameter(Parameter *pt){
    while (pt != NULL){
        freeStatement(pt->data.value);
        freeStatement(pt->data.name);
        Parameter *tmp = pt->next;
        memFree(pt);
        pt = tmp;
    }
}

/**
 * 使用形式参数的默认值
 * @param function_ad
 * @param inter
 * @param var_list
 * @return
 */
Result varParameter(Parameter **function_ad, INTER_FUNCTIONSIG_CORE, int *num){
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
 * 使用形式参数的默认值
 * @param call_ad
 * @param inter
 * @param var_list
 * @return
 */
Result nameParameter(Parameter **call_ad, VarList *function_var, INTER_FUNCTIONSIG_CORE, int *num){
    Parameter *call = *call_ad;
    Result result;
    bool get;
    while (call != NULL){
        Result tmp, tmp_ass;
        get = true;
        Statement *name = call->type == only_value ? call->data.value : call->data.name;
        if(operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(name, var_list))) {
            get = false;
            if (call->type == name_value && !operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(call->data.value, var_list))){
                goto not_return;
            }
            *call_ad = call;
            return tmp;
        }
        not_return:
        tmp_ass = assCore(name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
        if (tmp_ass.type == error_return) {
            *call_ad = call;
            return tmp_ass;
        }
        if (get)
            (*num)++;
        call = call->next;
    }
    setResult(&result, true, inter);

    *call_ad = call;
    return result;
}

/**
 * 根据形参和实参对位赋值
 * @param call_ad
 * @param function_ad
 * @param function_var
 * @param inter
 * @param var_list
 * @return
 */
Result valueParameter(Parameter **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_CORE){
    Parameter *call = *call_ad, *function = *function_ad;
    Result result;
    while (call != NULL && function != NULL && call->type != name_value && function->type != only_args){
        Result tmp, tmp_ass;
        Statement *name = function->type == only_value ? function->data.value : function->data.name;

        if(operationSafeInterStatement(&tmp, CALL_INTER_FUNCTIONSIG(call->data.value, var_list))) {
            *call_ad = call;
            *function_ad = function;
            return tmp;
        }

        tmp_ass = assCore(name, tmp.value, CALL_INTER_FUNCTIONSIG_CORE(function_var));
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

#define returnResult(result) do{ \
if (!run_continue(result)) { \
return result; \
} \
}while(0)

/**
 * 参数设定
 * [1] 形式参数和实际参数对位赋值
 * [2] 形式参数默认值
 * [1] -实参完成, 形参空缺-> [2]
 * [3] 实际参数直接赋值, 形式默认值补充
 * [1] -实参出现name_value类型-> [3]
 * [4] 把剩余的only_name实参转化为list
 * [1] -形参遇到only_args->[4]
 * [4] -实参完成-> [2]
 * [4] -实参未完成-> [3]
 * @param call
 * @param function
 * @param function_var
 * @param inter
 * @param var_list
 * @return
 */
Result setParameter(Parameter *call, Parameter *function, VarList *function_var, INTER_FUNCTIONSIG_CORE){
    enum {
        p_1 = 1,
        p_2 = 2,
        p_3 = 3,
        p_4 = 4,
        error = -1,
        finished = 0
    } status = p_1;
    Result result;
    while (true){
        switch (status) {
            case p_1: {
                result = valueParameter(&call, &function, function_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
                returnResult(result);
                if (call != NULL && function != NULL && call->type == name_value)
                    status = p_3;
                else if (call == NULL && function != NULL && function->type == name_value)
                    status = p_2;
                else if (call != NULL && function != NULL && function->type == only_args && call->type == only_value)
                    status = p_4;
                else if (call != NULL || function != NULL)
                    status = error;
                else
                    status = finished;
                break;
            }
            case p_2: {
                int num = 0;
                result = varParameter(&function, CALL_INTER_FUNCTIONSIG_CORE(function_var), &num);
                returnResult(result);
                status = finished;
                break;
            }
            case p_3: {
                VarList *tmp = makeVarList(inter);
                int set_num = 0, get_num = 0;
                result = varParameter(&call, CALL_INTER_FUNCTIONSIG_CORE(tmp), &set_num);
                returnResult(result);
                result = nameParameter(&function, function_var, CALL_INTER_FUNCTIONSIG_CORE(tmp), &get_num);
                returnResult(result);
                freeVarList(tmp, true);
                if (set_num > get_num) {
                    status = error;
                    break;
                }
                status = finished;
                break;
            }
            case p_4: {
                Result tmp_result;
                Value *value = makeListValue(&call, &tmp_result, CALL_INTER_FUNCTIONSIG_CORE(var_list), value_tuple);
                returnResult(tmp_result);
                LinkValue *tmp = makeLinkValue(value, NULL, inter);
                tmp_result = assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_CORE(function_var));
                returnResult(tmp_result);
                function = function->next;
                if (call != NULL && function != NULL && call->type == name_value)
                    status = p_3;
                else if (call == NULL && function != NULL && function->type == name_value)
                    status = p_2;
                else if (call != NULL || function != NULL)
                    status = error;
                else
                    status = finished;
                break;
            }
            case error:
                setResultError(&result, inter);
                return result;
            default:
                goto return_;
        }
    }
    return_:
    return result;
}

#undef returnResult
