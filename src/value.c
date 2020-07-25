#include "__virtualmath.h"

Value *makeValue(Inter *inter) {
    Value *tmp, *list_tmp = inter->base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->type = number;
    tmp->data.num.num = 0;
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

Value *makeNumberValue(number_type num, Inter *inter) {
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

void freeValue(Value *value, Inter *inter){
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
        default:
            break;
    }
    memFree(value);
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
}

void setResult(Result *ru, bool link, Inter *inter) {
    ru->type = statement_end;
    if (link){
        ru->value = makeLinkValue(NULL, NULL, inter);
    }
}
