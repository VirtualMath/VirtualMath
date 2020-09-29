#include "__run.h"

ResultType getBaseVarInfo(wchar_t **name, int *times, FUNC){
    *name = setStrVarName(st->u.base_var.name, false, inter);
    *times = 0;
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CFUNC(st->u.base_var.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, V_num)){
        setResultErrorSt(E_TypeException, L"Variable operation got unsupported V_num of layers", true, st, CFUNC_NT(var_list, result, belong));
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    makeStringValue(st->u.base_var.name, st->line, st->code_file, CFUNC_NT(var_list, result, belong));
    return result->type;
}

ResultType getBaseSVarInfo(wchar_t **name, int *times, FUNC){
    freeResult(result);

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CFUNC(st->u.base_svar.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, V_num)){
        setResultErrorSt(E_TypeException, L"Variable operation got unsupported V_num of layers", true, st, CFUNC_NT(var_list, result, belong));
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;

    freeResult(result);
    not_times:
    if (operationSafeInterStatement(CFUNC(st->u.base_svar.name, var_list, result, belong)))
        return result->type;

    *name = getNameFromValue(result->value->value, inter);
    result->type = R_opt;  // 执行 operationSafeInterStatement 的时候已经初始化 result

    return result->type;
}

ResultType getVarInfo(wchar_t **name, int *times, FUNC){
    if (st->type == base_var)
        getBaseVarInfo(name, times, CFUNC(st, var_list, result, belong));
    else if (st->type == base_svar)
        getBaseSVarInfo(name, times, CFUNC(st, var_list, result, belong));
    else{
        if (operationSafeInterStatement(CFUNC(st, var_list, result, belong)))
            return result->type;
        *name = getNameFromValue(result->value->value, inter);
        *times = 0;
    }
    return result->type;
}

wchar_t *setStrVarName(wchar_t *old, bool free_old, Inter *inter) {
    return memWidecat(inter->data.var_str_prefix, old, false, free_old);
}

wchar_t *setNumVarName(vnum num, struct Inter *inter) {
    wchar_t name[50];
    swprintf(name, 50, L"%lld", num);
    return memWidecat(inter->data.var_num_prefix, name, false, false);
}

wchar_t *getNameFromValue(Value *value, struct Inter *inter) {
    switch (value->type){
        case V_str:
            return setStrVarName(value->data.str.str, true, inter);
        case V_num:
            return setNumVarName(value->data.num.num, inter);
        case V_bool:
            if (value->data.bool_.bool_)
                return memWidecat(inter->data.var_bool_prefix, L"true", false, false);
            else
                return memWidecat(inter->data.var_bool_prefix, L"false", false, false);
        case V_none:
            return memWidecpy(inter->data.var_none);
        case V_ell:
            return memWidecpy(inter->data.var_pass);
        case V_class:{
            size_t len = memWidelen(inter->data.var_class_prefix) + 20;  // 预留20个字节给指针
            wchar_t *name = memWide(len);
            wchar_t *return_ = NULL;
            swprintf(name, len, L"%ls%p", inter->data.var_class_prefix, value);
            return_ = memWidecpy(name);  // 再次复制去除多余的空字节
            memFree(name);
            return return_;
        }
        default:{
            size_t len = memWidelen(inter->data.var_object_prefix) + 20;
            wchar_t *name = memWide(len);
            wchar_t *return_ = NULL;
            swprintf(name, len, L"%ls%p", inter->data.var_object_prefix, value);
            return_ = memWidecpy(name);  // 再次复制去除多余的空字节
            memFree(name);
            return return_;
        }
    }
}

/**
 * 获取Statement的VarList(若yield)， 否则push out_var(若不为yield)
 * @param return_ VarList的返回值
 * @return 是否位yield模式
 */
bool popYieldVarList(Statement *st, VarList **return_, VarList *out_var, Inter *inter){
    bool yield_run;
    if ((yield_run = st->info.have_info)) {
        *return_ = st->info.var_list;
        (*return_)->next = out_var;
    }
    else
        *return_ = pushVarList(out_var, inter);
    return yield_run;
}

ResultType setFunctionArgument(Argument **arg, Argument **base, LinkValue *_func, fline line, char *file, int pt_sep, FUNC_NT){
    Argument *tmp = NULL;
    LinkValue *self;
    LinkValue *func;
    enum FunctionPtType pt_type = _func->value->data.function.function_data.pt_type;
    setResultCore(result);

    switch (pt_sep) {
        case 0:
            func = _func;
            self = pt_type == cls_free_ || pt_type == cls_static_ ? _func->value->data.function.function_data.cls : _func->belong;
            *base = *arg;
            break;
        case 1: {
            if (*arg != NULL) {
                if (pt_type == static_) {
                    func = (*arg)->data.value;
                    self = NULL;  // static_模式不需要self
                }
                else {
                    func = _func;
                    self = (*arg)->data.value;
                }
                *arg = (*arg)->next;  // 忽略第一个arg, 但是不释放(在该函数外部统一释放)
                *base = *arg;
            } else {
                error_:
                setResultError(E_ArgumentException, FEW_ARG, line, file, true, CFUNC_NT(var_list, result, belong));
                return R_error;
            }
            break;
        }
        case 2: {
            if (*arg != NULL && (*arg)->next != NULL) {
                func = (*arg)->data.value;
                self = (*arg)->next->data.value;  // 第一个参数是func, 第二个是self; 这样做保证了和形参调用的一致

                *arg = (*arg)->next->next;
                *base = *arg;
            } else
                goto error_;
            break;
        }
        default:
            setResultError(E_ArgumentException, MANY_ARG, line, file, true, CFUNC_NT(var_list, result, belong));
            return R_error;
    }

    if (pt_type != free_ && self == NULL) {
        setResultError(E_ArgumentException, L"Function does not belong to anything(not self)", line, file, true, CFUNC_NT(var_list, result, belong));
        return R_error;
    }

    switch (pt_type) {
        case static_:
            tmp = makeValueArgument(func);
            tmp->next = *arg;
            *arg = tmp;
            break;
        case class_static_:
            tmp = makeValueArgument(func);
            if (self->value->type != V_class) {
                Inherit *ih = self->value->object.inherit;
                self = NULL;
                for (PASS; ih != NULL; ih = ih->next)  // 使用循环的方式检查
                    if (ih->value->value->type == V_class) {
                        self = ih->value;
                        break;
                    }
            }

            if (self != NULL) {
                tmp->next = makeValueArgument(self);
                tmp->next->next = *arg;
            } else  // 若未检查到class, 则放弃该形参(由原arg补上)
                tmp->next = *arg;
            *arg = tmp;
            break;
        case cls_static_:
        case all_static_:
            tmp = makeValueArgument(func);
            tmp->next = makeValueArgument(self);
            tmp->next->next = *arg;
            *arg = tmp;
            break;
        case object_static_:
            tmp = makeValueArgument(func);
            if (self->value->type != V_class){
                tmp->next = makeValueArgument(self);
                tmp->next->next = *arg;
            }
            else
                tmp->next = *arg;
            *arg = tmp;
            break;
        case class_free_:
            if (self->value->type != V_class){
                Inherit *ih = self->value->object.inherit;
                self = NULL;
                for (PASS; ih != NULL; ih = ih->next)  // 循环检查
                    if (ih->value->value->type == V_class) {
                        self = ih->value;
                        break;
                    }
            }

            if (self != NULL) {  // 若检查到class
                tmp = makeValueArgument(self);
                tmp->next = *arg;
                *arg = tmp;
            }  // 若无class则不对arg做任何调整
            break;
        case object_free_:
            if (self->value->type != V_class) {
                tmp = makeValueArgument(self);
                tmp->next = *arg;
                *arg = tmp;
            }
            break;
        case cls_free_:
        case all_free_:
            tmp = makeValueArgument(self);
            tmp->next = *arg;
            *arg = tmp;
            break;
        default:
            break;
    }
    setResultBase(result, inter);
    return result->type;
}

void freeFunctionArgument(Argument *arg, Argument *base) {
    for (Argument *tmp = arg; tmp != NULL; tmp = tmp->next) {
        if (tmp->next == base) {
            tmp->next = NULL;
            freeArgument(arg, true);
            break;
        }
    }
}

LinkValue *findStrVar(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, FUNC_NT){
    LinkValue *tmp = NULL;
    wchar_t *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, get_var, CFUNC_CORE(var_list));
    memFree(name_);
    if (tmp != NULL && nowrun) {
        setResultCore(result);
        if (!runVarFunc(tmp, line, file, CFUNC_NT(var_list, result, belong)))
            setResultOperationBase(result, tmp);
    }
    return tmp;
}

LinkValue *findStrVarOnly(wchar_t *name, bool free_old, FUNC_CORE) {
    return findStrVar(name, free_old, 0, "sys", false, CFUNC_NT(var_list, NULL, NULL));
}

LinkValue *checkStrVar(wchar_t *name, bool free_old, FUNC_CORE){
    LinkValue *tmp = NULL;
    wchar_t *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, read_var, CFUNC_CORE(var_list));
    memFree(name_);
    return tmp;
}

static void addStrVarCore(int setting, wchar_t *var_name, LinkValue *name_, LinkValue *value, fline line, char *file, VarList *out_var, FUNC_NT) {
    addFromVarList(var_name, name_, 0, value, CFUNC_CORE(var_list));
    out_var = out_var == NULL ? var_list : out_var;
    if (setting)
        newObjectSetting(name_, line, file, value, result, inter, out_var);
    else
        setResult(result, inter);
}

void addStrVar(wchar_t *name, bool free_old, bool setting, LinkValue *value, fline line, char *file, bool run, FUNC_NT) {
    LinkValue *name_;
    wchar_t *var_name = setStrVarName(name, free_old, inter);
    setResultCore(result);

    if (run) {
        LinkValue *tmp = findFromVarList(name, 0, read_var, CFUNC_CORE(var_list));
        if (tmp != NULL && !setVarFunc(tmp, value, line, file, CFUNC_NT(var_list, result, belong)))
            goto return_;
    }

    makeStringValue(name, line, file, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    name_ = result->value;
    result->value = NULL;
    freeResult(result);
    addStrVarCore(setting, var_name, name_, value, line, file, NULL, CFUNC_NT(var_list, result, belong));
    gc_freeTmpLink(&name_->gc_status);

    return_:
    memFree(var_name);
}

LinkValue *findAttributes(wchar_t *name, bool free_old, fline line, char *file, bool nowrun, FUNC_NT) {  // TODO-szh 处理调用该函数的地方释放result
    LinkValue *attr;
    gc_freeze(inter, var_list, belong->value->object.var, true);
    attr = findStrVar(name, free_old, line, file, false, CFUNC_NT(belong->value->object.var, result, belong));  // TODO-szh 重新启动
    if (attr != NULL && (attr->belong == NULL || attr->belong->value != belong->value && checkAttribution(belong->value, attr->belong->value)))
        attr->belong = belong;
    gc_freeze(inter, var_list, belong->value->object.var, false);
    return attr;
}

bool addAttributes(wchar_t *name, bool free_old, LinkValue *value, fline line, char *file, bool run, FUNC_NT) {
    wchar_t *var_name = setStrVarName(name, free_old, inter);
    LinkValue *name_;
    setResultCore(result);

    if (run) {
        LinkValue *tmp = findFromVarList(name, 0, read_var, CFUNC_CORE(belong->value->object.var));
        if (tmp != NULL && !setVarFunc(tmp, value, line, file, CFUNC_NT(belong->value->object.var, result, belong)))
            goto return_;
    }

    makeStringValue(name, line, file, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    name_ = result->value;
    result->value = NULL;
    freeResult(result);

    gc_freeze(inter, var_list, belong->value->object.var, true);
    addStrVarCore(false, var_name, name_, value, line, file, var_list, CFUNC_NT(belong->value->object.var, result, belong));
    gc_freeze(inter, var_list, belong->value->object.var, false);

    gc_freeTmpLink(&name_->gc_status);

    return_:
    memFree(var_name);
    return CHECK_RESULT(result);
}

void newObjectSetting(LinkValue *name, fline line, char *file, FUNC_NT) {
    setResultCore(result);
    addAttributes(inter->data.object_name, false, name, line, file, false, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return;
    freeResult(result);

    addAttributes(inter->data.object_self, false, belong, line, file, false, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result) && belong->value->object.inherit != NULL) {
        freeResult(result);
        addAttributes(inter->data.object_father, false, belong->value->object.inherit->value, line, file, false,
                      CFUNC_NT(var_list, result, belong));
    }
}


ResultType getElement(LinkValue *from, LinkValue *index, fline line, char *file, FUNC_NT) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    gc_addTmpLink(&from->gc_status);
    gc_addTmpLink(&index->gc_status);

    _func_ = findAttributes(inter->data.object_down, false, 0, "sys", true, CFUNC_NT(var_list, result, from));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);
    if (_func_ != NULL){
        Argument *arg = NULL;
        gc_addTmpLink(&_func_->gc_status);
        arg = makeValueArgument(index);
        callBackCore(_func_, arg, line, file, 0, CFUNC_NT(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(subscript(__down__)), line, file, true, CFUNC_NT(var_list, result, belong));

    return_:
    gc_freeTmpLink(&from->gc_status);
    gc_freeTmpLink(&index->gc_status);
    return result->type;
}

ResultType getIter(LinkValue *value, int status, fline line, char *file, FUNC_NT) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);

    if (status == 1)
        _func_ = findAttributes(inter->data.object_iter, false, 0, "sys", true, CFUNC_NT(var_list, result, value));
    else
        _func_ = findAttributes(inter->data.object_next, false, 0, "sys", true, CFUNC_NT(var_list, result, value));

    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);
    if (_func_ != NULL){
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(iter), line, file, true, CFUNC_NT(var_list, result, belong));

    return_:
    gc_freeTmpLink(&value->gc_status);
    return result->type;
}

bool checkBool(LinkValue *value, fline line, char *file, FUNC_NT){
    LinkValue *_bool_ = findAttributes(inter->data.object_bool, false, 0, "sys", true, CFUNC_NT(var_list, result, value));
    if (!CHECK_RESULT(result))
        return false;
    if (_bool_ != NULL){
        gc_addTmpLink(&_bool_->gc_status);
        callBackCore(_bool_, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
        gc_freeTmpLink(&_bool_->gc_status);
        if (result->value->value->type != V_bool)
            setResultError(E_TypeException, RETURN_ERROR(__bool__, bool), line, file, true, CFUNC_NT(var_list, result, belong));
        else
            return result->value->value->data.bool_.bool_;
    } else {
        makeBoolValue(true, 0, "sys.bool", CFUNC_NT(var_list, result, belong));
        return true;
    }
    return false;
}

wchar_t *getRepoStr(LinkValue *value, bool is_repo, fline line, char *file, FUNC_NT){
    LinkValue *_repo_;
    wchar_t *re = NULL;
    setResultCore(result);
    gc_addTmpLink(&value->gc_status);

    _repo_ = findAttributes(is_repo ? inter->data.object_repo : inter->data.object_str, false, 0, "sys", true, CFUNC_NT(var_list, result, value));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_repo_ != NULL){
        gc_addTmpLink(&_repo_->gc_status);
        callBackCore(_repo_, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
        gc_freeTmpLink(&_repo_->gc_status);
        if (!CHECK_RESULT(result))
            goto return_;
        else if (result->value->value->type != V_str){
            setResultError(E_TypeException, OBJ_NOTSUPPORT(repo(str)), line, file, true, CFUNC_NT(var_list, result, belong));
            goto return_;
        }
        re = result->value->value->data.str.str;
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(repo(str)), line, file, true, CFUNC_NT(var_list, result, belong));

    return_:
    gc_freeTmpLink(&value->gc_status);
    return re;
}

bool is_iterStop(LinkValue *value, Inter *inter) {
    return value->value == inter->data.iterstop_exc->value || checkAttribution(value->value, inter->data.iterstop_exc->value);
}

bool is_indexException(LinkValue *value, Inter *inter) {
    return value->value == inter->data.index_exc->value || checkAttribution(value->value, inter->data.index_exc->value);
}

bool checkAut(enum ValueAuthority value, enum ValueAuthority base, fline line, char *file, char *name, bool pri_auto, FUNC_NT) {
    if ((value == public_aut || (!pri_auto && value == auto_aut)) && (base != public_aut && base != auto_aut)) {
        if (name == NULL)
            setResultError(E_PermissionsException, L"Wrong Permissions: access variables as public", line, file, true, CFUNC_NT(var_list, result, belong));
        else {
            wchar_t *message = memWidecat(L"Wrong Permissions: access variables as public: ", memStrToWcs(name, false), false, true);
            setResultError(E_PermissionsException, message, line, file, true, CFUNC_NT(var_list, result, belong));
            memFree(message);
        }
        return false;
    }
    else if ((value == protect_aut) && (base == private_aut)) {
        if (name == NULL)
            setResultError(E_PermissionsException, L"Wrong Permissions: access variables as protect", line, file, true, CFUNC_NT(var_list, result, belong));
        else {
            wchar_t *message = memWidecat(L"Wrong Permissions: access variables as protect: ", memStrToWcs(name, false), false, true);
            setResultError(E_PermissionsException, message, line, file, true, CFUNC_NT(var_list, result, belong));
            memFree(message);
        }
        return false;
    }
    return true;
}

LinkValue *make_new(Inter *inter, LinkValue *belong, LinkValue *class){
    Inherit *object_father = getInheritFromValueCore(class);
    VarList *new_var = copyVarList(class->value->object.out_var, false, inter);
    Value *new_object = makeObject(inter, NULL, new_var, object_father);
    return makeLinkValue(new_object, belong, inter);
}

static int init_new(LinkValue *obj, Argument *arg, fline line, char *file, FUNC_NT) {
    LinkValue *_init_ = NULL;
    setResultCore(result);

    _init_ = findAttributes(inter->data.object_init, false, 0, "sys", true, CFUNC_NT(var_list, result, obj));
    if (!CHECK_RESULT(result))
        return -1;
    freeResult(result);


    if (_init_ == NULL) {
        if (arg != NULL) {
            setResultError(E_ArgumentException, MANY_ARG, line, file, true, CFUNC_NT(var_list, result, belong));
            return 0;
        } else
            return 1;
    }

    gc_addTmpLink(&_init_->gc_status);
    _init_->belong = obj;
    callBackCore(_init_, arg, 0, file, 0, CFUNC_NT(var_list, result, obj));
    gc_freeTmpLink(&_init_->gc_status);

    return CHECK_RESULT(result) ? 1 : -1;
}

int run_init(LinkValue *obj, Argument *arg, fline line, char *file, FUNC_NT) {
    int return_;
    setResultCore(result);

    return_ = init_new(obj, arg, line, file, CFUNC_NT(var_list, result, belong));
    if (return_ == 1) {
        freeResult(result);
        setResultOperation(result, obj);
    }
    return return_;
}

bool setBoolAttrible(bool value, wchar_t *var, fline line, char *file, LinkValue *obj, FUNC_NT) {
    LinkValue *bool_value = NULL;
    setResultCore(result);
    makeBoolValue(value, line, file, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return false;
    bool_value = result->value;
    freeResult(result);
    if (!addAttributes(var, false, bool_value, line, file, true, CFUNC_NT(var_list, result, obj)))
        return false;
    freeResult(result);
    return true;
}

bool runVarFunc(LinkValue *var, fline line, char *file, FUNC_NT) {
    setResultCore(result);
    if (var->value->type != V_func || !var->value->data.function.function_data.run)
        return false;
    gc_addTmpLink(&var->gc_status);
    callBackCore(var, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
    gc_freeTmpLink(&var->gc_status);
    return true;
}

bool setVarFunc(LinkValue *var, LinkValue *new, fline line, char *file, FUNC_NT) {
    Argument *arg;
    setResultCore(result);
    if (var->value->type != V_func || !var->value->data.function.function_data.run)
        return false;
    gc_addTmpLink(&var->gc_status);
    gc_addTmpLink(&new->gc_status);

    arg = makeValueArgument(new);
    callBackCore(var, arg, line, file, 0, CFUNC_NT(var_list, result, belong));
    freeArgument(arg, true);

    gc_freeTmpLink(&var->gc_status);
    gc_freeTmpLink(&new->gc_status);
    return true;
}

