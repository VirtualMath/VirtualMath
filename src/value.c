#include "__virtualmath.h"


Value *makeValue(Inter *inter) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->type = none;
    setGC(&tmp->gc_status);
    tmp->next = NULL;
    if (list_tmp == NULL){
        inter->base = tmp;
        tmp->last = NULL;
        goto return_;
    }

    for (PASS; list_tmp->next !=  NULL; list_tmp = list_tmp->next)
        PASS;

    list_tmp->next = tmp;
    tmp->last = list_tmp;

    return_:
    return tmp;
}

Value *makeNumberValue(NUMBER_TYPE num, Inter *inter) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->type = number;
    tmp->data.num.num = num;
    return tmp;
}

Value *makeStringValue(char *str, Inter *inter) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->type = string;
    tmp->data.str.str = memStrcpy(str);
    return tmp;
}

Value *makeFunctionValue(Statement *st, Parameter *pt, VarList *var_list, Inter *inter) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->type = function;
    tmp->data.function.function = copyStatement(st);
    tmp->data.function.pt = copyParameter(pt);
    tmp->data.function.var = copyVarList(var_list, false, inter);
    return tmp;
}

Value *makeListValue(Argument **arg_ad, Inter *inter, enum ListType type) {
    Value *tmp;
    Argument *at = *arg_ad;
    tmp = makeValue(inter);
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

Value *makeDictValue(Argument **arg_ad, bool new_hash, Result *result, Inter *inter, VarList *var_list) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->data.dict.size = 0;
    tmp->type = dict;
    if (new_hash) {
        VarList *hash = pushVarList(var_list, inter);
        gcAddTmp(&tmp->gc_status);
        tmp->data.dict.dict = hash->hashtable;
        freeResult(result);
        argumentToVar(arg_ad, &tmp->data.dict.size, CALL_INTER_FUNCTIONSIG_NOT_ST(hash, result));
        popVarList(hash);
        gcFreeTmpLink(&tmp->gc_status);
    }
    else
        tmp->data.dict.dict = NULL;
    return tmp;
}

Value *freeValue(Value *value, Inter *inter){
    Value *return_value = NULL;
    freeBase(value, return_);
    return_value = value->next;
    if (value->last == NULL)
        inter->base = value->next;
    else
        value->last->next = value->next;

    if (value->next != NULL)
        value->next->last = value->last;

    switch (value->type) {
        case string:
            memFree(value->data.str.str);
            break;
        case function: {
            VarList *tmp = value->data.function.var;
            freeParameter(value->data.function.pt, true);
            freeStatement(value->data.function.function);
            while (tmp != NULL)
                tmp = freeVarList(tmp, true);
            break;
        }
        case list:
            memFree(value->data.list.list);
            break;
        default:
            break;
    }
    memFree(value);
    return_:
    return return_value;
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
        tmp->last = NULL;
        goto return_;
    }

    for (PASS; list_tmp->next !=  NULL; list_tmp = list_tmp->next)
        PASS;

    list_tmp->next = tmp;
    tmp->last = list_tmp;

    return_:
    return tmp;
}

LinkValue * freeLinkValue(LinkValue *value, Inter *inter){
    LinkValue *return_value = NULL;
    freeBase(value, return_);
    return_value = value->next;
    if (value->last == NULL)
        inter->link_base = value->next;
    else
        value->last->next = value->next;

    if (value->next != NULL)
        value->next->last = value->last;

    memFree(value);
    return_:
    return return_value;
}

void setResultCore(Result *ru) {
    ru->type = not_return;
    ru->times = 0;
    ru->error = NULL;
    ru->value = NULL;
}

void setResult(Result *ru, Inter *inter) {
    freeResult(ru);
    setResultBase(ru, inter);
}

void setResultBase(Result *ru, Inter *inter) {
    setResultCore(ru);
    ru->value = makeLinkValue(inter->base, NULL, inter);
    gcAddTmp(&ru->value->gc_status);
}

void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, Statement *st, bool new) {
    if (!new && ru->type != error_return)
        return;
    if (new) {
        setResult(ru, inter);
        ru->type = error_return;
    }
    else{
        error_type = NULL;
        error_message = NULL;
    }
    ru->error = connectError(makeError(error_type, error_message, st->line, st->code_file), ru->error);
}

void setResultOperationNone(Result *ru, Inter *inter) {
    setResult(ru, inter);
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
        gcAddTmp(&ru->value->gc_status);
    ru->type = operation_return;
}

void freeResult(Result *ru){
    freeResultSave(ru);
    if (ru->value != NULL) {
        gcFreeTmpLink(&ru->value->gc_status);
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
