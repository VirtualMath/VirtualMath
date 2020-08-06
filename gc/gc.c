#include "__virtualmath.h"

void gc_iterLinkValue(LinkValue *value){
    if (value == NULL)
        return;
    gcAddLink(&value->gc_status);
    if (!setIterAlready(&value->gc_status)){
        gc_iterLinkValue(value->father);
        gc_iterValue(value->value);
    }
}

void gc_iterValue(Value *value){
    if (value == NULL)
        return;
    gcAddLink(&value->gc_status);
    if (setIterAlready(&value->gc_status))
        return;
    switch (value->type) {
        case function:
            gc_var_list(value->data.function.var);
            break;
        case list:
            for (int i=0;i < value->data.list.size;i++)
                gc_iterLinkValue(value->data.list.list[i]);
            break;
        case dict:
            gc_iterHashTable(value->data.dict.dict);
            break;
        default:
            break;
    }
}

void gc_var_list(VarList *vl){
    while (vl != NULL){
        gc_iterHashTable(vl->hashtable);
        vl = vl->next;
    }
}

void gc_iterHashTable(HashTable *ht){
    if (ht == NULL)
        return;
    gcAddLink(&ht->gc_status);
    if (setIterAlready(&ht->gc_status))
        return;
    for (int i=0;i < MAX_SIZE;i++){
        gc_iterVar(ht->hashtable[i]);
    }
}

void gc_iterVar(Var *var){
    while (var != NULL){
        gc_iterLinkValue(var->name_);
        gc_iterLinkValue(var->value);
        var = var->next;
    }
}

void gc_resetBase(Inter *inter){
    Value *value_base = inter->base;
    while (value_base != NULL){
        resetGC(&value_base->gc_status);
        value_base = value_base->next;
    }

    LinkValue *link_base = inter->link_base;
    while (link_base != NULL){
        resetGC(&link_base->gc_status);
        link_base = link_base->next;
    }

    HashTable *hash_base = inter->hash_base;
    while (hash_base != NULL){
        resetGC(&hash_base->gc_status);
        hash_base = hash_base->next;
    }
}

void gc_checkBase(Inter *inter){
    Value *value_base = inter->base;
    while (value_base != NULL){
        if (!needFree(&value_base->gc_status) && !value_base->gc_status.continue_)
            gc_iterValue(value_base);
        value_base = value_base->next;
    }

    LinkValue *link_base = inter->link_base;
    while (link_base != NULL){
        if (!needFree(&link_base->gc_status) && !link_base->gc_status.continue_)
            gc_iterLinkValue(link_base);
        link_base = link_base->next;
    }

    HashTable *hash_base = inter->hash_base;
    while (hash_base != NULL){
        if (!needFree(&hash_base->gc_status) && !hash_base->gc_status.continue_)
            gc_iterHashTable(hash_base);
        hash_base = hash_base->next;
    }
}

void gc_freeBase(Inter *inter){
#if START_GC
    Value *value_base = inter->base;
    while (value_base != NULL){
        if (needFree(&value_base->gc_status))
            value_base = freeValue(value_base, inter);
        else
            value_base = value_base->next;
    }

    LinkValue *link_base = inter->link_base;
    while (link_base != NULL){
        if (needFree(&link_base->gc_status))
            link_base = freeLinkValue(link_base, inter);
        else
            link_base = link_base->next;
    }

    HashTable *hash_base = inter->hash_base;
    while (hash_base != NULL){
        if (needFree(&hash_base->gc_status))
            hash_base = freeHashTable(hash_base, inter);
        else
            hash_base = hash_base->next;
    }
#endif
}

void runGC(Inter *inter, int var_list, int link_value, int value, ...){
#if START_GC
    gc_resetBase(inter);
    va_list arg;
    va_start(arg, value);
    for (int i =0;i < var_list;i ++){
        VarList *tmp = va_arg(arg, VarList *);
        gc_var_list(tmp);
    }
    for (int i =0;i < link_value;i ++){
        LinkValue *tmp = va_arg(arg, LinkValue *);
        gc_iterLinkValue(tmp);
    }
    for (int i =0;i < value;i ++){
        Value *tmp = va_arg(arg, Value *);
        gc_iterValue(tmp);
    }
    va_end(arg);
    gc_checkBase(inter);
    gc_freeBase(inter);
#endif
}
