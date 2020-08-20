#include "__run.h"

static bool compareNumber(int a, int b, int type);

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
        if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(function->data.value, var_list, result, belong)))
            goto return_;

        value = result->value;
        freeResult(result);
        assCore(function->data.name, value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!run_continue(result))
            goto return_;
    }
    setResult(result, inter, belong);

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
        assCore(call->data.name, call->data.value, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!run_continue(result))
            goto return_;
    }
    setResult(result, inter, belong);

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
            value = makeLinkValue(makeDictValue(NULL, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, NULL, belong)), belong, inter);
            value->value->data.dict.dict = var_list->hashtable;
            value->value->data.dict.size = max - *num;
            *status = true;
            goto not_return;
        }

        freeResult(result);
        getVarInfo(&str_name, &int_times, CALL_INTER_FUNCTIONSIG(name, var_list, result, belong));
        if (!run_continue(result)) {
            memFree(str_name);
            *function_ad = function;
            return result->type;
        }

        freeResult(result);
        value = findFromVarList(str_name, int_times, 1, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        memFree(str_name);

        if(value == NULL) {
            get = false;
            if (function->type == name_par && !operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(function->data.value, var_list, result, belong))) {
                value = result->value;
                goto not_return;
            }
            setResultErrorSt(result, inter, "ArgumentException", "Too less Argument", name, belong, true);
            goto reutnr_;
        }
        else if ((name->aut == public_aut || name->aut == auto_aut) && (value->aut != public_aut && value->aut != auto_aut)) {
            setResultErrorSt(result, inter, "PermissionsException", "Wrong Permissions: access Argument as public",
                             name,
                             belong, true);
            goto reutnr_;
        }
        else if ((name->aut == protect_aut) && (value->aut == private_aut)) {
            setResultErrorSt(result, inter, "PermissionsException", "Wrong Permissions: access variables as protect",
                             name, belong, true);
            goto reutnr_;
        }

        not_return:
        freeResult(result);
        assCore(name, value, CALL_INTER_FUNCTIONSIG_NOT_ST(function_var, result, belong));

        if (!run_continue(result)) {
            *function_ad = function;
            return result->type;
        }
        else
            freeResult(result);

        if (get)
            (*num)++;
    }

    setResult(result, inter, belong);
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
        assCore(name, call->data.value, CALL_INTER_FUNCTIONSIG_NOT_ST(function_var, result, belong));
        if (!run_continue(result))
            goto return_;
        freeResult(result);
    }

    setResult(result, inter, belong);
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
        if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(call->data.value, var_list, result, belong)))
            goto return_;

        if (call->type == value_par)
            base = connectValueArgument(result->value, base);
        else if (call->type == name_par){
            if (is_dict){
                LinkValue *value = result->value;
                setResultCore(result);
                if(operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(call->data.name, var_list, result, belong))) {
                    gc_freeTmpLink(&value->gc_status);
                    goto return_;
                }
                char *name_str = getNameFromValue(result->value->value, inter);
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
    setResult(result, inter, belong);

    return_:
    *base_ad = base;
    return result->type;
}

Argument * getArgument(Parameter *call, bool is_dict, INTER_FUNCTIONSIG_NOT_ST) {
    Argument *new_arg = NULL;
    setResultCore(result);
    iterParameter(call, &new_arg, is_dict, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
ResultType setParameter(long int line, char *file, Parameter *call_base, Parameter *function_base, VarList *function_var, LinkValue *function_father, INTER_FUNCTIONSIG_NOT_ST) {
    Argument *self_tmp = makeValueArgument(function_father);
    Argument *father_tmp = makeValueArgument(function_father->belong);
    Argument *call = NULL;
    setResultCore(result);
    call = getArgument(call_base, false, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    self_tmp->next = father_tmp;
    father_tmp->next = call;
    if (!run_continue(result)) {
        freeArgument(call, false);
        return result->type;
    }

    freeResult(result);
    setParameterCore(line, file, self_tmp, function_base, function_var, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, function_father));
    freeArgument(self_tmp, false);
    return result->type;
}

ResultType setParameterCore(long int line, char *file, Argument *call, Parameter *function_base, VarList *function_var,
                        INTER_FUNCTIONSIG_NOT_ST) {
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
        finished = 0,
    } status = match_status;
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
        else if (function->type == args_par)  // 根据前面的条件, 已经决定call不会为NULL
            status = mul_par;
        else if (call->type == value_arg)
            status = match_status;
        else if (call->type == name_arg)
            status = self_ass;

        freeResult(result);
        switch (status) {
            case match_status: {
                argumentToParameter(&call, &function, function_var, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                returnResult(result);
                break;
            }
            case default_status: {
                NUMBER_TYPE num = 0;
                defaultParameter(&function, &num, CALL_INTER_FUNCTIONSIG_NOT_ST(function_var, result, belong));
                returnResult(result);
                break;
            }
            case self_ass: {
                NUMBER_TYPE set_num = 0;
                NUMBER_TYPE get_num = 0;
                bool dict_status = false;
                VarList *tmp = makeVarList(inter, true);

                argumentToVar(&call, &set_num, CALL_INTER_FUNCTIONSIG_NOT_ST(tmp, result, belong));
                if (!run_continue(result)) {
                    freeVarList(tmp);
                    goto return_;
                }

                freeResult(result);
                parameterFromVar(&function, function_var, &get_num, set_num, &dict_status, CALL_INTER_FUNCTIONSIG_NOT_ST(tmp, result, belong));
                freeVarList(tmp);
                if (!run_continue(result))
                    goto return_;

                if (!dict_status && set_num > get_num)
                    goto to_more;
                break;
            }
            case mul_par: {
                LinkValue *tmp = makeLinkValue(makeListValue(&call, inter, value_tuple), belong, inter);
                assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(function_var, result, belong));
                returnResult(result);
                function = function->next;
                break;
            }
            case space_kwargs:{
                LinkValue *tmp = makeLinkValue(makeDictValue(NULL, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)), belong, inter);
                returnResult(result);
                freeResult(result);

                assCore(function->data.value, tmp, CALL_INTER_FUNCTIONSIG_NOT_ST(function_var, result, belong));
                returnResult(result);
                function = function->next;
                break;
            }
            case error_to_less:
                setResultError(result, inter, "ArgumentException", "Too less argument", line, file, belong, true);
                goto return_;
            case error_to_more:
            to_more:
                setResultError(result, inter, "ArgumentException", "Too more argument", line, file, belong, true);
                goto return_;
            case error_kw:
                setResultError(result, inter, "ArgumentException", "Value argument for double star", line, file, belong, true);
                goto return_;
            default:
                goto break_;
        }
    }
    break_:
    setResult(result, inter, belong);

    return_:
    gc_freeze(inter, function_var, NULL, false);
    gc_freeze(inter, var_list, NULL, false);
    freeParameter(tmp_function, true);
    return result->type;
}

Inherit *setFather(Argument *call) {
    Inherit *father_tmp = NULL;
    for (Argument *tmp = call; tmp != NULL && tmp->type == value_arg; tmp = tmp->next)
        if (tmp->data.value->value->type == class) {
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

/**
 *
 * @param c_value value_类型的参数的最大值
 * @param c_name name_类型参数的最大值
 * @param type_value c_value [<= < == > >=] c_value_
 * @param type_name c_name [<= < == > >=] c_name_
 * @param arg
 * @return
 */
bool checkArgument(int c_value, int c_name, int type_value, int type_name, Argument *arg) {
    int c_value_ = 0;
    int c_name_ = 0;
    for (PASS; arg != NULL; arg = arg->next){
        if (arg->type == value_arg)
            c_value_++;
        else
            c_name_++;
    }
    if ((c_value == -1 || compareNumber(c_value, c_value_, type_value)) && (c_name == -1 || compareNumber(c_name, c_name_, type_name)))
        return true;
    return false;
}

int parserArgumentUnion(ArgumentParser ap[], Argument *arg, INTER_FUNCTIONSIG_NOT_ST){
    setResultCore(result);
    if (ap->type != only_name){
        ArgumentParser *bak = NULL;
        int status = 1;
        arg = parserValueArgument(ap, arg, &status, &bak);
        if (status != 1){
            setResultError(result, inter, "ArgumentException", "Too less Argument", 0, "sys", belong, true);
            return 0;
        }
        ap = bak;
    }
    if (ap->must != -1){
        ArgumentParser *bak = NULL;
        int status;

        if (arg != NULL && arg->type != name_arg) {
            setResultError(result, inter, "ArgumentException", "Too many Argument", 0, "sys", belong, true);
            return -6;
        }

        status = parserNameArgument(ap, arg, &bak, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!run_continue(result))
            return -1;
        if (status == -3){
            if (parserArgumentNameDefault(ap)->must != -1){
                setResultError(result, inter, "ArgumentException", "Too less Argument", 0, "sys", belong, true);
                return -7;
            }
        }
        else if (status == 0){
            setResultError(result, inter, "ArgumentException", "Too many Argument", 0, "sys", belong, true);
            return -2;
        } else if (status == -4){
            setResultError(result, inter, "ArgumentException", "Too less Argument", 0, "sys", belong, true);
            return -3;
        }
    } else{
        if (arg != NULL) {
            setResultError(result, inter, "ArgumentException", "Too many Argument", 0, "sys", belong, true);
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

int parserNameArgument(ArgumentParser ap[], Argument *arg, ArgumentParser **bak, INTER_FUNCTIONSIG_NOT_ST){
    VarList *tmp = NULL;
    NUMBER_TYPE set_num = 0;
    NUMBER_TYPE get_num = 0;
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
    argumentToVar(&arg, &set_num, CALL_INTER_FUNCTIONSIG_NOT_ST(tmp, result, belong));
    if (!run_continue(result)) {
        return_ = -1;
        goto return_;
    }
    setResultBase(result, inter, belong);

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
    LinkValue *value = findStrVar(ap->name, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
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

static bool compareNumber(int a, int b, int type){
    switch (type) {
        case -2:
            return a <= b;
        case -1:
            return a < b;
        case 0:
            return a == b;
        case 1:
            return a > b;
        case 2:
            return a >= b;
        default:
            return false;
    }
}
