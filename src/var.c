#include "__virtualmath.h"

Var *makeVar(char *name, LinkValue *value, LinkValue *name_) {
    Var *tmp;
    tmp = memCalloc(1, sizeof(Var));
    tmp->name = memStrcpy(name, 0, false, false);
    tmp->value = value;
    tmp->name_ = name_;
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

HashTable *makeHashTable(Inter *inter, bool supervision) {
    HashTable *list_tmp = inter->hash_base;
    HashTable *tmp;
    tmp = memCalloc(1, sizeof(Value));
    tmp->hashtable = (Var **)calloc(MAX_SIZE, sizeof(Var *));
    tmp->next = NULL;
    tmp->last = NULL;
    if (!supervision)
        goto return_;

    if (list_tmp == NULL){
        inter->hash_base = tmp;
        tmp->last = NULL;
        goto return_;
    }

    while (list_tmp->next !=  NULL)
        list_tmp = list_tmp->next;
    list_tmp->next = tmp;
    tmp->last = list_tmp;

    return_:
    return tmp;
}

void freeHashTable(HashTable *ht, Inter *inter, bool supervision) {
    freeBase(ht, return_);
    if (!supervision)
        goto not_supervision;
    if (ht->last == NULL)
        inter->hash_base = ht->next;
    else
        ht->last->next = ht->next;

    if (ht->next != NULL) {
        HashTable *tmp = ht->last;
        ht->next->last = tmp;
    }

    not_supervision:
    for (int i=0; i < MAX_SIZE; i++){
        Var *tmp = ht->hashtable[i];
        while (tmp != NULL)
            tmp = freeVar(tmp, true);
    }
    memFree(ht->hashtable);
    memFree(ht);
    return_:
    return;
}

VarList *makeVarList(Inter *inter) {
    VarList *tmp = calloc(1, sizeof(VarList));
    tmp->next = NULL;
    tmp->hashtable = makeHashTable(inter, true);
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


void addVar(char *name, LinkValue *value, LinkValue *name_, VarList *var_list) {
    HASH_INDEX index = time33(name);
    Var *base = var_list->hashtable->hashtable[index];
    if (base == NULL){
        var_list->hashtable->hashtable[index] = makeVar(name, value, name_);
        goto return_;
    }
    while (true){
        if (base->next != NULL)
            goto new_one;
        if (eqString(base->name, name))
            goto change;
        base = base->next;
    }
    new_one:
    base->next = makeVar(name, value, name_);
    goto return_;

    change:
    base->value = value;
    goto return_;

    return_:
    return;
}

LinkValue *findVar(char *name, VarList *var_list, bool del_var) {
    LinkValue *tmp = NULL;
    HASH_INDEX index = time33(name);
    Var *base = var_list->hashtable->hashtable[index];
    Var *last = NULL;
    if (base == NULL){
        goto return_;
    }
    while (base != NULL){
        if (eqString(base->name, name)){
            tmp = base->value;
            if (del_var){
                if (last == NULL)
                    var_list->hashtable->hashtable[index] = freeVar(base, true);
                else
                    last->next = freeVar(base, true);
            }
            goto return_;
        }
        last = base;
        base = base->next;
    }
    return_:
    return tmp;
}

LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, bool del_var) {
    LinkValue *tmp = NULL;
    for (NUMBER_TYPE i=0; i < times && var_list->next != NULL; i++){
        var_list = var_list->next;
    }
    while (var_list != NULL){
        tmp = findVar(name, var_list, del_var);
        if (tmp != NULL){
            goto return_;
        }
        var_list = var_list->next;
    }
    return_:
    return tmp;
}

void addFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, LinkValue *value, LinkValue *name_) {
    for (NUMBER_TYPE i=0; i < times && var_list->next != NULL; i++){
        var_list = var_list->next;
    }
    addVar(name, value, name_, var_list);
}

VarList *pushVarList(VarList *base, Inter *inter){
    VarList *new = makeVarList(inter);
    new->next = base;
    return new;
}

VarList *popVarList(VarList *base) {
    if (base->next == NULL)
        return base;
    return freeVarList(base, true);
}

VarList *copyVarListCore(VarList *base, Inter *inter){
    VarList *tmp = makeVarList(inter);
    tmp->hashtable = base->hashtable;
    return tmp;
}

VarList *copyVarList(VarList *base, bool n_new, Inter *inter){
    VarList *new;
    VarList *tmp;
    new = tmp = copyVarListCore(base, inter);
    while (base->next != NULL){
        tmp->next = copyVarListCore(base->next, inter);
        tmp = tmp->next;
        base = base->next;
    }
    if (n_new)
        new = pushVarList(new, inter);
    return new;
}
