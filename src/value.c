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
    tmp->last = list_tmp->next;

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
    tmp->data.function.pt = pt;
    tmp->data.function.var = copyVarList(var_list, false, inter);
    return tmp;
}

Value *makeListValue(Argument **ad_ad, Inter *inter, int type) {
    Value *tmp;
    Argument *at = *ad_ad;
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
    *ad_ad = at;
    return tmp;
}

void freeValue(Value *value, Inter *inter){
    freeBase(value, return_);
    if (value->last == NULL){
        inter->base = value->next;
    }
    else{
        value->last->next = value->next;
    }
    if (value->next != NULL){
        value->next->last = value->last;
    }
    switch (value->type) {
        case string:
            memFree(value->data.str.str);
            break;
        case function: {
            VarList *tmp = value->data.function.var;
            freeParameter(value->data.function.pt, true);
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
    LinkValue *tmp, *list_tmp = inter->link_base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->father = linkValue;
    tmp->value = value;
    if (list_tmp == NULL){
        inter->link_base = tmp;
        tmp->last = NULL;
        goto return_;
    }

    while (list_tmp->next !=  NULL){
        list_tmp = list_tmp->next;
    }
    list_tmp->next = tmp;
    tmp->last = list_tmp->next;

    return_:
    return tmp;
}

void freeLinkValue(LinkValue *value, Inter *inter){
    freeBase(value, return_);
    if (value->last == NULL){
        inter->link_base = value->next;
    }
    else{
        value->last->next = value->next;
    }
    if (value->next != NULL){
        value->next->last = value->last;
    }
    memFree(value);
    return_:
    return;
}

void setResult(Result *ru, bool link, Inter *inter) {
    ru->type = not_return;
    if (link){
        // inter->base即None值
        ru->value = makeLinkValue(inter->base, NULL, inter);
    }
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
            writeLog(debug, INFO, "<%ld>", value->data.num.num);
            break;
        case string:
            writeLog(debug, INFO, "<'%s'>", value->data.str.str);
            break;
        case function:
            writeLog(debug, INFO, "function on <%lx>", (unsigned long )value);
            break;
        case list:
            writeLog(debug, INFO, "list on %lx, size = %d, [", (unsigned long )value, (int)value->data.list.size);
            for (int i=0;i < value->data.list.size;i++){
                printLinkValue(value->data.list.list[i], "", "", debug);
            }
            writeLog(debug, INFO, "]", NULL);
            break;
        case none:
            writeLog(debug, INFO, "<None>", NULL);
            break;
        default:
            writeLog(debug, INFO, "default on <%lx>", (unsigned long )value);
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

