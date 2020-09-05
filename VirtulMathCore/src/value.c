#include "__run.h"

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, Inherit *inherit) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    setGC(&tmp->gc_status);
    tmp->type = object_;
    tmp->gc_next = NULL;
    if (inter->data.object != NULL && inherit == NULL)
        inherit = makeInherit(makeLinkValue(inter->data.object, NULL, inter));
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

Value *makeNoneValue(fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp;
    setResultCore(result);
    callBackCore(makeLinkValue(inter->data.object, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->type = none;
    return tmp;
}

Value *useNoneValue(Inter *inter, Result *result) {
    Value *tmp = inter->data.none;
    if (result != NULL) {
        setResultCore(result);
        result->type = operation_return;
        result->value = makeLinkValue(tmp, inter->base_father, inter);
        gc_addTmpLink(&result->value->gc_status);
    }
    return tmp;
}

Value *makeBoolValue(bool bool_num, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.bool_, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.bool_.bool_ = bool_num;
    return tmp;
}

Value *makePassValue(fline line, char *file, INTER_FUNCTIONSIG_NOT_ST){  // TODO-szh 让切片支持该语法
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.pass_, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    return tmp;
}

Value *makeNumberValue(vnum num, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.num, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.num.num = num;
    return tmp;
}

Value *makeStringValue(char *str, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.str, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    memFree(tmp->data.str.str);
    tmp->data.str.str = memStrcpy(str);
    return tmp;
}

Value *makeVMFunctionValue(Statement *st, Parameter *pt, INTER_FUNCTIONSIG_NOT_ST) {  // TODO-szh 设置无var_list的函数 (允许使用装饰器装饰，该功能未测试)
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.function, inter->base_father, inter), NULL, st->line, st->code_file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.function.function = copyStatement(st);
    tmp->data.function.pt = copyParameter(pt);
    for (VarList *vl = tmp->object.out_var, *vl_next; vl != NULL; vl = vl_next) {
        vl_next = vl->next;
        freeVarList(vl);
    }
    tmp->object.out_var = copyVarList(var_list, false, inter);
    result->value->belong = belong;
    return tmp;
}

Value *makeCFunctionValue(OfficialFunction of, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    callBackCore(makeLinkValue(inter->data.function, inter->base_father, inter), NULL, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    tmp->data.function.type = c_function;
    tmp->data.function.of = of;
    tmp->data.function.function_data.pt_type = inter->data.default_pt_type;
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
    function_new(CALL_OFFICAL_FUNCTION(arg, func->value->object.var, &result, func));
    return_ = result.value;
    result.value = NULL;
    freeResult(&result);

    init_arg = makeValueArgument(return_);
    function_init(CALL_OFFICAL_FUNCTION(init_arg, func->value->object.var, &result, func));
    freeResult(&result);
    freeArgument(init_arg, true);
    freeArgument(arg, true);

    return_->value->data.function.type = c_function;
    return_->value->data.function.of = of;
    return_->value->data.function.function_data.pt_type = inter->data.default_pt_type;
    for (VarList *vl = return_->value->object.out_var, *vl_next; vl != NULL; vl = vl_next) {
        vl_next = vl->next;
        freeVarList(vl);
    }
    return_->value->object.out_var = copyVarList(var_list, false, inter);
    return_->belong = belong;
    gc_freeTmpLink(&return_->gc_status);
    return return_;
}

Value *makeClassValue(VarList *var_list, Inter *inter, Inherit *father) {
    Value *tmp;
    tmp = makeObject(inter, NULL, var_list, father);
    tmp->type = class;
    return tmp;
}

Value *makeListValue(Argument *arg, fline line, char *file, enum ListType type, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    if (type == value_list)
        callBackCore(makeLinkValue(inter->data.list, inter->base_father, inter), arg, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    else
        callBackCore(makeLinkValue(inter->data.tuple, inter->base_father, inter), arg, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
    if (!CHECK_RESULT(result))
        return NULL;
    tmp = result->value->value;
    return tmp;
}

Value *makeDictValue(Argument *arg, bool new_hash, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp = NULL;
    LinkValue *dict = makeLinkValue(inter->data.dict, inter->base_father, inter);
    callBackCore(dict, arg, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result ,belong));
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
        case string:
            memFree(free_value->data.str.str);
            break;
        case function: {
            freeParameter(free_value->data.function.pt, true);
            freeStatement(free_value->data.function.function);
            break;
        }
        case list:
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
    ru->type = not_return;
    ru->times = 0;
    ru->error = NULL;
    ru->value = NULL;
    ru->label = NULL;
    ru->node = NULL;
}

void setResult(Result *ru, Inter *inter, LinkValue *belong) {
    freeResult(ru);
    setResultBase(ru, inter);
}

void setResultBase(Result *ru, Inter *inter) {
    setResultCore(ru);
    useNoneValue(inter, ru);
}

void setResultErrorSt(BaseErrorType type, char *error_message, bool new, Statement *st, INTER_FUNCTIONSIG_NOT_ST) {
    setResultError(type, error_message, st->line, st->code_file, new, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
}

Value *findBaseError(BaseErrorType type, Inter *inter){
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

char *getErrorInfo(LinkValue *exc, int type, Inter *inter){
    char *str_name = type == 1 ? inter->data.object_name : inter->data.object_message;
    LinkValue *_info_ = findAttributes(str_name, false, exc, inter);
    if (_info_ != NULL && _info_->value->type == string)
        return memStrcpy(_info_->value->data.str.str);
    else
        return type == 1 ? memStrcpy("Error Type: Unknown") : memStrcpy("Error Message: Unknown");
}

void callException(LinkValue *exc, char *message, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *_new_ = findAttributes(inter->data.object_new, false, exc, inter);
    char *type = NULL;
    char *error_message = NULL;
    setResultCore(result);
    gc_addTmpLink(&exc->gc_status);

    if (_new_ != NULL){
        Argument *arg = NULL;
        makeStringValue(message, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            goto return_;
        arg =  makeValueArgument(result->value);
        freeResult(result);

        gc_addTmpLink(&_new_->gc_status);
        callBackCore(_new_, arg, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&_new_->gc_status);
        freeArgument(arg, true);
        type = getErrorInfo(result->value, 1, inter);
        error_message = getErrorInfo(result->value, 2, inter);
    }
    else {
        result->value = exc;
        gc_addTmpLink(&result->value->gc_status);
    }

    result->type = error_return;
    result->error = connectError(makeError(type, error_message, line, file), result->error);
    memFree(type);
    memFree(error_message);
    return_: gc_freeTmpLink(&exc->gc_status);
}

void setResultError(BaseErrorType type, char *error_message, fline line, char *file, bool new, INTER_FUNCTIONSIG_NOT_ST) {
    if (!new && result->type != error_return)
        return;
    if (new) {
        Value *exc = findBaseError(type, inter);
        if (exc == NULL)
            exc = inter->data.base_exc;
        freeResult(result);
        callException(makeLinkValue(exc, belong, inter), error_message, line, file, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    }
    else
        result->error = connectError(makeError(NULL, NULL, line, file), result->error);
}

void setResultOperationNone(Result *ru, Inter *inter, LinkValue *belong) {
    setResult(ru, inter, belong);
    ru->type = operation_return;
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
    ru->type = operation_return;
}

void freeResult(Result *ru){
    memFree(ru->label);
    ru->label = NULL;
    freeResultSafe(ru);
    if (ru->value != NULL) {
        gc_freeTmpLink(&ru->value->gc_status);
        ru->value = NULL;
    }
}

void freeResultSafe(Result *ru){
    if (ru->error != NULL)
        freeError(ru);
    ru->error = NULL;
}

Error *makeError(char *type, char *message, fline line, char *file) {
    Error *tmp = memCalloc(1, sizeof(Error));
    tmp->line = line;
    tmp->type = memStrcpy(type);
    tmp->messgae = memStrcpy(message);
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
            fprintf(inter->data.inter_stderr, "%s\n%s\nOn Line: %lld\nIn File: %s\nError ID: %p\n", base->type, base->messgae, base->line, base->file, base);
    }
    if (free)
        freeError(result);
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

Inherit *getInheritFromValue(Value *value, Inter *inter){
    LinkValue *num_father = makeLinkValue(value, inter->base_father, inter);
    return getInheritFromValueCore(num_father);
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

bool needDel(Value *object_value, Inter *inter) {
    LinkValue *_del_ = checkStrVar(inter->data.object_del, false, CALL_INTER_FUNCTIONSIG_CORE(object_value->object.var));
    enum FunctionPtType type;
    if (_del_ == NULL)
        return false;
    type = _del_->value->data.function.function_data.pt_type;
    if ((type == object_free_ || type == object_static_) && object_value->type == class)
        return false;
    if (_del_->belong == NULL || _del_->belong->value == object_value || checkAttribution(object_value, _del_->belong->value))
        return true;
    return false;
}

bool callDel(Value *object_value, Result *result, Inter *inter, VarList *var_list) {
    LinkValue *_del_ = findStrVar(inter->data.object_del, false, CALL_INTER_FUNCTIONSIG_CORE(object_value->object.var));
    setResultCore(result);

    if (_del_ != NULL){
        gc_addTmpLink(&_del_->gc_status);
        if (_del_->belong == NULL || _del_->belong->value != object_value && checkAttribution(object_value, _del_->belong->value))
            _del_->belong = makeLinkValue(object_value, inter->base_father, inter);
        callBackCore(_del_, NULL, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, inter->base_father));
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
        case number:
            fprintf(debug, "%lld", value->data.num.num);
            break;
        case string:
            fprintf(debug, "%s", value->data.str.str);
            break;
        case function:
            if (print_father)
                fprintf(debug, "function");
            else
                fprintf(debug, "(function on %p)", value);
            break;
        case list:
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
        case dict:
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
                        fprintf(debug, " ['%s'] : ", tmp->name);
                        printValue(tmp->value->value, debug, false, false);
                    }
                }
                fprintf(debug, " }", NULL);
            } else
                fprintf(debug, "[dict]", NULL);
            break;
        case none:
            fprintf(debug, "(null)", NULL);
            break;
        case class:
            if (print_father)
                fprintf(debug, "class");
            else
                fprintf(debug, "(class on %p)", value);
            break;
        case object_:
            if (print_father)
                fprintf(debug, "object");
            else
                fprintf(debug, "(object on %p)", value);
            break;
        case bool_:
            if (value->data.bool_.bool_)
                fprintf(debug, "true");
            else
                fprintf(debug, "false");
            break;
        case pass_:
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