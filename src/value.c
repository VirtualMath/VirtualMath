#include "__virtualmath.h"

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, FatherValue *father) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    setGC(&tmp->gc_status);
    tmp->type = object_;
    tmp->gc_next = NULL;

    if (inter->data.object != NULL && father == NULL)
        father = makeFatherValue(makeLinkValue(inter->data.object, NULL, inter));
    tmp->object.var = makeObjectVarList(father, inter, object);
    tmp->object.out_var = out_var;
    tmp->object.father = father;

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

Value *makeNoneValue(Inter *inter) {
    Value *tmp;
    if (inter->data.none == NULL) {
        tmp = makeObject(inter, NULL, NULL, NULL);
        tmp->type = none;
    }
    else
        tmp = inter->data.none;
    return tmp;
}

Value *makeBoolValue(bool bool_num, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = bool_;
    tmp->data.bool_.bool_ = bool_num;
    return tmp;
}

Value *makePassValue(Inter *inter){
    Value *tmp;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = pass_;
    return tmp;
}

Value *makeNumberValue(NUMBER_TYPE num, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = number;
    tmp->data.num.num = num;
    return tmp;
}

Value *makeStringValue(char *str, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = string;
    tmp->data.str.str = memStrcpy(str);
    return tmp;
}


void setFunctionData(Value *value) {
    value->data.function.function_data.pt_type = object_static_;
}

Value *makeVMFunctionValue(Statement *st, Parameter *pt, VarList *var_list, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, var_list, NULL);
    tmp->type = function;
    tmp->data.function.type = vm_function;
    tmp->data.function.function = copyStatement(st);
    tmp->data.function.pt = copyParameter(pt);
    tmp->data.function.of = NULL;
    setFunctionData(tmp);
    return tmp;
}

Value *makeCFunctionValue(OfficialFunction of, VarList *var_list, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, copyVarList(var_list, false, inter), NULL);
    tmp->type = function;
    tmp->data.function.type = c_function;
    tmp->data.function.function = NULL;
    tmp->data.function.pt = NULL;
    tmp->data.function.of = of;
    setFunctionData(tmp);
    return tmp;
}

Value *makeClassValue(VarList *var_list, Inter *inter, FatherValue *father) {
    Value *tmp;
    tmp = makeObject(inter, NULL, var_list, father);
    tmp->type = class;
    return tmp;
}

Value *makeListValue(Argument **arg_ad, Inter *inter, enum ListType type) {
    Value *tmp;
    Argument *at = *arg_ad;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = list;
    tmp->data.list.type = type;
    tmp->data.list.list = NULL;
    tmp->data.list.size = 0;
    for (PASS; at != NULL && at->type == value_arg; at = at->next) {
        tmp->data.list.size++;
        tmp->data.list.list = memRealloc(tmp->data.list.list, tmp->data.list.size * sizeof(LinkValue *));
        tmp->data.list.list[tmp->data.list.size - 1] = at->data.value;
    }
    *arg_ad = at;
    return tmp;
}

Value *makeDictValue(Argument **arg_ad, bool new_hash, INTER_FUNCTIONSIG_NOT_ST) {
    Value *tmp;
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->data.dict.size = 0;
    tmp->type = dict;
    if (new_hash) {
        VarList *hash = pushVarList(var_list, inter);
        gc_addTmpLink(&tmp->gc_status);
        tmp->data.dict.dict = hash->hashtable;
        freeResult(result);
        argumentToVar(arg_ad, &tmp->data.dict.size, CALL_INTER_FUNCTIONSIG_NOT_ST(hash, result, father));
        popVarList(hash);
        gc_freeTmpLink(&tmp->gc_status);
    }
    else
        tmp->data.dict.dict = NULL;
    return tmp;
}

void freeValue(Value **value) {
    Value *free_value = *value;
    freeBase(free_value, return_);
    for (VarList *tmp = free_value->object.var; tmp != NULL; tmp = freeVarList(tmp))
            PASS;
    for (VarList *tmp = free_value->object.out_var; tmp != NULL; tmp = freeVarList(tmp))
            PASS;
    for (struct FatherValue *tmp = free_value->object.father; tmp != NULL; tmp = freeFatherValue(tmp))
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

LinkValue *makeLinkValue(Value *value, LinkValue *linkValue, Inter *inter){
    LinkValue *tmp;
    LinkValue *list_tmp = inter->link_base;
    tmp = memCalloc(1, sizeof(Value));
    setGC(&tmp->gc_status);
    tmp->father = linkValue;
    tmp->value = value;
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
    freeBase(free_value, return_);

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
    tmp = makeLinkValue(value->value, value->father, inter);
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

void setResult(Result *ru, Inter *inter, LinkValue *father) {
    freeResult(ru);
    setResultBase(ru, inter, father);
}

void setResultBase(Result *ru, Inter *inter, LinkValue *father) {
    setResultCore(ru);
    ru->value = makeLinkValue(makeNoneValue(inter), father, inter);
    gc_addTmpLink(&ru->value->gc_status);
}

void setResultErrorSt(Result *ru, Inter *inter, char *error_type, char *error_message, Statement *st, LinkValue *father, bool new) {
    setResultError(ru, inter, error_type, error_message, st->line, st->code_file, father, new);
}

void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, long int line, char *file, LinkValue *father, bool new) {
    if (!new && ru->type != error_return)
        return;
    if (new) {
        setResult(ru, inter, father);
        ru->type = error_return;
    }
    else{
        error_type = NULL;
        error_message = NULL;
    }
    ru->error = connectError(makeError(error_type, error_message, line, file), ru->error);
}

void setResultOperationNone(Result *ru, Inter *inter, LinkValue *father) {
    setResult(ru, inter, father);
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

void printValue(Value *value, FILE *debug, bool print_father) {
    switch (value->type){
        case number:
            fprintf(debug, "%"NUMBER_FORMAT"", value->data.num.num);
            break;
        case string:
            fprintf(debug, "'%s'", value->data.str.str);
            break;
        case function:
            fprintf(debug, "function");
            break;
        case list:
            fprintf(debug, "list on size : %d  [ ", (int)value->data.list.size);
            for (int i=0;i < value->data.list.size;i++){
                if (i > 0)
                    fprintf(debug, ", ", NULL);

                printLinkValue(value->data.list.list[i], "", "", debug);
            }
            fprintf(debug, " ]", NULL);
            break;
        case dict: {
            Var *tmp = NULL;
            bool print_comma = false;
            fprintf(debug, "dict size : %d  { ", (int) value->data.dict.size);
            for (int i = 0; i < MAX_SIZE; i++) {
                for (tmp = value->data.dict.dict->hashtable[i]; tmp != NULL; tmp = tmp->next) {
                    if (print_comma)
                        fprintf(debug, ", ", NULL);
                    else
                        print_comma = true;
                    printLinkValue(tmp->name_, "", "", debug);
                    fprintf(debug, " ['%s'] : ", tmp->name);
                    printLinkValue(tmp->value, "", "", debug);
                }
            }
            fprintf(debug, " }", NULL);
            break;
        }
        case none:
            fprintf(debug, "<None>", NULL);
            break;
        case class:
            fprintf(debug, "class");
            break;
        case object_:
            fprintf(debug, "object");
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
            fprintf(debug, "unknow");
            break;
    }
    fprintf(debug, "(");
    printf("<%p>", value);
    if (print_father)
        for (FatherValue *fv = value->object.father; fv != NULL; fv = fv->next) {
            printf(" -> ");
            printValue(fv->value->value, debug, false);
        }
    fprintf(debug, ")");

}

void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug){
    if (value == NULL)
        return;
    fprintf(debug, "%s", first);
    if (value->father != NULL) {
        printLinkValue(value->father, "", "", debug);
        fprintf(debug, " . ", NULL);
    }
    if (value->value != NULL)
        printValue(value->value, debug, true);
    fprintf(debug, "%s", last);
}

Error *makeError(char *type, char *message, long int line, char *file) {
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
            fprintf(inter->data.error, "Error Backtracking:  On Line: %ld In file: %s Error ID: %p\n", base->line, base->file, base);
        else
            fprintf(inter->data.error, "%s\n%s\nOn Line: %ld\nIn File: %s\nError ID: %p\n", base->type, base->messgae, base->line, base->file, base);
    }
    if (free)
        freeError(result);
}

inline bool isType(Value *value, enum ValueType type){
    return value->type == type;
}

FatherValue *makeFatherValue(LinkValue *value){
    FatherValue *tmp;
    tmp = memCalloc(1, sizeof(FatherValue));
    tmp->value = value;
    tmp->next = NULL;
    return tmp;
}

FatherValue *copyFatherValueCore(FatherValue *value){
    FatherValue *tmp;
    if (value == NULL)
        return NULL;
    tmp = makeFatherValue(value->value);
    return tmp;
}

FatherValue *copyFatherValue(FatherValue *value){
    FatherValue *base = NULL;
    FatherValue **tmp = &base;
    for (PASS; value != NULL; value = value->next, tmp = &(*tmp)->next)
        *tmp = copyFatherValueCore(value);
    return base;
}

FatherValue *freeFatherValue(FatherValue *value){
    freeBase(value, error_);
    FatherValue *next = value->next;
    memFree(value);
    return next;
    error_: return NULL;
}

FatherValue *connectFatherValue(FatherValue *base, FatherValue *back){
    FatherValue **tmp = &base;
    for (PASS; *tmp != NULL; tmp = &(*tmp)->next)
        PASS;
    *tmp = back;
    return base;
}

FatherValue *connectSafeFatherValue(FatherValue *base, FatherValue *back){
    FatherValue **last_node = &base;
    if (back == NULL)
        goto reutrn_;
    for (PASS; *last_node != NULL;)
        if ((*last_node)->value->value == back->value->value)
            *last_node = freeFatherValue(*last_node);
        else
            last_node = &(*last_node)->next;
    *last_node = back;
    reutrn_: return base;
}

/**
 * 检查 father 是否为 self 的父亲
 * @param self
 * @param father
 * @return
 */
bool checkAttribution(Value *self, Value *father){
    for (FatherValue *self_father = self->object.father; self_father != NULL; self_father = self_father->next)
        if (self_father->value->value == father)
            return true;
    return false;
}
