#include "__virtualmath.h"

#if START_GC
static void gc_iterVar(Var *var);
static void gc_iterLinkValue(LinkValue *value);
static void gc_iterValue(Value *value);
static void gc_varList(VarList *vl);
static void gc_iterHashTable(HashTable *ht);

#define resetGC(gcs) ((gcs)->continue_ = false, (gcs)->link = 0)

// 若(gcs)->continue_为true, 则直接返回; 若为false则自增(+1后为false), 并返回自增前的值
//#define gc_iterAlready(gcs) ((gcs)->continue_) || (((gcs)->continue_)++)  // TODO-szh 不能达到预期的效果

static bool gc_iterAlready(GCStatus *gcs){
    bool return_ = gcs->continue_;
    gcs->continue_ = true;
    return return_;
}

#define gc_needFree(gcs) ((gcs)->statement_link == 0 && (gcs)->tmp_link == 0 && (gcs)->link == 0)
#define gc_resetValue(value) ((value)->gc_status.c_value = not_free)
#define gc_needFreeValue(value) (gc_needFree(&(value)->gc_status) && (value)->gc_status.c_value == need_free)

static void gc_iterLinkValue(LinkValue *value){
    if (value == NULL)
        return;
    gc_addLink(&value->gc_status);
    if (!gc_iterAlready(&value->gc_status)){
        gc_iterLinkValue(value->belong);
        gc_iterValue(value->value);
    }
}

static void gc_iterValue(Value *value){
    if (value == NULL)
        return;
    gc_addLink(&value->gc_status);
    if (gc_iterAlready(&value->gc_status))
        return;
    gc_varList(value->object.var);
    gc_varList(value->object.out_var);
    {
        Inherit *ih = value->object.inherit;
        for (PASS; ih != NULL; ih = ih->next)
            gc_iterLinkValue(ih->value);
    }
    gc_resetValue(value);
    switch (value->type) {
        case V_list:
            for (int i=0;i < value->data.list.size;i++)
                gc_iterLinkValue(value->data.list.list[i]);
            break;
        case V_dict:
            gc_iterHashTable(value->data.dict.dict);
            break;
        case V_func:
            gc_iterLinkValue(value->data.function.function_data.cls);
            break;
        default:
            break;
    }
}

static void gc_varList(VarList *vl){
    for (PASS; vl != NULL; vl = vl->next)
        gc_iterHashTable(vl->hashtable);
}

static void gc_iterHashTable(HashTable *ht){
    if (ht == NULL)
        return;
    gc_addLink(&ht->gc_status);
    if (gc_iterAlready(&ht->gc_status))
        return;
    for (int i=0;i < MAX_SIZE;i++)
        gc_iterVar(ht->hashtable[i]);
}

static void gc_iterVar(Var *var){
    if (var == NULL)
        return;
    if (gc_iterAlready(&var->gc_status))
        return;
    for (PASS; var != NULL; var = var->next){
        gc_addLink(&var->gc_status);
        gc_iterLinkValue(var->name_);
        gc_iterLinkValue(var->value);
    }
}

#define gc_resetBase(inter) do {  \
    for (Value *value_base = (inter)->base; value_base != NULL; value_base = value_base->gc_next)  \
        {resetGC(&value_base->gc_status);}  \
    for (LinkValue *link_base = (inter)->link_base; link_base != NULL; link_base = link_base->gc_next)   \
        {resetGC(&link_base->gc_status);}   \
    for (HashTable *hash_base = (inter)->hash_base; hash_base != NULL; hash_base = hash_base->gc_next)  \
        {resetGC(&hash_base->gc_status);}  \
    for (Var *var_base = (inter)->base_var; var_base != NULL; var_base = var_base->gc_next)  \
        {resetGC(&var_base->gc_status);}  \
} while(0)

static void gc_checkBase(Inter *inter){
    for (Value *value_base = inter->base; value_base != NULL; value_base = value_base->gc_next)
        if (!gc_needFree(&value_base->gc_status) && !value_base->gc_status.continue_)
            gc_iterValue(value_base);

    for (LinkValue *link_base = inter->link_base; link_base != NULL; link_base = link_base->gc_next)
        if (!gc_needFree(&link_base->gc_status) && !link_base->gc_status.continue_)
            gc_iterLinkValue(link_base);

    for (HashTable *hash_base = inter->hash_base; hash_base != NULL; hash_base = hash_base->gc_next)
        if (!gc_needFree(&hash_base->gc_status) && !hash_base->gc_status.continue_)
            gc_iterHashTable(hash_base);

    for (Var *var_base = inter->base_var; var_base != NULL; var_base = var_base->gc_next)
        if (!gc_needFree(&var_base->gc_status) && !var_base->gc_status.continue_)
            gc_iterVar(var_base);
}

static void gc_checkDel(Inter *inter){
    for (Value *value = inter->base; value != NULL; value = value->gc_next)
        if (!gc_needFree(&value->gc_status))
            gc_resetValue(value);
        else if (value->gc_status.c_value == not_free){
            if (needDel(value, inter)){
                gc_iterValue(value);
                value->gc_status.c_value = run_del;
            }
            else
                value->gc_status.c_value = need_free;
        }
}

void gc_runDelAll(Inter *inter){
    Result result;
    setResultCore(&result);
    for (Value *value = inter->base; value != NULL; value = value->gc_next) {
        gc_addTmpLink(&value->gc_status);
        if (needDel(value, inter)) {
            callDel(value, &result, inter, inter->var_list);
            if (!RUN_TYPE(result.type))
                printError(&result, inter, true);
            freeResult(&result);
        }
        gc_freeTmpLink(&value->gc_status);
    }
}

static void gc_runDel(Inter *inter, VarList *var_list){
    Result result;
    setResultCore(&result);
    for (Value *value = inter->base; value != NULL; value = value->gc_next) {
        if (value->gc_status.c_value == run_del) {
            gc_addTmpLink(&value->gc_status);
            callDel(value, &result, inter, var_list);
            if (!RUN_TYPE(result.type))
                printError(&result, inter, true);
            gc_freeTmpLink(&value->gc_status);
            value->gc_status.c_value = need_free;
            freeResult(&result);
        }
    }
}

static void gc_freeBase(Inter *inter){
    for (Value **value_base = &inter->base; *value_base != NULL;)
        if (gc_needFreeValue(*value_base))
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
}

void gc_run(Inter *inter, VarList *run_var, int var_list, int link_value, int value, ...){
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
    gc_checkDel(inter);
    gc_freeBase(inter);
    gc_runDel(inter, run_var);
}

static void gc_freezeHashTable(HashTable *ht, bool is_lock){
    if (ht == NULL)
        return;

    if (is_lock) {
        gc_addTmpLink(&ht->gc_status);
    } else {
        gc_freeTmpLink(&ht->gc_status);
    }

    gc_iterAlready(&ht->gc_status);
}

/**
 * 冻结不可达的VarList的hashTable
 * @param inter
 * @param freeze
 * @param base
 * @param is_lock
 */
void gc_freeze(Inter *inter, VarList *freeze, VarList *base, bool is_lock){
    gc_resetBase(inter);
    for (PASS; freeze != NULL; freeze = freeze->next){
        for (VarList *src = base; src != NULL; src = src->next) {
            if (src->hashtable != freeze->hashtable) {
                gc_freezeHashTable(freeze->hashtable, is_lock);
                break;
            }
        }
    }
}
#endif
