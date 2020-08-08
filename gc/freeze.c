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

    switch (value->type) {
        case function:
            gc_freezeVarList(value->data.function.var, is_lock);
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
    while (vl != NULL) {
        gc_freezeHashTable(vl->hashtable, is_lock);
        vl = vl->next;
    }
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

    for (int i=0;i < MAX_SIZE;i++){
        gc_freezeVar(ht->hashtable[i], is_lock);
    }
}

void gc_freezeVar(Var *var, bool is_lock){
    while (var != NULL){
        gc_freezeLinkValue(var->name_, is_lock);
        gc_freezeLinkValue(var->value, is_lock);
        var = var->next;
    }
}

void iterFreezeVarList(VarList *freeze, VarList *base, bool is_lock){
    while (freeze != NULL){
        VarList *tmp = base;
        bool need_freeze = true;
        while (tmp != NULL){
            if (tmp->hashtable == freeze->hashtable){
                need_freeze = false;
                break;
            }
            tmp = tmp->next;
        }
        if (need_freeze)
            gc_freezeHashTable(freeze->hashtable, is_lock);
        freeze = freeze->next;
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
