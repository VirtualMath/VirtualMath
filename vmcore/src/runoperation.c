#include "__run.h"

static bool getLeftRightValue(Result *left, Result *right, FUNC);
static ResultType operationCore(FUNC, wchar_t *name, enum OperationType type);
static ResultType operationCore2(FUNC, wchar_t *name, enum OperationType type);
static ResultType assOperation(FUNC);
ResultType pointOperation(FUNC);
ResultType blockOperation(FUNC);
ResultType boolNotOperation(FUNC);
ResultType boolOperation(FUNC);

/**
 * operation的整体操作
 * @param st
 * @param inter
 * @param var_list
 * @return
 */
#define OPT_CASE(TYPE) case OPT_##TYPE: operationCore(CNEXT, inter->data.mag_func[M_##TYPE], OPT_##TYPE); break
#define OPT_CASE2(TYPE) case OPT_##TYPE: operationCore2(CNEXT, inter->data.mag_func[M_##TYPE], OPT_##TYPE); break
ResultType operationStatement(FUNC) {
    setResultCore(result);
    switch (st->u.operation.OperationType) {
        OPT_CASE(ADD);
        OPT_CASE(SUB);
        OPT_CASE(MUL);
        OPT_CASE(DIV);
        OPT_CASE(INTDIV);
        OPT_CASE(MOD);
        OPT_CASE(POW);


        OPT_CASE(BAND);
        OPT_CASE(BOR);
        OPT_CASE(BXOR);
        OPT_CASE(BL);
        OPT_CASE(BR);

        OPT_CASE(EQ);
        OPT_CASE(MOREEQ);
        OPT_CASE(LESSEQ);
        OPT_CASE(MORE);
        OPT_CASE(LESS);
        OPT_CASE(NOTEQ);

        OPT_CASE2(BNOT);
        OPT_CASE2(NEGATE);

        case OPT_NOT:
            boolNotOperation(CNEXT);
            break;

        case OPT_OR:
        case OPT_AND:
            boolOperation(CNEXT);
            break;

        case OPT_ASS:
            assOperation(CNEXT);
            break;
        case OPT_OUTPOINT:
        case OPT_POINT:
            pointOperation(CNEXT);
            break;
        case OPT_BLOCK:
            blockOperation(CNEXT);
            break;
        default:
            setResult(result, inter);
            break;
    }
    return result->type;
}
#undef OPT_CASE
#undef OPT_CASE2

static void updateBlockYield(Statement *block_st, Statement *node){
    block_st->info.node = node->type == yield_code ? node->next : node;
    block_st->info.have_info = true;
}

static void newBlockYield(Statement *block_st, Statement *node, VarList *new_var, Inter *inter){
    new_var->next = NULL;
    block_st->info.var_list = new_var;
    block_st->info.node = node->type == yield_code ? node->next : node;
    block_st->info.have_info = true;
}

static void setBlockResult(Statement *st, bool yield_run, Result *result, FUNC_CORE) {
    if (yield_run) {
        if (result->type == R_yield){
            updateBlockYield(st, result->node);
            result->type = R_opt;
            result->is_yield = true;
        }
        else
            freeRunInfo(st, true);
    }
    else {
        if (result->type == R_yield){
            newBlockYield(st, result->node, var_list, inter);
            result->type = R_opt;
            result->is_yield = true;
        }
        else
            popVarList(var_list);
    }
}

ResultType blockOperation(FUNC) {
    Statement *info_st = st->u.operation.left;
    bool yield_run;
    if ((yield_run = popYieldVarList(st, &var_list, var_list, inter)))
        info_st = st->info.node;
    blockSafeInterStatement(CFUNC(info_st, var_list, result, belong));
    if (result->type == R_error)
        return result->type;
    else
        setBlockResult(st, yield_run, result, CFUNC_CORE(var_list));
    if (CHECK_RESULT(result) && st->aut != auto_aut)
        result->value->aut = st->aut;  // 权限覆盖
    return result->type;
}

ResultType boolNotOperation(FUNC) {
    bool new;
    LinkValue *left;
    setResultCore(result);
    if (optSafeInterStatement(CFUNC(st->u.operation.left, var_list, result, belong)))
        return result->type;
    GET_RESULT(left, result);

    new = !checkBool(left, st->line, st->code_file, CNEXT_NT);
    gc_freeTmpLink(&left->gc_status);

    if (CHECK_RESULT(result)) {
        freeResult(result);
        makeBoolValue(new, st->line, st->code_file, CNEXT_NT);
    }
    return result->type;
}

ResultType boolOperation(FUNC) {
    bool left_bool;
    LinkValue *left;
    setResultCore(result);

    if (optSafeInterStatement(CFUNC(st->u.operation.left, var_list, result, belong)))
        return result->type;
    GET_RESULT(left, result);

    left_bool = checkBool(left, st->line, st->code_file, CNEXT_NT);
    gc_freeTmpLink(&left->gc_status);
    if (!CHECK_RESULT(result))
        return result->type;

    freeResult(result);
    if (st->u.operation.OperationType == OPT_AND) {  // 与运算
        if (left_bool)
            optSafeInterStatement(CFUNC(st->u.operation.right, var_list, result, belong));
        else
            setResultOperation(result, left);
    } else {  // 或运算
        if (left_bool)
            setResultOperation(result, left);
        else
            optSafeInterStatement(CFUNC(st->u.operation.right, var_list, result, belong));
    }
    return result->type;
}

ResultType pointOperation(FUNC) {
    LinkValue *left;
    VarList *object = NULL;
    setResultCore(result);
    if (optSafeInterStatement(CFUNC(st->u.operation.left, var_list, result, belong)) || result->value->value->type == V_none)
        return result->type;
    GET_RESULT(left, result);
    if (st->aut != auto_aut)
        left->aut = st->aut;  // 权限覆盖

    if (st->u.operation.OperationType == OPT_POINT)
        object = left->value->object.var;
    else if (st->u.operation.OperationType == OPT_OUTPOINT)
        object = left->value->object.out_var;

    if (object == NULL) {
        setResultError(E_TypeException, OBJ_NOTSUPPORT(->/.), st->line, st->code_file, true, CNEXT_NT);
        goto return_;
    }
    optSafeInterStatement(CFUNC(st->u.operation.right, object, result, left));  // 点运算运算时需要调整belong为点的左值

    if (CHECK_RESULT(result)) {
        /*
            class A() {
                pri:a = 10
                    fun pa(self) {  # 使用 ``z = A(); z.pa()`` 的方式调用该函数, belong被设置为`z`
                        self.a  # 左值即belong
                        A.a  # 左值是belong的父亲
                }
            }
         */
        bool pri_auto = left->value == belong->value || checkAttribution(belong->value, left->value);  // 判断左值是否即belong，或者是belong的父亲
        checkAut(left->aut, result->value->aut, st->line, st->code_file, NULL, pri_auto, CNEXT_NT);
    }
    // belong的设定再get_var中已经设定了

    return_:
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}

ResultType delOperation(FUNC) {
    Statement *var;
    setResultCore(result);
    var = st->u.del_.var;
    delCore(var, false, CNEXT_NT);
    return result->type;
}

ResultType delCore(Statement *name, bool check_aut, FUNC_NT) {
    setResultCore(result);
    if (name->type == base_list && name->u.base_list.type == L_tuple)
        listDel(name, CNEXT_NT);
    else if (name->type == slice_)
        downDel(name, CNEXT_NT);
    else if (name->type == operation && (name->u.operation.OperationType == OPT_POINT || name->u.operation.OperationType == OPT_OUTPOINT))
        pointDel(name, CNEXT_NT);
    else
        varDel(name, check_aut, CNEXT_NT);
    return result->type;
}

ResultType listDel(Statement *name, FUNC_NT) {
    setResultCore(result);
    for (Parameter *pt = name->u.base_list.list; pt != NULL; pt = pt->next){
        delCore(pt->data.value, false, CNEXT_NT);
        freeResult(result);
    }
    setResultBase(result, inter);
    return result->type;
}

ResultType varDel(Statement *name, bool check_aut, FUNC_NT) {
    wchar_t *str_name = NULL;
    int int_times = 0;
    LinkValue *get;
    setResultCore(result);
    getVarInfo(&str_name, &int_times, CFUNC(name, var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        memFree(str_name);
        return result->type;
    }
    if (check_aut) {
        LinkValue *tmp = findFromVarList(str_name, int_times, NULL, read_var, CFUNC_CORE(var_list));
        freeResult(result);
        if (tmp != NULL && !checkAut(name->aut, tmp->aut, name->line, name->code_file, NULL, false, CNEXT_NT))
            goto return_;
    }
    get = findFromVarList(str_name, int_times, NULL, del_var, CFUNC_CORE(var_list));
    if (get != NULL)
        setResult(result, inter);
    else {  // 变量没有删除成功
        setResultError(E_PermissionsException, L"wrong Permissions: Variable deletion failed"
                                                                  ", please confirm that the variable belongs to"
                                                                  " the left value and the number of layers is correct"
                                                                  , name->line, name->code_file, true, CNEXT_NT);
        // 变量删除失败，可能原因是层数错误
    }
    return_:
    memFree(str_name);
    return result->type;
}

ResultType pointDel(Statement *name, FUNC_NT) {
    LinkValue *left;
    VarList *object = NULL;
    Statement *right = name->u.operation.right;

    setResultCore(result);
    if (optSafeInterStatement(CFUNC(name->u.operation.left, var_list, result, belong)))
        return result->type;
    GET_RESULT(left, result);  // 不关心左值和整体(st)权限

    object = name->u.operation.OperationType == OPT_POINT ? left->value->object.var : left->value->object.out_var;
    if (object == NULL) {
        setResultError(E_TypeException, OBJ_NOTSUPPORT(->/.), name->line, name->code_file, true, CNEXT_NT);
        goto return_;
    }

    if (right->type == T_OPERATION && (right->u.operation.OperationType == OPT_POINT || right->u.operation.OperationType == OPT_OUTPOINT))
        pointDel(name->u.operation.right, CFUNC_NT(object, result, belong));
    else
        delCore(name->u.operation.right, true, CFUNC_NT(object, result, belong));

    return_:
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}

ResultType downDel(Statement *name, FUNC_NT) {
    LinkValue *iter = NULL;
    LinkValue *_func_ = NULL;
    Parameter *pt = name->u.slice_.index;

    setResultCore(result);
    if (optSafeInterStatement(CFUNC(name->u.slice_.element, var_list, result, belong)))
        return result->type;
    GET_RESULT(iter, result);

    if (name->u.slice_.type == SliceType_down_)
        _func_ = findAttributes(inter->data.mag_func[M_DOWN_DEL], false, LINEFILE, true, CFUNC_NT(var_list, result, iter));
    else
        _func_ = findAttributes(inter->data.mag_func[M_DOWN_DEL], false, LINEFILE, true, CFUNC_NT(var_list, result, iter));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_func_ != NULL){
        Argument *arg = NULL;
        gc_addTmpLink(&_func_->gc_status);
        arg = getArgument(pt, false, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto dderror_;
        freeResult(result);
        callBackCore(_func_, arg, name->line, name->code_file, 0, CNEXT_NT);

        dderror_:
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else
        setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(del(__down_del__/__slice_del__)), true, name, CNEXT_NT);

    return_:
    gc_freeTmpLink(&iter->gc_status);
    return result->type;
}

ResultType assOperation(FUNC) {
    LinkValue *value = NULL;
    setResultCore(result);
    if (st->u.operation.left->type == call_function){
        Statement *return_st = makeReturnStatement(st->u.operation.right, st->line, st->code_file);
        LinkValue *func = NULL;
        makeVMFunctionValue(return_st, st->u.operation.left->u.call_function.parameter, CNEXT_NT);
        return_st->u.return_code.value = NULL;
        freeStatement(return_st);

        GET_RESULT(func, result);
        assCore(st->u.operation.left->u.call_function.function, func, false, true, CNEXT_NT);
        gc_freeTmpLink(&func->gc_status);
    }
    else{
        if (optSafeInterStatement(CFUNC(st->u.operation.right, var_list, result, belong)))
            return result->type;
        value = result->value;

        freeResult(result);
        assCore(st->u.operation.left, value, false, false, CNEXT_NT);
    }
    return result->type;
}

ResultType assCore(Statement *name, LinkValue *value, bool check_aut, bool setting, FUNC_NT) {
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);

    if (name->type == base_list && name->u.base_list.type == L_tuple)
        listAss(name, value, CNEXT_NT);
    else if (name->type == slice_)
        downAss(name, value, CNEXT_NT);
    else if (name->type == operation && (name->u.operation.OperationType == OPT_POINT || name->u.operation.OperationType == OPT_OUTPOINT))
        pointAss(name, value, CNEXT_NT);
    else
        varAss(name, value, check_aut, setting, CNEXT_NT);

    gc_freeTmpLink(&value->gc_status);
    return result->type;
}


ResultType varAss(Statement *name, LinkValue *value, bool check_aut, bool setting, FUNC_NT) {
    wchar_t *str_name = NULL;
    int int_times = 0;
    LinkValue *name_ = NULL;
    LinkValue *tmp;
    bool run = name->type == base_var ? name->u.base_var.run : name->type == base_svar ? name->u.base_svar.run : false;

    setResultCore(result);
    getVarInfo(&str_name, &int_times, CFUNC(name, var_list, result, belong));
    if (!CHECK_RESULT(result)) {
        memFree(str_name);
        return result->type;
    }
    GET_RESULT(name_, result);
    if (name->aut != auto_aut)  // 当左值设定访问权限的时候, 覆盖右值的权限
        value->aut = name->aut;

    tmp = findFromVarList(str_name, int_times, NULL, read_var, CFUNC_CORE(var_list));
    if (check_aut) {
        if (tmp != NULL && !checkAut(value->aut, tmp->aut, name->line, name->code_file, NULL, false, CNEXT_NT))
            goto error_;
    }

    if (tmp == NULL || !run || !setVarFunc(tmp, value, name->line, name->code_file, CNEXT_NT))
        addFromVarList(str_name, name_, int_times, value, CFUNC_CORE(var_list));
    if (!CHECK_RESULT(result))
        goto error_;
    if (setting) {
        freeResult(result);
        newObjectSetting(name_, name->line, name->code_file, value, result, inter, var_list);
        if (!CHECK_RESULT(result))
            goto error_;
    }

    freeResult(result);
    result->type = R_opt;
    result->value = value;
    gc_addTmpLink(&result->value->gc_status);

    error_:
    gc_freeTmpLink(&name_->gc_status);
    memFree(str_name);
    return result->type;
}

ResultType listAss(Statement *name, LinkValue *value, FUNC_NT) {
    Parameter *pt = NULL;
    Argument *call = NULL;
    Statement *tmp_st = makeBaseLinkValueStatement(value, name->line, name->code_file);

    setResultCore(result);
    pt = makeArgsParameter(tmp_st);
    call = getArgument(pt, false, CNEXT_NT);
    if (!CHECK_RESULT(result))
        goto return_;

    freeResult(result);
    setParameterCore(name->line, name->code_file, call, name->u.base_list.list, var_list, CNEXT_NT);
    if (CHECK_RESULT(result)){
        freeResult(result);
        makeListValue(call, name->line, name->code_file, L_tuple, CNEXT_NT);
    }
    return_:
    freeArgument(call, false);
    freeParameter(pt, true);
    return result->type;
}

ResultType downAss(Statement *name, LinkValue *value, FUNC_NT) {
    LinkValue *iter = NULL;
    LinkValue *_func_ = NULL;
    Parameter *pt = name->u.slice_.index;

    setResultCore(result);
    if (optSafeInterStatement(CFUNC(name->u.slice_.element, var_list, result, belong)))
        return result->type;
    GET_RESULT(iter, result);

    if (name->u.slice_.type == SliceType_down_)
        _func_ = findAttributes(inter->data.mag_func[M_DOWN_ASSIGMENT], false, LINEFILE, true, CFUNC_NT(var_list, result, iter));
    else
        _func_ = findAttributes(inter->data.mag_func[M_SLICE_ASSIGMENT], false, LINEFILE, true, CFUNC_NT(var_list, result, iter));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_func_ != NULL){
        Argument *arg = makeValueArgument(value);
        gc_addTmpLink(&_func_->gc_status);
        arg->next = getArgument(pt, false, CNEXT_NT);
        if (!CHECK_RESULT(result))
            goto daerror_;

        freeResult(result);
        callBackCore(_func_, arg, name->line, name->code_file, 0, CNEXT_NT);

        daerror_:
        freeArgument(arg, true);
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultErrorSt(E_TypeException, OBJ_NOTSUPPORT(assignment(__down_assignment__/__slice_assignment__)), true, name, CNEXT_NT);

    return_:
    gc_freeTmpLink(&iter->gc_status);
    return result->type;
}

ResultType pointAss(Statement *name, LinkValue *value, FUNC_NT) {
    LinkValue *left;
    Statement *right = name->u.operation.right;
    VarList *object = NULL;

    setResultCore(result);
    if (optSafeInterStatement(CFUNC(name->u.operation.left, var_list, result, belong)))
        return result->type;
    GET_RESULT(left, result);  // 不关心左值和整体(st)权限

    object = name->u.operation.OperationType == OPT_POINT ? left->value->object.var : left->value->object.out_var;
    if (object == NULL) {
        setResultError(E_TypeException, OBJ_NOTSUPPORT(->/.), name->line, name->code_file, true, CNEXT_NT);
        goto return_;
    }

    if (right->type == T_OPERATION && (right->u.operation.OperationType == OPT_POINT || right->u.operation.OperationType == OPT_OUTPOINT))
        pointAss(name->u.operation.right, value, CFUNC_NT(object, result, belong));
    else
        assCore(name->u.operation.right, value, true, false, CFUNC_NT(object, result, belong));

    return_:
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}

static ResultType setNameException(LinkValue *val, wchar_t *name, fline line, char *file, FUNC_NT) {
    Result tmp;
    LinkValue *_attr_;
    wchar_t *message = memWidecat(L"Variable not found: ", name, false, false);
    setResultCore(&tmp);
    gc_addTmpLink(&val->gc_status);

    setResultError(E_NameExceptiom, message, line, file, true, CNEXT_NT);
    addAttributes(inter->data.mag_func[M_VAL], false, val, line, file, true, CFUNC_NT(var_list, &tmp, result->value));  // 将错误设置为__val__
    if (!RUN_TYPE(tmp.type)) {
        freeResult(result);
        *result = tmp;
        goto return_;
    }

    freeResult(&tmp);
    _attr_ = findAttributes(inter->data.mag_func[M_ATTR], false, LINEFILE, true, CFUNC_NT(var_list, &tmp, belong));
    if (!RUN_TYPE(tmp.type)) {  // 回调__attr__并把错误作为参数
        freeResult(result);
        *result = tmp;
        goto return_;
    }

    freeResult(&tmp);
    if (_attr_ != NULL) {
        Argument *arg = makeValueArgument(result->value);
        freeResult(result);
        gc_addTmpLink(&_attr_->gc_status);
        callBackCore(_attr_, arg, line, file, 0, CFUNC_NT(var_list, result, belong));
        gc_freeTmpLink(&_attr_->gc_status);
        freeArgument(arg, true);
    }

    return_:
    gc_freeTmpLink(&val->gc_status);
    memFree(message);
    return result->type;
}

ResultType getVar(FUNC, VarInfo var_info) {
    int int_times = 0;
    wchar_t *name = NULL;
    LinkValue *var;
    LinkValue *name_;
    Var *re_var;

    setResultCore(result);

    var_info(&name, &int_times, CNEXT);
    if (!CHECK_RESULT(result)) {
        memFree(name);
        return result->type;
    }
    GET_RESULT(name_, result);
    var = findFromVarList(name, int_times, &re_var, read_var, CFUNC_CORE(var_list));

    if (var == NULL) {
        if (st->type == base_svar && !st->u.base_svar.is_var) {
            setResultOperationBase(result, name_);
        } else
            setNameException(name_, name, st->line, st->code_file, CNEXT_NT);
    } else if (checkAut(st->aut, var->aut, st->line, st->code_file, name, true, CNEXT_NT)) {  // 默认访问权限为pri
        if (var->belong != NULL && var->belong->value != belong->value && checkAttribution(belong->value, var->belong->value)) { // 检查var所属于的对象是否位左值的父亲(若是则需要重新设定belong)
            var = COPY_LINKVALUE(var, inter);
            var->belong = belong;
        }

        bool run = st->type == base_var ? st->u.base_var.run : st->type == base_svar ? st->u.base_svar.run : false;
        if (!run || !runVarFunc(var, st->line, st->code_file, CNEXT_NT))
            setResultOperationBase(result, var);
    }

    gc_freeTmpLink(&name_->gc_status);
    memFree(name);
    return result->type;
}

ResultType getBaseValue(FUNC) {
    setResultCore(result);
    if (st->u.base_value.type == link_value) {
        result->value = st->u.base_value.value;
        result->type = R_opt;
        gc_addTmpLink(&result->value->gc_status);
    }
    else {
        switch (st->u.base_value.type) {
            case number_str: {
                if (wcschr(st->u.base_value.str, '.') == NULL)
                    makeIntValue(wcstoll(st->u.base_value.str, NULL, 10), st->line, st->code_file, CNEXT_NT);
                else
                    makeDouValue(wcstold(st->u.base_value.str, NULL), st->line, st->code_file, CNEXT_NT);
                break;
            }
            case bool_true:
                makeBoolValue(true, st->line, st->code_file, CNEXT_NT);
                break;
            case bool_false:
                makeBoolValue(false, st->line, st->code_file, CNEXT_NT);
                break;
            case pass_value:
                makePassValue(st->line, st->code_file, CNEXT_NT);
                break;
            case null_value:
                useNoneValue(inter, result);
                break;
            default:
                makeStringValue(st->u.base_value.str, st->line, st->code_file, CNEXT_NT);
                break;
        }
        result->value->belong = belong;
        if (inter->data.value_folding) {  // 常量折叠
            memFree(st->u.base_value.str);
            st->u.base_value.type = link_value;
            st->u.base_value.value = result->value;
            st->u.base_value.str = NULL;
            gc_addStatementLink(&result->value->gc_status);
        }
    }
    return result->type;
}

ResultType getList(FUNC) {
    Argument *at = NULL;
    Argument *at_tmp = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_list.list, false, CNEXT_NT);
    at_tmp = at;
    if (!CHECK_RESULT(result)){
        freeArgument(at_tmp, false);
        return result->type;
    }
    freeResult(result);
    makeListValue(at, st->line, st->code_file, st->u.base_list.type, CNEXT_NT);
    freeArgument(at_tmp, false);

    return result->type;
}

ResultType getDict(FUNC) {
    Argument *at = NULL;

    setResultCore(result);
    at = getArgument(st->u.base_dict.dict, true, CNEXT_NT);
    if (!CHECK_RESULT(result)) {
        freeArgument(at, false);
        return result->type;
    }

    freeResult(result);
    Value *tmp_value = makeDictValue(at, true, st->line, st->code_file, CNEXT_NT);
    if (!CHECK_RESULT(result)) {
        freeArgument(at, false);
        return result->type;
    }

    freeResult(result);
    LinkValue *value = makeLinkValue(tmp_value, belong, auto_aut, inter);
    setResultOperation(result, value);
    freeArgument(at, false);

    return result->type;
}

ResultType setDefault(FUNC){
    enum DefaultType type = st->u.default_var.default_type;
    int base = 0;  // 用于nonlocal和global
    setResultCore(result);
    if (type == global_)
        for (VarList *tmp = var_list; tmp->next != NULL; tmp = tmp->next)
            base++;
    else if (type == nonlocal_)
        base = 1;
    for (Parameter *pt = st->u.default_var.var; pt != NULL; pt = pt->next){
        wchar_t *name = NULL;
        int times = 0;
        freeResult(result);
        getVarInfo(&name, &times, CFUNC(pt->data.value, var_list, result, belong));
        if (!CHECK_RESULT(result))
            break;
        if (type != default_)
            times = base;
        var_list->default_var = connectDefaultVar(var_list->default_var, name, times);
        memFree(name);
    }
    return result->type;
}

bool getLeftRightValue(Result *left, Result *right, FUNC){
    if (optSafeInterStatement(CFUNC(st->u.operation.left, var_list, result, belong)))
        return true;
    *left = *result;
    setResultCore(result);

    if (optSafeInterStatement(CFUNC(st->u.operation.right, var_list, result, belong)))
        return true;
    *right = *result;
    setResultCore(result);
    return false;
}

ResultType runOperationFromValue(LinkValue *self, LinkValue *left, LinkValue *right, wchar_t *name, fline line, char *file, FUNC_NT) {
    LinkValue *_func_;
    setResultCore(result);
    gc_addTmpLink(&self->gc_status);
    gc_addTmpLink(&left->gc_status);
    gc_addTmpLink(&right->gc_status);

    _func_ = findAttributes(name, false, LINEFILE, true, CFUNC_NT(var_list, result, self));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_func_ != NULL){
        Argument *f_arg = NULL;
        f_arg = makeValueArgument(left);
        if (right != NULL)
            f_arg->next = makeValueArgument(right);
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, f_arg, line, file, 0, CNEXT_NT);
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(f_arg, true);
    }
    else {
        wchar_t *message = memWidecat(L"Object not support ", name, false, false);
        setResultError(E_TypeException, message, line, file, true, CNEXT_NT);
        memFree(message);
    }
    return_:
    gc_freeTmpLink(&self->gc_status);
    gc_freeTmpLink(&left->gc_status);
    gc_freeTmpLink(&right->gc_status);
    return result->type;
}

ResultType runOperationFromValue2(LinkValue *self, wchar_t *name, fline line, char *file, FUNC_NT) {
    LinkValue *_func_;
    gc_addTmpLink(&self->gc_status);
    setResultCore(result);

    _func_ = findAttributes(name, false, LINEFILE, true, CFUNC_NT(var_list, result, self));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_func_ != NULL){
        Argument *f_arg = NULL;
        f_arg = makeValueArgument(self);
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, f_arg, line, file, 0, CNEXT_NT);
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(f_arg, true);
    }
    else {
        wchar_t *message = memWidecat(L"Object not support ", name, false, false);
        setResultError(E_TypeException, message, line, file, true, CNEXT_NT);
        memFree(message);
    }
    return_:
    gc_freeTmpLink(&self->gc_status);
    return result->type;
}

#define OPT_CASE(NAME, FUNC_NAME) case OPT_##NAME: vobject_##FUNC_NAME##_base(CFUNC_VOBJ_LEFT(var_list, result, belong, left.value, right.value)); break
static ResultType operationCore(FUNC, wchar_t *name, enum OperationType type) {
    Result left;
    Result right;
    setResultCore(&left);
    setResultCore(&right);
    setResultCore(result);

    if (getLeftRightValue(&left, &right, CNEXT))  // 不需要释放result
        return result->type;

    if (IS_BUILTIN_VALUE(left.value->value, inter)) {
        switch (type) {
            OPT_CASE(ADD, add);
            OPT_CASE(SUB, sub);
            OPT_CASE(MUL, mul);
            OPT_CASE(DIV, div);
            OPT_CASE(INTDIV, intdiv);
            OPT_CASE(MOD, mod);
            OPT_CASE(POW, pow);
            OPT_CASE(EQ, eq);
            OPT_CASE(NOTEQ, noteq);
            OPT_CASE(MOREEQ, moreeq);
            OPT_CASE(LESSEQ, lesseq);
            OPT_CASE(MORE, more);
            OPT_CASE(LESS, less);
            OPT_CASE(BAND, band);
            OPT_CASE(BOR, bor);
            OPT_CASE(BXOR, bxor);
            OPT_CASE(BL, bl);
            OPT_CASE(BR, br);
            default:
                goto default_mode;
        }
    } else
        default_mode: runOperationFromValue(left.value, left.value, right.value, name, st->line, st->code_file, CNEXT_NT);

    if (inter->data.opt_folding && st->u.operation.left->type == base_value && st->u.operation.right->type == base_value) {  // 常量表达式折叠
        freeStatement(st->u.operation.left);
        freeStatement(st->u.operation.right);
        st->type = base_value;
        st->u.base_value.str = NULL;
        st->u.base_value.value = result->value;
        st->u.base_value.type = link_value;
        gc_addStatementLink(&result->value->gc_status);
    }

    freeResult(&left);
    freeResult(&right);
    return result->type;
}
#undef OPT_CASE

#define OPT_CASE(NAME, FUNC_NAME) case OPT_##NAME: vobject_##FUNC_NAME##_base(CFUNC_VOBJR(var_list, result, belong, left)); break
static ResultType operationCore2(FUNC, wchar_t *name, enum OperationType type) {
    LinkValue *left;
    setResultCore(result);

    if (optSafeInterStatement(CFUNC(st->u.operation.left, var_list, result, belong)))
        return result->type;
    GET_RESULTONLY(left, result);  // 不使用freeResult, 不需要多余的把result.value设置为none

    if (IS_BUILTIN_VALUE(left->value, inter)) {
        switch (type) {
            OPT_CASE(BNOT, bnot);
            OPT_CASE(NEGATE, negate);
            default:
                goto default_mode;
        }
    } else
        default_mode: runOperationFromValue2(left, name, st->line, st->code_file, CNEXT_NT);
    gc_freeTmpLink(&left->gc_status);
    return result->type;
}
#undef OPT_CASE
