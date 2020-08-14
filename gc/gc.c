#include "__virtualmath.h"

void gc_iterLinkValue(LinkValue *value){
    if (value == NULL)
        return;
    gc_addLink(&value->gc_status);
    if (!gc_IterAlready(&value->gc_status)){
        gc_iterLinkValue(value->father);
        gc_iterValue(value->value);
    }
}

void gc_fatherValue(FatherValue *value){
    for (PASS; value != NULL; value = value->next)
        gc_iterLinkValue(value->value);
}

void gc_iterValue(Value *value){
    if (value == NULL)
        return;
    gc_addLink(&value->gc_status);
    if (gc_IterAlready(&value->gc_status))
        return;
    gc_varList(value->object.var);
    gc_varList(value->object.out_var);
    gc_fatherValue(value->object.father);
    switch (value->type) {
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

void gc_varList(VarList *vl){
    for (PASS; vl != NULL; vl = vl->next)
        gc_iterHashTable(vl->hashtable);
}

void gc_iterHashTable(HashTable *ht){
    if (ht == NULL)
        return;
    gc_addLink(&ht->gc_status);
    if (gc_IterAlready(&ht->gc_status))
        return;
    for (int i=0;i < MAX_SIZE;i++)
        gc_iterVar(ht->hashtable[i]);
}

void gc_iterVar(Var *var){
    if (var == NULL)
        return;
    gc_addLink(&var->gc_status);
    if (gc_IterAlready(&var->gc_status))
        return;
    for (PASS; var != NULL; var = var->next){
        gc_iterLinkValue(var->name_);
        gc_iterLinkValue(var->value);
    }
}

void gc_resetBase(Inter *inter){
    for (Value *value_base = inter->base; value_base != NULL; value_base = value_base->gc_next)
        resetGC(&value_base->gc_status);

    for (LinkValue *link_base = inter->link_base; link_base != NULL; link_base = link_base->gc_next)
        resetGC(&link_base->gc_status);

    for (HashTable *hash_base = inter->hash_base; hash_base != NULL; hash_base = hash_base->gc_next)
        resetGC(&hash_base->gc_status);

    for (Var *var_base = inter->base_var; var_base != NULL; var_base = var_base->gc_next)
        resetGC(&var_base->gc_status);
}

void gc_checkBase(Inter *inter){
    for (Value *value_base = inter->base; value_base != NULL; value_base = value_base->gc_next)
        if (!gc_needFree(&value_base->gc_status) && !value_base->gc_status.continue_)
            gc_iterValue(value_base);

    for (LinkValue *link_base = inter->link_base; link_base != NULL; link_base = link_base->gc_next)
        if (!gc_needFree(&link_base->gc_status) && !link_base->gc_status.continue_)
            gc_iterLinkValue(link_base);

    for (HashTable *hash_base = inter->hash_base; hash_base != NULL; hash_base = hash_base->gc_next)
        if (!gc_needFree(&hash_base->gc_status) && !hash_base->gc_status.continue_)
            gc_iterHashTable(hash_base);
}

void gc_freeBase(Inter *inter){
#if START_GC
    for (Value **value_base = &inter->base; *value_base != NULL;)
        if (gc_needFree(&(*value_base)->gc_status))
            freeValue(value_base);
        else
            value_base = &(*value_base)->gc_next;

    for (LinkValue **link_base = &inter->link_base; *link_base != NULL;)
        if (gc_needFree(&(*link_base)->gc_status))
            freeLinkValue(link_base);
        else
            link_base = &(*link_base)->gc_next;

    for (HashTable **hash_base = &inter->hash_base; *hash_base != NULL;)
        if (gc_needFree(&(*hash_base)->gc_status))
            freeHashTable(hash_base);
        else
            hash_base = &(*hash_base)->gc_next;

    for (Var **var_base = &inter->base_var; *var_base != NULL;)
        if (gc_needFree(&(*var_base)->gc_status))
            freeVar(var_base);
        else
            var_base = &(*var_base)->gc_next;
#endif
}

void runGC(Inter *inter, int var_list, int link_value, int value, ...){
#if START_GC
    gc_resetBase(inter);
    va_list arg;
    va_start(arg, value);
    for (int i =0;i < var_list;i ++){
        VarList *tmp = va_arg(arg, VarList *);
        gc_varList(tmp);
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
