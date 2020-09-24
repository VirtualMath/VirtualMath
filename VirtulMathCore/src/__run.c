#include "__run.h"

ResultType getBaseVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG){
    *name = setStrVarName(st->u.base_var.name, false, inter);
    *times = 0;
    if (st->u.base_var.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_var.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultErrorSt(E_TypeException, "Variable operation got unsupported number of layers", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;
    freeResult(result);

    not_times:
    makeStringValue(st->u.base_var.name, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return result->type;
}

ResultType getBaseSVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG){
    freeResult(result);

    if (st->u.base_svar.times == NULL){
        *times = 0;
        goto not_times;
    }
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.times, var_list, result, belong)))
        return result->type;
    if (!isType(result->value->value, number)){
        setResultErrorSt(E_TypeException, "Variable operation got unsupported number of layers", true, st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return result->type;
    }
    *times = (int)result->value->value->data.num.num;

    freeResult(result);
    not_times:
    if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st->u.base_svar.name, var_list, result, belong)))
        return result->type;

    *name = getNameFromValue(result->value->value, inter);
    result->type = operation_return;  // 执行 operationSafeInterStatement 的时候已经初始化 result

    return result->type;
}

ResultType getVarInfo(wchar_t **name, int *times, INTER_FUNCTIONSIG){
    if (st->type == base_var)
        getBaseVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
    else if (st->type == base_svar)
        getBaseSVarInfo(name, times, CALL_INTER_FUNCTIONSIG(st, var_list, result, belong));
    else{
        if (operationSafeInterStatement(CALL_INTER_FUNCTIONSIG(st, var_list, result, belong)))
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
        case string:
            return setStrVarName(value->data.str.str, true, inter);  // TODO-szh VAR使用宽字符
        case number:
            return setNumVarName(value->data.num.num, inter);
        case bool_:
            if (value->data.bool_.bool_)
                return memWidecat(inter->data.var_bool_prefix, L"true", false, false);
            else
                return memWidecat(inter->data.var_bool_prefix, L"false", false, false);
        case none:
            return memWidecpy(inter->data.var_none);
        case pass_:
            return memWidecpy(inter->data.var_pass);
        case class:{
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

bool popStatementVarList(Statement *funtion_st, VarList **function_var, VarList *out_var, Inter *inter){
    bool yield_run;
    if ((yield_run = funtion_st->info.have_info)) {
        *function_var = funtion_st->info.var_list;
        (*function_var)->next = out_var;
    }
    else
        *function_var = pushVarList(out_var, inter);
    return yield_run;
}

void newFunctionYield(Statement *funtion_st, Statement *node, VarList *new_var, Inter *inter){
    new_var->next = NULL;
    gc_freeze(inter, new_var, NULL, true);
    funtion_st->info.var_list = new_var;
    funtion_st->info.node = node->type == yield_code ? node->next : node;
    funtion_st->info.have_info = true;
}

void updateFunctionYield(Statement *function_st, Statement *node){
    function_st->info.node = node->type == yield_code ? node->next : node;
    function_st->info.have_info = true;
}

ResultType setFunctionArgument(Argument **arg, Argument **base, LinkValue *_func, fline line, char *file, int pt_sep, INTER_FUNCTIONSIG_NOT_ST){
    Argument *tmp = NULL;
    LinkValue *self;
    LinkValue *func;
    enum FunctionPtType pt_type = _func->value->data.function.function_data.pt_type;
    setResultCore(result);

    switch (pt_sep) {
        case 0:
            func = _func;
            self = _func->belong;
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
                setResultError(E_ArgumentException, FEW_ARG, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
                return error_return;
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
            setResultError(E_ArgumentException, MANY_ARG, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
    }

    if (pt_type != free_ && self == NULL) {
        setResultError(E_ArgumentException, "Function does not belong to anything(not self)", line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    switch (pt_type) {
        case static_:
            tmp = makeValueArgument(func);
            tmp->next = *arg;
            *arg = tmp;
            break;
        case class_static_:
            tmp = makeValueArgument(func);
            if (self->value->type != class) {
                self = NULL;
                for (Inherit *ih = self->value->object.inherit; ih != NULL; ih = ih->next)  // 使用循环的方式检查
                    if (ih->value->value->type == class) {
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
        case all_static_:
            tmp = makeValueArgument(func);
            tmp->next = makeValueArgument(self);
            tmp->next->next = *arg;
            *arg = tmp;
            break;
        case object_static_:
            tmp = makeValueArgument(func);
            if (self->value->type != class){
                tmp->next = makeValueArgument(self);
                tmp->next->next = *arg;
            }
            else
                tmp->next = *arg;
            *arg = tmp;
            break;
        case class_free_:
            if (self->value->type != class){
                self = NULL;
                for (Inherit *ih = self->value->object.inherit; ih != NULL; ih = ih->next)  // 循环检查
                    if (ih->value->value->type == class) {
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
            if (self->value->type != class) {
                tmp = makeValueArgument(self);
                tmp->next = *arg;
                *arg = tmp;
            }
            break;
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

LinkValue *findStrVar(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE){
    LinkValue *tmp = NULL;
    wchar_t *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, get_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(name_);
    return tmp;
}

LinkValue *checkStrVar(wchar_t *name, bool free_old, INTER_FUNCTIONSIG_CORE){
    LinkValue *tmp = NULL;
    wchar_t *name_ = setStrVarName(name, free_old, inter);
    tmp = findFromVarList(name_, 0, read_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(name_);
    return tmp;
}

void addStrVarCore(int setting, wchar_t *var_name, LinkValue *name_, LinkValue *value, fline line, char *file, VarList *out_var, INTER_FUNCTIONSIG_NOT_ST) {
    addFromVarList(var_name, name_, 0, value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    out_var = out_var == NULL ? var_list : out_var;
    if (setting)
        newObjectSetting(name_, line, file, value, result, inter, out_var);
    else
        setResult(result, inter, belong);
}

void addStrVar(wchar_t *name, bool free_old, bool setting, LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *name_;
    wchar_t *var_name = setStrVarName(name, free_old, inter);
    setResultCore(result);

    makeStringValue(var_name, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    name_ = result->value;
    result->value = NULL;
    freeResult(result);

    addStrVarCore(setting, var_name, name_, value, line, file, NULL, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    gc_freeTmpLink(&name_->gc_status);
    return_:
    memFree(var_name);
}

LinkValue *findAttributes(wchar_t *name, bool free_old, LinkValue *value, Inter *inter) {
    LinkValue *attr = findStrVar(name, free_old, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
    if (attr != NULL && (attr->belong == NULL || attr->belong->value != value->value && checkAttribution(value->value, attr->belong->value)))
        attr->belong = value;
    return attr;
}

bool addAttributes(wchar_t *name, bool free_old, LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    wchar_t *var_name = setStrVarName(name, free_old, inter);
    LinkValue *name_;
    setResultCore(result);

    makeStringValue(var_name, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        goto return_;

    name_ = result->value;
    result->value = NULL;
    freeResult(result);

    gc_freeze(inter, var_list, belong->value->object.var, true);
    addStrVarCore(false, var_name, name_, value, line, file, var_list, CALL_INTER_FUNCTIONSIG_NOT_ST(belong->value->object.var, result, belong));
    gc_freeze(inter, var_list, belong->value->object.var, false);

    gc_freeTmpLink(&name_->gc_status);
    return_:
    memFree(var_name);
    return CHECK_RESULT(result);
}

void newObjectSetting(LinkValue *name, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    setResultCore(result);
    addAttributes(inter->data.object_name, false, name, line, file, belong, result, inter, var_list);
    if (CHECK_RESULT(result))
        return;
    freeResult(result);
    addAttributes(inter->data.object_self, false, belong, line, file, belong, result, inter, var_list);
    if (CHECK_RESULT(result) && belong->value->object.inherit != NULL) {
        freeResult(result);
        addAttributes(inter->data.object_father, false, belong->value->object.inherit->value, line, file, belong,
                      result, inter, var_list);
    }
}


ResultType getElement(LinkValue *from, LinkValue *index, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    gc_addTmpLink(&from->gc_status);
    gc_addTmpLink(&index->gc_status);

    _func_ = findAttributes(inter->data.object_down, false, from, inter);
    if (_func_ != NULL){
        Argument *arg = NULL;
        gc_addTmpLink(&_func_->gc_status);
        arg = makeValueArgument(index);
        callBackCore(_func_, arg, line, file, 0, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
        freeArgument(arg, true);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(subscript(__down__)), line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    gc_freeTmpLink(&from->gc_status);
    gc_freeTmpLink(&index->gc_status);
    return result->type;
}

ResultType getIter(LinkValue *value, int status, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *_func_ = NULL;
    setResultCore(result);
    if (status == 1)
        _func_ = findAttributes(inter->data.object_iter, false, value, inter);
    else
        _func_ = findAttributes(inter->data.object_next, false, value, inter);

    if (_func_ != NULL){
        gc_addTmpLink(&_func_->gc_status);
        callBackCore(_func_, NULL, line, file, 0, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_func_->gc_status);
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(iter), line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    return result->type;
}

bool checkBool(LinkValue *value, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST){
    LinkValue *_bool_ = findAttributes(inter->data.object_bool, false, value, inter);
    if (_bool_ != NULL){
        gc_addTmpLink(&_bool_->gc_status);
        callBackCore(_bool_, NULL, line, file, 0, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_bool_->gc_status);
        if (result->value->value->type != bool_)
            setResultError(E_TypeException, RETURN_ERROR(__bool__, bool), line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else
            return result->value->value->data.bool_.bool_;
    } else {
        makeBoolValue(true, 0, "sys.bool", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return true;
    }
    return false;
}

wchar_t *getRepoStr(LinkValue *value, bool is_repo, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST){
    LinkValue *_repo_ = findAttributes(is_repo ? inter->data.object_repo : inter->data.object_str, false, value, inter);
    setResultCore(result);
    if (_repo_ != NULL){
        gc_addTmpLink(&value->gc_status);
        gc_addTmpLink(&_repo_->gc_status);
        callBackCore(_repo_, NULL, line, file, 0, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_repo_->gc_status);
        gc_freeTmpLink(&value->gc_status);
        if (!CHECK_RESULT(result))
            return NULL;
        else if (result->value->value->type != string){
            setResultError(E_TypeException, OBJ_NOTSUPPORT(repo(str)), line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return NULL;
        }
        return result->value->value->data.str.str;
    }
    else
        setResultError(E_TypeException, OBJ_NOTSUPPORT(repo(str)), line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    return NULL;
}

bool is_iterStop(LinkValue *value, Inter *inter) {
    return value->value == inter->data.iterstop_exc->value || checkAttribution(value->value, inter->data.iterstop_exc->value);
}

bool is_indexException(LinkValue *value, Inter *inter) {
    return value->value == inter->data.index_exc->value || checkAttribution(value->value, inter->data.index_exc->value);
}

bool checkAut(enum ValueAuthority value, enum ValueAuthority base, fline line, char *file, char *name, bool pri_auto, INTER_FUNCTIONSIG_NOT_ST) {
    if ((value == public_aut || (!pri_auto && value == auto_aut)) && (base != public_aut && base != auto_aut)) {
        if (name == NULL)
            setResultError(E_PermissionsException, "Wrong Permissions: access variables as public", line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else {
            char *message = memStrcat("Wrong Permissions: access variables as public: ", name, false, false);
            setResultError(E_PermissionsException, message, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            memFree(message);
        }
        return false;
    }
    else if ((value == protect_aut) && (base == private_aut)) {
        if (name == NULL)
            setResultError(E_PermissionsException, "Wrong Permissions: access variables as protect", line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        else {
            char *message = memStrcat("Wrong Permissions: access variables as protect: ", name, false, false);
            setResultError(E_PermissionsException, message, line, file, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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

int init_new(LinkValue *obj, Argument *arg, char *message, INTER_FUNCTIONSIG_NOT_ST){
    LinkValue *_init_ = NULL;
    _init_ = findAttributes(inter->data.object_init, false, obj, inter);

    if (_init_ == NULL) {
        if (arg != NULL) {
            setResultError(E_ArgumentException, MANY_ARG, 0, message, true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return 0;
        } else
            return 1;
    }
    _init_->belong = obj;
    gc_addTmpLink(&_init_->gc_status);
    callBackCore(_init_, arg, 0, message, 0, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, obj));
    gc_freeTmpLink(&_init_->gc_status);
    return CHECK_RESULT(result) ? 1 : -1;
}

bool setBoolAttrible(bool value, wchar_t *var, fline line, char *file, LinkValue *obj, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *bool_value = NULL;
    setResultCore(result);
    makeBoolValue(value, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return false;
    bool_value = result->value;
    freeResult(result);
    if (!addAttributes(var, false, bool_value, line, file, obj, result, inter, var_list))
        return false;
    freeResult(result);
    return true;
}
