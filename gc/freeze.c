#include "__virtualmath.h"

void gc_freezeHashTable(HashTable *ht, bool is_lock){
    if (ht == NULL)
        return;

    if (is_lock)
        gc_addTmpLink(&ht->gc_status);
    else
        gc_freeTmpLink(&ht->gc_status);

    gc_IterAlready(&ht->gc_status);
}

void gc_iterFreezeVarList(VarList *freeze, VarList *base, bool is_lock){
    for (PASS; freeze != NULL; freeze = freeze->next){
        if (!comparVarList(freeze, base))
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
void gc_freeze(Inter *inter, VarList *freeze, VarList *base, bool is_lock){
#if START_GC
    gc_resetBase(inter);
    gc_iterFreezeVarList(freeze, base, is_lock);
#endif
}
