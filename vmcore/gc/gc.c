#include "__virtualmath.h"

#if START_GC
static void gc_iterVar(Var *var);
static void gc_iterLinkValue(LinkValue *value);
static void gc_iterValue(Value *value);
static void gc_iterHashTable(HashTable *ht);

#define resetGC(gcs) ((gcs)->continue_ = false, (gcs)->link = 0)

// 若(gcs)->continue_为true, 则直接返回; 若为false则自增(+1后为false), 并返回自增前的值
#define gc_shouldIter(gcs) (!((gcs)->continue_))
#define gc_setIterAlready(gcs) ((gcs)->continue_ = true)

#define gc_needFree(gcs) ((gcs)->statement_link == 0 && (gcs)->tmp_link == 0 && (gcs)->link == 0)
#define gc_needFreeValue(value) (gc_needFree(&(value)->gc_status) && (value)->gc_status.c_value == need_free)

static void gc_iterLinkValue(LinkValue *value){
    gc_setIterAlready(&value->gc_status);
    gc_addLink(&value->gc_status);
    if (value->belong != NULL && gc_shouldIter(&value->belong->gc_status))
        gc_iterLinkValue(value->belong);
    if (gc_shouldIter(&value->value->gc_status))
        gc_iterValue(value->value);
}

static void gc_iterValue(Value *value){
    gc_setIterAlready(&value->gc_status);
    gc_addLink(&value->gc_status);

    for (VarList *vl = value->object.var; vl != NULL; vl = vl->next) {
        if (gc_shouldIter(&vl->hashtable->gc_status))
            gc_iterHashTable(vl->hashtable);
    }

    for (VarList *vl = value->object.out_var; vl != NULL; vl = vl->next) {
        if (gc_shouldIter(&vl->hashtable->gc_status))
            gc_iterHashTable(vl->hashtable);
    }

    for (Inherit *ih = value->object.inherit; ih != NULL; ih = ih->next) {
        if (gc_shouldIter(&ih->value->gc_status))
            gc_iterLinkValue(ih->value);
    }

    // 不重置value的c_value属性, __del__始终只执行一次
    switch (value->type) {
        case V_list:
            for (int i=0;i < value->data.list.size;i++) {
                if (gc_shouldIter(&value->data.list.list[i]->gc_status))
                    gc_iterLinkValue(value->data.list.list[i]);
            }
            break;
        case V_dict:
            if (gc_shouldIter(&value->data.dict.dict->gc_status))
                gc_iterHashTable(value->data.dict.dict);
            break;
        case V_func:
            if (value->data.function.function_data.cls != NULL && gc_shouldIter(&value->data.function.function_data.cls->gc_status))
                gc_iterLinkValue(value->data.function.function_data.cls);
            break;
        default:
            break;
    }
}

static void gc_iterHashTable(HashTable *ht){
    gc_setIterAlready(&ht->gc_status);
    gc_addLink(&ht->gc_status);
    for (int i=0;i < MAX_SIZE;i++) {
        if (ht->hashtable[i] != NULL)
            gc_iterVar(ht->hashtable[i]);
    }
}

static void gc_iterVar(Var *var){
    gc_setIterAlready(&var->gc_status);
    for (PASS; var != NULL; var = var->next){
        gc_addLink(&var->gc_status);
        if (gc_shouldIter(&var->name_->gc_status))
            gc_iterLinkValue(var->name_);
        if (gc_shouldIter(&var->value->gc_status))
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
        if (!gc_needFree(&value_base->gc_status) && gc_shouldIter(&value_base->gc_status))
            gc_iterValue(value_base);

    for (LinkValue *link_base = inter->link_base; link_base != NULL; link_base = link_base->gc_next)
        if (!gc_needFree(&link_base->gc_status) && gc_shouldIter(&link_base->gc_status))
            gc_iterLinkValue(link_base);

    for (HashTable *hash_base = inter->hash_base; hash_base != NULL; hash_base = hash_base->gc_next)
        if (!gc_needFree(&hash_base->gc_status) && gc_shouldIter(&hash_base->gc_status))
            gc_iterHashTable(hash_base);

    for (Var *var_base = inter->base_var; var_base != NULL; var_base = var_base->gc_next)
        if (!gc_needFree(&var_base->gc_status) && gc_shouldIter(&var_base->gc_status))
            gc_iterVar(var_base);
}

static void gc_checkDel(Inter *inter){
    for (Value *value = inter->base; value != NULL; value = value->gc_next) {
        if (value->gc_status.c_value == not_free) {
            if (needDel(value, inter)) {
                gc_iterValue(value);  // 不需要检查是否位shouldIter, 因为他必然不是
                value->gc_status.c_value = run_del;
            } else
                value->gc_status.c_value = need_free;
        }
    }
}

void gc_runDelAll(Inter *inter){
    Result result;
    setResultCore(&result);
    for (Value *value = inter->base; value != NULL; value = value->gc_next) {
        gc_addTmpLink(&value->gc_status);
        if (value->gc_status.c_value != need_free && needDel(value, inter)) {  // 检查value->gc_status.c_value != need_free; 是避免二次运行__del__
            value->gc_status.c_value = need_free;
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
            value->gc_status.c_value = need_free;
            gc_addTmpLink(&value->gc_status);
            callDel(value, &result, inter, var_list);
            if (!RUN_TYPE(result.type))
                printError(&result, inter, true);
            gc_freeTmpLink(&value->gc_status);
            freeResult(&result);
        }
    }
}

static void gc_freeBase(Inter *inter){
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

    for (Value **value_base = &inter->base; *value_base != NULL;)
        if (gc_needFreeValue(*value_base))
            freeValue(value_base);
        else
            value_base = &(*value_base)->gc_next;
}

void gc_run(Inter *inter, VarList *run_var){
    gc_resetBase(inter);
    inter->data.run_gc = 0;
    gc_checkBase(inter);
    gc_checkDel(inter);
    gc_freeBase(inter);
    gc_runDel(inter, run_var);
}
#endif
