#include "__virtualmath.h"


Value *makeObject(Inter *inter, VarList *object, VarList *out_var, FatherValue *father) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    setGC(&tmp->gc_status);
    tmp->type = object_;
    tmp->gc_next = NULL;
    tmp->object.var = object == NULL ? makeObjectVarList(father, inter) : object;
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
    tmp = makeObject(inter, NULL, NULL, NULL);
    tmp->type = none;
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

Value *makeFunctionValue(Statement *st, Parameter *pt, VarList *var_list, Inter *inter) {
    Value *tmp;
    tmp = makeObject(inter, NULL, var_list, NULL);
    tmp->type = function;
    tmp->data.function.function = copyStatement(st);
    tmp->data.function.pt = copyParameter(pt);
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
    for (VarList *tmp = free_value->object.var; tmp != NULL; tmp = freeVarList(tmp, true))
            PASS;
    for (VarList *tmp = free_value->object.out_var; tmp != NULL; tmp = freeVarList(tmp, true))
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
    LinkValue *tmp = makeLinkValue(value->value, value->father, inter);
    tmp->aut = value->aut;
    return tmp;
}

void setResultCore(Result *ru) {
    ru->type = not_return;
    ru->times = 0;
    ru->error = NULL;
    ru->value = NULL;
}

void setResult(Result *ru, Inter *inter, LinkValue *father) {
    freeResult(ru);
    setResultBase(ru, inter, father);
}

void setResultBase(Result *ru, Inter *inter, LinkValue *father) {
    setResultCore(ru);
    ru->value = makeLinkValue(inter->base, father, inter);
    gc_addTmpLink(&ru->value->gc_status);
}

void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, Statement *st, LinkValue *father,
                    bool new) {
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
    ru->error = connectError(makeError(error_type, error_message, st->line, st->code_file), ru->error);
}

void setResultOperationNone(Result *ru, Inter *inter, LinkValue *father) {
    setResult(ru, inter, father);
    ru->type = operation_return;
}

void setResultOperation(Result *ru, LinkValue *value, Inter *inter) {
    freeResult(ru);
    setResultOperationBase(ru, value, inter);
}

void setResultOperationBase(Result *ru, LinkValue *value, Inter *inter) {
    setResultCore(ru);
    ru->value = value;
    if (value != NULL)
        gc_addTmpLink(&ru->value->gc_status);
    ru->type = operation_return;
}

void freeResult(Result *ru){
    freeResultSave(ru);
    if (ru->value != NULL) {
        gc_freeTmpLink(&ru->value->gc_status);
        ru->value = NULL;
    }
}

void freeResultSave(Result *ru){
    if (ru->error != NULL)
        freeError(ru);
}

void printValue(Value *value, FILE *debug){
    switch (value->type){
        case number:
            writeLog(debug, INFO, "%"NUMBER_FORMAT"", value->data.num.num);
            break;
        case string:
            writeLog(debug, INFO, "'%s'", value->data.str.str);
            break;
        case function:
            writeLog(debug, INFO, "function on <%p>", value);
            break;
        case list:
            writeLog(debug, INFO, "list on <%p> size : %d  [ ", value, (int)value->data.list.size);
            for (int i=0;i < value->data.list.size;i++){
                if (i > 0)
                    writeLog(debug, INFO, ", ", NULL);

                printLinkValue(value->data.list.list[i], "", "", debug);
            }
            writeLog(debug, INFO, " ]", NULL);
            break;
        case dict: {
            Var *tmp = NULL;
            bool print_comma = false;
            writeLog(debug, INFO, "dict on <%p> size : %d  { ", value, (int) value->data.dict.size);
            for (int i = 0; i < MAX_SIZE; i++) {
                for (tmp = value->data.dict.dict->hashtable[i]; tmp != NULL; tmp = tmp->next) {
                    if (print_comma)
                        writeLog(debug, INFO, ", ", NULL);
                    else
                        print_comma = true;
                    printLinkValue(tmp->name_, "", "", debug);
                    writeLog(debug, INFO, " ['%s'] : ", tmp->name);
                    printLinkValue(tmp->value, "", "", debug);
                }
            }
            writeLog(debug, INFO, " }", NULL);
            break;
        }
        case none:
            writeLog(debug, INFO, "<None>", NULL);
            break;
        case class:
            writeLog(debug, INFO, "class on <%p>", value);
            break;
        case object_:
            writeLog(debug, INFO, "object on <%p>", value);
            break;
        default:
            writeLog(debug, INFO, "default on <%p>", value);
            break;
    }
}

void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug){
    if (value == NULL)
        return;
    writeLog(debug, INFO, "%s", first);
    if (value->father != NULL) {
        printLinkValue(value->father, "", "", debug);
        writeLog(debug, INFO, " . ", NULL);
    }
    if (value->value != NULL)
        printValue(value->value, debug);
    writeLog(debug, INFO, "%s", last);
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
        if (base->next != NULL){
            writeLog(inter->data.error, ERROR, "Error Backtracking:  On Line: %ld In file: %s Error ID: %p\n", base->line, base->file, base);
        }
        else{
            writeLog(inter->data.error, ERROR, "%s\n%s\nOn Line: %ld\nIn File: %s\nError ID: %p\n", base->type, base->messgae, base->line, base->file, base);
        }
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

FatherValue *copyFatherValue(FatherValue *value){
    FatherValue *tmp;
    if (value == NULL)
        return NULL;
    tmp = makeFatherValue(value->value);
    return tmp;
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
    for (tmp = &base; *tmp != NULL; tmp = &(*tmp)->next)
        PASS;
    *tmp = back;
    return base;
}

FatherValue *connectSafeFatherValue(FatherValue *base, FatherValue *back){
    FatherValue **last_node = &base;
    if (back == NULL)
        goto reutrn_;
    for (PASS; *last_node != NULL; )
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
