#include "__virtualmath.h"

Var *makeVar(char *name, LinkValue *value){
    Var *tmp;
    tmp = memCalloc(1, sizeof(Var));
    tmp->name = memStrcpy(name, 0, false, false);
    tmp->value = value;
    tmp->next = NULL;
    return tmp;
}

Var *freeVar(Var *var, bool self){
    freeBase(var, return_);
    memFree(var->name);
    if (self){
        Var *next_var = var->next;
        memFree(var);
        return next_var;
    }
    return_:
    return var;
}

HashTable *makeHashTable(Inter *inter) {
    HashTable *tmp, *list_tmp = inter->hash_base;
    tmp = memCalloc(1, sizeof(Value));
    tmp->hashtable = (Var **)calloc(MAX_SIZE, sizeof(Var *));
    tmp->next = NULL;
    if (list_tmp == NULL){
        inter->hash_base = tmp;
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

void freeHashTable(HashTable *ht, Inter *inter){
    freeBase(ht, return_);
    if (ht->last == NULL){
        inter->hash_base = ht->next;
    }
    else{
        ht->last->next = ht->next;
    }
    if (ht->next != NULL){
        ht->next->last = ht->last;
    }

    for (int i=0; i < MAX_SIZE; i++){
        Var *tmp = ht->hashtable[i];
        while (tmp != NULL){
            tmp = freeVar(tmp, true);
        }
    }
    memFree(ht->hashtable);
    memFree(ht);
    return_:
    return;
}

VarList *makeVarList(Inter *inter) {
    VarList *tmp = calloc(1, sizeof(VarList));
    tmp->next = NULL;
    tmp->hashtable = makeHashTable(inter);
    return tmp;
}

VarList *freeVarList(VarList *vl, bool self){
    freeBase(vl, return_);
    if (self){
        VarList *next_var = vl->next;
        memFree(vl);
        return next_var;
    }
    return_:
    return vl;
}

/**
 * hashTable使用time33算法
 * @param key
 * @return
 */
HASH_INDEX time33(char *key){ // hash function
    HASH_INDEX hash = 5381;
    while(*key){
        hash += (hash << (HASH_INDEX)5) + (*key++);
    }
    return (hash & (HASH_INDEX)0x7FFFFFFF) % MAX_SIZE;
}


void addVar(char *name, LinkValue *value, VarList *var_list){
    HASH_INDEX index = time33(name);
    Var *base = var_list->hashtable->hashtable[index];
    if (base == NULL){
        var_list->hashtable->hashtable[index] = makeVar(name, value);
        goto return_;
    }
    while (base->next != NULL){
        base = base->next;
    }
    base->next = makeVar(name, value);
    return_:
    return;
}

LinkValue *findVar(char *name, VarList *var_list){
    LinkValue *tmp = NULL;
    HASH_INDEX index = time33(name);
    Var *base = var_list->hashtable->hashtable[index];
    if (base == NULL){
        goto return_;
    }
    while (base != NULL){
        if (eqString(base->name, name)){
            tmp = base->value;
            goto return_;
        }
        base = base->next;
    }
    return_:
    return tmp;
}

LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times){
    LinkValue *tmp = NULL;
    for (NUMBER_TYPE i=0; i < times && var_list->next != NULL; i++){
        var_list = var_list->next;
    }
    while (var_list != NULL){
        tmp = findVar(name, var_list);
        if (tmp != NULL){
            goto return_;
        }
        var_list = var_list->next;
    }
    return_:
    return tmp;
}

void addFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, LinkValue *value){
    for (NUMBER_TYPE i=0; i < times && var_list->next != NULL; i++){
        var_list = var_list->next;
    }
    addVar(name, value, var_list);
}
