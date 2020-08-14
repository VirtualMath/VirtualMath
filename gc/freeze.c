#include "__virtualmath.h"

void gc_freezeHashTable(HashTable *ht, bool is_lock){
    if (ht == NULL)
        return;

    if (is_lock)
        gcAddTmp(&ht->gc_status);
    else
        gcFreeTmpLink(&ht->gc_status);

    setIterAlready(&ht->gc_status);
}

void iterFreezeVarList(VarList *freeze, VarList *base, bool is_lock){
    for (PASS; freeze != NULL; freeze = freeze->next){
        bool need_freeze = true;
        for (VarList *tmp = base; tmp != NULL;tmp = tmp->next)
            if (tmp->hashtable == freeze->hashtable){
                need_freeze = false;
                break;
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
