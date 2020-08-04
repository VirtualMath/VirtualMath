#include "__virtualmath.h"

Value *makeValue(Inter *inter) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->type = none;
    tmp->next = NULL;
    if (list_tmp == NULL){
        inter->base = tmp;
        tmp->last = NULL;
        goto return_;
    }

    while (list_tmp->next !=  NULL){
        list_tmp = list_tmp->next;
    }
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
    tmp->data.str.str = memStrcpy(str, 0, false, false);
    return tmp;
}

Value *makeFunctionValue(Statement *st, Parameter *pt, VarList *var_list, Inter *inter) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->type = function;
    tmp->data.function.function = st;
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
    while (at != NULL && at->type == value_arg){
        tmp->data.list.size++;
        tmp->data.list.list = memRealloc(tmp->data.list.list, tmp->data.list.size * sizeof(LinkValue *));
        tmp->data.list.list[tmp->data.list.size - 1] = at->data.value;
        at = at->next;
    }
    *arg_ad = at;
    return tmp;
}

Value *makeDictValue(Argument **arg_ad, bool new_hash, Inter *inter) {
    Value *tmp;
    tmp = makeValue(inter);
    tmp->data.dict.size = 0;
    tmp->type = dict;
    if (new_hash) {
        VarList *hash = makeVarList(inter);
        tmp->data.dict.dict = hash->hashtable;
        argumentToVar(arg_ad, inter, hash, &tmp->data.dict.size);
        freeVarList(hash, true);
    }
    else
        tmp->data.dict.dict = NULL;
    return tmp;
}

void freeValue(Value *value, Inter *inter){
    freeBase(value, return_);
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
            freeParameter(value->data.function.pt, false);
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
    return;
}

LinkValue *makeLinkValue(Value *value, LinkValue *linkValue, Inter *inter){
    LinkValue *tmp;
    LinkValue *list_tmp = inter->link_base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->father = linkValue;
    tmp->value = value;
    if (list_tmp == NULL){
        inter->link_base = tmp;
        tmp->last = NULL;
        goto return_;
    }

    while (list_tmp->next !=  NULL)
        list_tmp = list_tmp->next;

    list_tmp->next = tmp;
    tmp->last = list_tmp;

    return_:
    return tmp;
}

void freeLinkValue(LinkValue *value, Inter *inter){
    freeBase(value, return_);
    if (value->last == NULL)
        inter->link_base = value->next;
    else
        value->last->next = value->next;

    if (value->next != NULL)
        value->next->last = value->last;

    memFree(value);
    return_:
    return;
}

void setResult(Result *ru, bool link, Inter *inter) {
    ru->type = not_return;
    if (link)
        ru->value = makeLinkValue(inter->base, NULL, inter);
}

void setResultError(Result *ru, Inter *inter) {
    ru->type = error_return;
    ru->value = makeLinkValue(inter->base, NULL, inter);
}

void setResultOperation(Result *ru, Inter *inter) {
    ru->type = operation_return;
    ru->value = makeLinkValue(inter->base, NULL, inter);
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
            bool print_comma = false;
            writeLog(debug, INFO, "dict on <%p> size : %d  { ", value, (int) value->data.dict.size);
            for (int i = 0; i < MAX_SIZE; i++) {
                Var *tmp = value->data.dict.dict->hashtable[i];
                while (tmp != NULL) {
                    if (print_comma)
                        writeLog(debug, INFO, ", ", NULL);
                    else
                        print_comma = true;
                    printLinkValue(tmp->name_, "", "", debug);
                    writeLog(debug, INFO, " ['%s'] : ", tmp->name);
                    printLinkValue(tmp->value, "", "", debug);
                    tmp = tmp->next;
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
    writeLog(debug, INFO, "%s", first);
    if (value->father != NULL) {
        printLinkValue(value->father, "", "", debug);
        writeLog(debug, INFO, " . ", NULL);
    }
    printValue(value->value, debug);
    writeLog(debug, INFO, "%s", last);
}

