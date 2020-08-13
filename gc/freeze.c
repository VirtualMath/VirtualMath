#include "__virtualmath.h"

void gc_freezeValue(Value *value, bool is_lock);
void gc_freezeHashTable(HashTable *ht, bool is_lock);
void gc_freezeVarList(VarList *vl, bool is_lock);
void gc_freezeVar(Var *var, bool is_lock);

void gc_freezeLinkValue(LinkValue *value, bool is_lock){
    if (value == NULL)
        return;

    if (is_lock)
        gcAddTmp(&value->gc_status);
    else
        gcFreeTmpLink(&value->gc_status);

    if (!setIterAlready(&value->gc_status)){
        gc_freezeLinkValue(value->father, is_lock);
        gc_freezeValue(value->value, is_lock);
    }
}

void gc_freezeValue(Value *value, bool is_lock){
    if (value == NULL)
        return;

    if (is_lock)
        gcAddTmp(&value->gc_status);
    else
        gcFreeTmpLink(&value->gc_status);

    if (setIterAlready(&value->gc_status))
        return;
    gc_freezeVarList(value->object.var, is_lock);
    switch (value->type) {
        case function:
            gc_freezeVarList(value->data.function.out_var, is_lock);
            break;
        case class:
            gc_freezeVarList(value->data.class.out_var, is_lock);
            break;
        case list:
            for (int i=0;i < value->data.list.size;i++)
                gc_freezeLinkValue(value->data.list.list[i], is_lock);
            break;
        case dict:
            gc_freezeHashTable(value->data.dict.dict, is_lock);
            break;
        default:
            break;
    }
}

void gc_freezeVarList(VarList *vl, bool is_lock){
    for (PASS; vl != NULL; vl = vl->next)
        gc_freezeHashTable(vl->hashtable, is_lock);
}

void gc_freezeHashTable(HashTable *ht, bool is_lock){
    if (ht == NULL)
        return;

    if (is_lock)
        gcAddTmp(&ht->gc_status);
    else
        gcFreeTmpLink(&ht->gc_status);

    if (setIterAlready(&ht->gc_status))
        return;

    for (int i=0;i < MAX_SIZE;i++)
        gc_freezeVar(ht->hashtable[i], is_lock);
}

void gc_freezeVar(Var *var, bool is_lock){
    for (PASS; var != NULL; var = var->next){
        gc_freezeLinkValue(var->name_, is_lock);
        gc_freezeLinkValue(var->value, is_lock);
    }
}

void iterFreezeVarList(VarList *freeze, VarList *base, bool is_lock){
    for (PASS; freeze != NULL; freeze = freeze->next){
        bool need_freeze = true;
        for (VarList *tmp = base; tmp != NULL;tmp = tmp->next){
            if (tmp->hashtable == freeze->hashtable){
                need_freeze = false;
                break;
            }
        }
        if (need_freeze)
            gc_freezeHashTable(freeze->hashtable, is_lock);
    }
}

/**
 * 冻结不可达的VarList的hashTable
 * @param inter
 * @param freeze
 * @param base
 * @param is_lock
 */
void runFREEZE(Inter *inter, VarList *freeze, VarList *base, bool is_lock){
#if START_GC
    gc_resetBase(inter);
    iterFreezeVarList(freeze, base, is_lock);
#endif
}
