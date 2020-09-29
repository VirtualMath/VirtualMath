#include "__run.h"

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, Inherit *inherit) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    setGC(&tmp->gc_status);
    tmp->type = V_obj;
    tmp->gc_next = NULL;
    if (inter->data.object != NULL && inherit == NULL)
        inherit = makeInherit(inter->data.object);
    if (out_var == NULL && inherit != NULL)
        out_var = copyVarList(inherit->value->value->object.out_var, false, inter);
    tmp->object.var = makeObjectVarList(inherit, inter, object);
    tmp->object.out_var = out_var;
    tmp->object.inherit = inherit;

    if (list_tmp == NULL){
        inter->base = tmp;
        tmp->gc_last = NULL;
        goto return_;
    }
    for (PASS; list_tmp->gc_next != NULL; list_tmp = list_tmp->gc_next)
        PASS;

    list_tmp->gc_next = tmp;
    tmp->gc_last = list_tmp;

    return_:
    return tmp;
}

Value *useNoneValue(Inter *inter, Result *result) {
    LinkValue *tmp = inter->data.none;
    if (result != NULL) {
        setResultCore(result);
        result->type = R_opt;
        result->value = tmp;
        gc_addTmpLink(&result->value->gc_status);
    }
    return tmp->value;
}

Value *makeBoolValue(bool bool_num, fline line, char *file, FUNC_NT) {
    Value *tmp = NULL;
    setResultCore(result);
    callBackCore(inter->data.bool_, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.bool_.bool_ = bool_num;
    return tmp;
}

Value *makePassValue(fline line, char *file, FUNC_NT){  // TODO-szh 让切片支持该语法 检查语法解析器支持 a[::]的语法
    Value *tmp = NULL;
    setResultCore(result);
    callBackCore(inter->data.pass_, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    return tmp;
}

Value *makeNumberValue(vnum num, fline line, char *file, FUNC_NT) {
    Value *tmp = NULL;
    setResultCore(result);
    callBackCore(inter->data.num, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.num.num = num;
    return tmp;
}

Value *makeStringValue(wchar_t *str, fline line, char *file, FUNC_NT) {
    Value *tmp = NULL;
    setResultCore(result);
    callBackCore(inter->data.str, NULL, line, file, 0, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;

    tmp = result->value->value;
    memFree(tmp->data.str.str);
    tmp->data.str.str = memWidecpy(str);
    return tmp;
}

Value *makeVMFunctionValue(Statement *st, Parameter *pt, FUNC_NT) {
    Value *tmp = NULL;
    callBackCore(inter->data.function, NULL, st->line, st->code_file, 0,
                 CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.function.function = copyStatement(st);
    tmp->data.function.pt = copyParameter(pt);
    tmp->data.function.function_data.cls = belong;
    for (VarList *vl = tmp->object.out_var, *vl_next; vl != NULL; vl = vl_next) {
        vl_next = vl->next;
        freeVarList(vl);
    }
    tmp->object.out_var = copyVarList(var_list, false, inter);
    result->value->belong = belong;
    return tmp;
}

Value *makeCFunctionValue(OfficialFunction of, fline line, char *file, FUNC_NT) {
    Value *tmp = NULL;
    callBackCore(inter->data.function, NULL, line, file, 0,
                 CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.function.type = c_func;
    tmp->data.function.of = of;
    tmp->data.function.function_data.pt_type = inter->data.default_pt_type;
    tmp->data.function.function_data.cls = belong;
    for (VarList *vl = tmp->object.out_var, *vl_next; vl != NULL; vl = vl_next) {
        vl_next = vl->next;
        freeVarList(vl);
    }
    tmp->object.out_var = copyVarList(var_list, false, inter);
    result->value->belong = belong;
    return tmp;
}

LinkValue *makeCFunctionFromOf(OfficialFunction of, LinkValue *func, OfficialFunction function_new, OfficialFunction function_init, LinkValue *belong, VarList *var_list, Inter *inter) {
    Argument *arg = makeValueArgument(func);
    Argument *init_arg = NULL;
    LinkValue *return_ = NULL;
    Result result;

    setResultCore(&result);
    function_new(CO_FUNC(arg, func->value->object.var, &result, func));
    return_ = result.value;
    result.value = NULL;
    freeResult(&result);

    init_arg = makeValueArgument(return_);
    function_init(CO_FUNC(init_arg, func->value->object.var, &result, func));
    freeResult(&result);
    freeArgument(init_arg, true);
    freeArgument(arg, true);

    return_->value->data.function.type = c_func;
    return_->value->data.function.of = of;
    return_->value->data.function.function_data.pt_type = inter->data.default_pt_type;
    return_->value->data.function.function_data.cls = belong;
    for (VarList *vl = return_->value->object.out_var; vl != NULL; vl = freeVarList(vl))
        PASS;
    return_->value->object.out_var = copyVarList(var_list, false, inter);
    return_->belong = belong;
    gc_freeTmpLink(&return_->gc_status);
    return return_;
}

Value *makeClassValue(VarList *var_list, Inter *inter, Inherit *father) {
    Value *tmp;
    VarList *new_var = copyVarList(var_list, false, inter);
    tmp = makeObject(inter, NULL, new_var, father);
    tmp->type = V_class;
    return tmp;
}

Value *makeListValue(Argument *arg, fline line, char *file, enum ListType type, FUNC_NT) {
    Value *tmp = NULL;
    setResultCore(result);
    if (type == L_list)
        callBackCore(inter->data.list, arg, line, file, 0,
                     CFUNC_NT(var_list, result, belong));
    else
        callBackCore(inter->data.tuple, arg, line, file, 0,
                     CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    return tmp;
}

Value *makeDictValue(Argument *arg, bool new_hash, fline line, char *file, FUNC_NT) {
    Value *tmp = NULL;
    setResultCore(result);
    callBackCore(inter->data.dict, arg, line, file, 0, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    if (!new_hash) {
        tmp->data.dict.dict = NULL;
        tmp->data.dict.size = 0;
    }
    return tmp;
}

void freeValue(Value **value) {
    Value *free_value = *value;
    FREE_BASE(free_value, return_);
    for (VarList *tmp = free_value->object.var; tmp != NULL; tmp = freeVarList(tmp))
        PASS;
    for (VarList *tmp = free_value->object.out_var; tmp != NULL; tmp = freeVarList(tmp))
        PASS;
    for (struct Inherit *tmp = free_value->object.inherit; tmp != NULL; tmp = freeInherit(tmp))
        PASS;
    switch (free_value->type) {
        case V_str:
            memFree(free_value->data.str.str);
            break;
        case V_func: {
            freeParameter(free_value->data.function.pt, true);
            freeStatement(free_value->data.function.function);
            break;
        }
        case V_list:
            memFree(free_value->data.list.list);
            break;
        default:
            break;
    }

    if ((*value)->gc_next != NULL)
        (*value)->gc_next->gc_last = (*value)->gc_last;
    *value = (*value)->gc_next;

    memFree(free_value);
    return_: return;
}

LinkValue *makeLinkValue(Value *value, LinkValue *belong, Inter *inter){
    LinkValue *tmp;
    LinkValue *list_tmp = inter->link_base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->belong = belong;
    tmp->value = value;
    setGC(&tmp->gc_status);
    if (list_tmp == NULL){
        inter->link_base = tmp;
        tmp->gc_last = NULL;
        goto return_;
    }

    for (PASS; list_tmp->gc_next != NULL; list_tmp = list_tmp->gc_next)
        PASS;

    list_tmp->gc_next = tmp;
    tmp->gc_last = list_tmp;
    tmp->aut = auto_aut;

    return_:
    return tmp;
}

void freeLinkValue(LinkValue **value) {
    LinkValue *free_value = *value;
    FREE_BASE(free_value, return_);

    if ((*value)->gc_next != NULL)
        (*value)->gc_next->gc_last = (*value)->gc_last;
    *value = (*value)->gc_next;

    memFree(free_value);
    return_: return;
}

LinkValue *copyLinkValue(LinkValue *value, Inter *inter) {
    LinkValue *tmp = NULL;
    if (value == NULL)
        return NULL;
    tmp = makeLinkValue(value->value, value->belong, inter);
    tmp->aut = value->aut;
    return tmp;
}

void setResultCore(Result *ru) {
    ru->type = R_not;
    ru->times = 0;
    ru->is_yield = false;
    ru->error = NULL;
    ru->value = NULL;
    ru->label = NULL;
    ru->node = NULL;
}

void setResult(Result *ru, Inter *inter) {
    freeResult(ru);
    setResultBase(ru, inter);
}

void setResultBase(Result *ru, Inter *inter) {
    setResultCore(ru);
    useNoneValue(inter, ru);
}

void setResultErrorSt(BaseErrorType type, wchar_t *error_message, bool new, Statement *st, FUNC_NT) {
    setResultError(type, error_message, st->line, st->code_file, new, CFUNC_NT(var_list, result, belong));
}

LinkValue *findBaseError(BaseErrorType type, Inter *inter){
    switch (type) {
        case E_BaseException:
            return inter->data.base_exc;
        case E_Exception:
            return inter->data.exc;
        case E_TypeException:
            return inter->data.type_exc;
        case E_ArgumentException:
            return inter->data.arg_exc;
        case E_PermissionsException:
            return inter->data.per_exc;
        case E_GotoException:
            return inter->data.goto_exc;
        case E_ResultException:
            return inter->data.result_exc;
        case E_NameExceptiom:
            return inter->data.name_exc;
        case E_AssertException:
            return inter->data.assert_exc;
        case E_IndexException:
            return inter->data.index_exc;
        case E_KeyException:
            return inter->data.key_exc;
        case E_StrideException:
            return inter->data.stride_exc;
        case E_StopIterException:
            return inter->data.iterstop_exc;
        case E_SuperException:
            return inter->data.super_exc;
        case E_ImportException:
            return inter->data.import_exc;
        case E_IncludeException:
            return inter->data.include_exp;
        case E_SystemException:
            return inter->data.sys_exc;
        case E_KeyInterrupt:
            return inter->data.keyInterrupt_exc;
        case E_QuitException:
            return inter->data.quit_exc;
        default:
            return NULL;
    }
}

static wchar_t *getErrorInfo(LinkValue *exc, int type, FUNC_NT){
    wchar_t *str_name = type == 1 ? inter->data.object_name : inter->data.object_message;
    LinkValue *_info_;
    setResultCore(result);
    gc_addTmpLink(&exc->gc_status);

    _info_ = findAttributes(str_name, false, 0, "sys", true, CFUNC_NT(var_list, result, exc));
    gc_freeTmpLink(&exc->gc_status);
    if (!CHECK_RESULT(result))
        return NULL;

    if (_info_ != NULL && _info_->value->type == V_str)
        return memWidecpy(_info_->value->data.str.str);
    else
        return type == 1 ? memWidecpy(L"Error Type: Unknown") : memWidecpy(L"Error Message: Unknown");
}

void callException(LinkValue *exc, wchar_t *message, fline line, char *file, FUNC_NT) {
    LinkValue *_new_;
    wchar_t *type = NULL;
    wchar_t *error_message = NULL;
    setResultCore(result);
    gc_addTmpLink(&exc->gc_status);

    _new_ = findAttributes(inter->data.object_new, false, 0, "sys", true, CFUNC_NT(var_list, result, exc));
    if (!CHECK_RESULT(result))
        goto return_;
    freeResult(result);

    if (_new_ != NULL){
        Argument *arg = NULL;
        LinkValue *error;
        makeStringValue(message, line, file, CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        arg =  makeValueArgument(result->value);
        freeResult(result);

        gc_addTmpLink(&_new_->gc_status);
        callBackCore(_new_, arg, line, file, 0, CFUNC_NT(var_list, result, belong));
        error = result->value;
        result->value = NULL;
        freeResult(result);
        gc_freeTmpLink(&_new_->gc_status);
        freeArgument(arg, true);

        type = getErrorInfo(error, 1, CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        freeResult(result);
        error_message = getErrorInfo(error, 2, CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        freeResult(result);
    }
    else {
        result->value = exc;
        gc_addTmpLink(&result->value->gc_status);
    }

    result->type = R_error;
    result->error = connectError(makeError(type, error_message, line, file), result->error);
    memFree(type);
    memFree(error_message);
    return_: gc_freeTmpLink(&exc->gc_status);
}

void setResultError(BaseErrorType type, wchar_t *error_message, fline line, char *file, bool new, FUNC_NT) {
    if (!new && result->type != R_error)
        return;
    if (new) {
        LinkValue *exc = findBaseError(type, inter);
        if (exc == NULL)
            exc = inter->data.base_exc;
        freeResult(result);
        callException(exc, error_message, line, file, CFUNC_NT(var_list, result, belong));
    }
    else
        result->error = connectError(makeError(NULL, NULL, line, file), result->error);
}

void setResultOperationNone(Result *ru, Inter *inter, LinkValue *belong) {
    setResult(ru, inter);
    ru->type = R_opt;
}

void setResultOperation(Result *ru, LinkValue *value) {
    freeResult(ru);
    setResultOperationBase(ru, value);
}

void setResultOperationBase(Result *ru, LinkValue *value) {
    setResultCore(ru);
    ru->value = value;
    if (value != NULL)
        gc_addTmpLink(&ru->value->gc_status);
    ru->type = R_opt;
}

void freeResult(Result *ru){
    memFree(ru->label);
    ru->label = NULL;
    if (ru->error != NULL)
        freeError(ru);
    ru->error = NULL;

    if (ru->value != NULL) {
        gc_freeTmpLink(&ru->value->gc_status);
        ru->value = NULL;
    }

    setResultCore(ru);
}

Error *makeError(wchar_t *type, wchar_t *message, fline line, char *file) {
    Error *tmp = memCalloc(1, sizeof(Error));
    tmp->line = line;
    tmp->type = memWidecpy(type);
    tmp->messgae = memWidecpy(message);
    tmp->file = memStrcpy(file);
    tmp->next = NULL;
    return tmp;
}

Error *connectError(Error *new, Error *base){
    new->next = base;
    return new;
}

void freeError(Result *base){
    Error *error = base->error;
    for (Error *next = NULL; error != NULL; error = next){
        next = error->next;
        memFree(error->messgae);
        memFree(error->type);
        memFree(error->file);
        memFree(error);
    }
    base->error = NULL;
}

void printError(Result *result, Inter *inter, bool free) {
    for (Error *base = result->error; base != NULL; base = base->next){
        if (base->next != NULL)
            fprintf(inter->data.inter_stderr, "Error Backtracking:  On Line: %lld In file: %s Error ID: %p\n", base->line, base->file, base);
        else
            fprintf(inter->data.inter_stderr, "%ls\n%ls\nOn Line: %lld\nIn File: %s\nError ID: %p\n", base->type, base->messgae, base->line, base->file, base);
    }
    if (free)
        freeError(result);
    fflush(inter->data.inter_stderr);
}

inline bool isType(Value *value, enum ValueType type){
    return value->type == type;
}

Inherit *makeInherit(LinkValue *value){
    Inherit *tmp;
    tmp = memCalloc(1, sizeof(Inherit));
    tmp->value = value;
    tmp->next = NULL;
    return tmp;
}

Inherit *copyInheritCore(Inherit *value){
    Inherit *tmp;
    if (value == NULL)
        return NULL;
    tmp = makeInherit(value->value);
    return tmp;
}

Inherit *copyInherit(Inherit *value){
    Inherit *base = NULL;
    Inherit **tmp = &base;
    for (PASS; value != NULL; value = value->next, tmp = &(*tmp)->next)
        *tmp = copyInheritCore(value);
    return base;
}

Inherit *freeInherit(Inherit *value){
    FREE_BASE(value, error_);
    Inherit *next = value->next;
    memFree(value);
    return next;
    error_: return NULL;
}

Inherit *connectInherit(Inherit *base, Inherit *back){
    Inherit **tmp = &base;
    for (PASS; *tmp != NULL; tmp = &(*tmp)->next)
        PASS;
    *tmp = back;
    return base;
}

Inherit *connectSafeInherit(Inherit *base, Inherit *back){
    Inherit **last_node = &base;
    if (back == NULL)
        goto return_;
    for (PASS; *last_node != NULL;)
        if ((*last_node)->value->value == back->value->value)
            *last_node = freeInherit(*last_node);
        else
            last_node = &(*last_node)->next;
    *last_node = back;
    return_: return base;
}

Inherit *getInheritFromValueCore(LinkValue *num_father) {
    Inherit *object_father;
    Argument *father_arg = makeValueArgument(num_father);
    gc_addTmpLink(&num_father->gc_status);
    object_father = setFather(father_arg);
    freeArgument(father_arg, true);
    gc_freeTmpLink(&num_father->gc_status);
    return object_father;
}

Package *makePackage(Value *value, char *md5, char *name, Package *base) {
    Package *tmp = memCalloc(1, sizeof(Package));
    Package *tmp_base = base;
    tmp->name = memStrcpy(name);
    tmp->md5 = memStrcpy(md5);
    tmp->package = value;
    gc_addStatementLink(&value->gc_status);
    tmp->next = NULL;
    if (base == NULL)
        return tmp;

    for (PASS; tmp_base->next != NULL; tmp_base = tmp_base->next)
            PASS;
    tmp_base->next = tmp;
    return base;
}

void freePackage(Package *base) {
    for (Package *next; base != NULL; base = next) {
        next = base->next;
        gc_freeStatementLink(&base->package->gc_status);
        memFree(base->name);
        memFree(base->md5);
        memFree(base);
    }
}

Value *checkPackage(Package *base, char *md5, char *name) {
    for (PASS; base != NULL; base = base->next) {
        if (eqString(name, base->name) && eqString(md5, base->md5))
            return base->package;
    }
    return NULL;
}

bool needDel(Value *object_value, Inter *inter) {
    LinkValue *_del_ = checkStrVar(inter->data.object_del, false, CFUNC_CORE(object_value->object.var));
    enum FunctionPtType type;
    if (_del_ == NULL)
        return false;
    type = _del_->value->data.function.function_data.pt_type;
    if ((type == object_free_ || type == object_static_) && object_value->type == V_class)
        return false;
    if (_del_->belong == NULL || _del_->belong->value == object_value || checkAttribution(object_value, _del_->belong->value))
        return true;
    return false;
}

bool callDel(Value *object_value, Result *result, Inter *inter, VarList *var_list) {
    LinkValue *_del_ = findStrVarOnly(inter->data.object_del, false, CFUNC_CORE(object_value->object.var));
    setResultCore(result);

    if (_del_ != NULL){
        gc_addTmpLink(&_del_->gc_status);
        if (_del_->belong == NULL || _del_->belong->value != object_value && checkAttribution(object_value, _del_->belong->value))
            _del_->belong = makeLinkValue(object_value, inter->base_belong, inter);
        callBackCore(_del_, NULL, 0, "sys", 0,
                     CFUNC_NT(var_list, result, inter->base_belong));
        gc_freeTmpLink(&_del_->gc_status);
        return true;
    } else
        return false;
}

/**
 * 检查 father 是否为 self 的父亲
 * @param self
 * @param father
 * @return
 */
bool checkAttribution(Value *self, Value *father){
    for (Inherit *self_father = self->object.inherit; self_father != NULL; self_father = self_father->next)
        if (self_father->value->value == father)
            return true;
    return false;
}

void printValue(Value *value, FILE *debug, bool print_father, bool print_in) {
    switch (value->type){
        case V_num:
            fprintf(debug, "(%lld)", value->data.num.num);
            break;
        case V_str:
            fprintf(debug, "'%ls'", value->data.str.str);
            break;
        case V_func:
            if (print_father)
                fprintf(debug, "func");
            else
                fprintf(debug, "(func on %p)", value);
            break;
        case V_list:
            if (print_in){
                fprintf(debug, "[");
                for (int i = 0; i < value->data.list.size; i++) {
                    if (i > 0)
                        fprintf(debug, ", ", NULL);
                    printValue(value->data.list.list[i]->value, debug, false, false);
                }
                fprintf(debug, " ]", NULL);
            } else
                fprintf(debug, "[list]", NULL);
            break;
        case V_dict:
            if (print_in){
                Var *tmp = NULL;
                bool print_comma = false;
                fprintf(debug, "{");
                for (int i = 0; i < MAX_SIZE; i++) {
                    for (tmp = value->data.dict.dict->hashtable[i]; tmp != NULL; tmp = tmp->next) {
                        if (print_comma)
                            fprintf(debug, ", ", NULL);
                        else
                            print_comma = true;
                        printValue(tmp->name_->value, debug, false, false);
                        fprintf(debug, " ['%ls'] : ", tmp->name);
                        printValue(tmp->value->value, debug, false, false);
                    }
                }
                fprintf(debug, " }", NULL);
            } else
                fprintf(debug, "[dict]", NULL);
            break;
        case V_none:
            fprintf(debug, "(null)", NULL);
            break;
        case V_class:
            if (print_father)
                fprintf(debug, "class");
            else
                fprintf(debug, "(class on %p)", value);
            break;
        case V_obj:
            if (print_father)
                fprintf(debug, "object");
            else
                fprintf(debug, "(object on %p)", value);
            break;
        case V_bool:
            if (value->data.bool_.bool_)
                fprintf(debug, "true");
            else
                fprintf(debug, "false");
            break;
        case V_ell:
            fprintf(debug, "...");
            break;
        default:
            fprintf(debug, "unknown");
            break;
    }
    if (print_father){
        fprintf(debug, "(");
        printf("<%p>", value);
        for (Inherit *fv = value->object.inherit; fv != NULL; fv = fv->next) {
            printf(" -> ");
            printValue(fv->value->value, debug, false, false);
        }
        fprintf(debug, ")");
    }

}

void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug){
    if (value == NULL)
        return;
    fprintf(debug, "%s", first);
    if (value->belong != NULL) {
        printLinkValue(value->belong, "", "", debug);
        fprintf(debug, " . ", NULL);
    }
    if (value->value != NULL)
        printValue(value->value, debug, true, true);
    fprintf(debug, "%s", last);
}