#include "__virtualmath.h"

Var *makeVar(char *name, LinkValue *value, LinkValue *name_, Inter *inter) {
    Var *list_tmp = inter->base_var;
    Var *tmp;
    tmp = memCalloc(1, sizeof(Var));
    setGC(&tmp->gc_status);
    tmp->name = memStrcpy(name);
    tmp->value = copyLinkValue(value, inter);
    tmp->name_ = copyLinkValue(name_, inter);
    tmp->next = NULL;

    tmp->gc_next = NULL;
    tmp->gc_last = NULL;

    if (list_tmp == NULL){
        inter->base_var = tmp;
        tmp->gc_last = NULL;
        goto return_;
    }

    for (PASS; list_tmp->gc_next !=  NULL; list_tmp = list_tmp->gc_next)
            PASS;
    list_tmp->gc_next = tmp;
    tmp->gc_last = list_tmp;

    return_:
    return tmp;
}

void freeVar(Var **var) {
    Var *free_value = *var;
    freeBase(free_value, return_);
    memFree(free_value->name);

    if ((*var)->gc_next != NULL)
        (*var)->gc_next->gc_last = (*var)->gc_last;
    *var = (*var)->gc_next;

    memFree(free_value);
    return_: return;
}

HashTable *makeHashTable(Inter *inter) {
    HashTable *list_tmp = inter->hash_base;
    HashTable *tmp;
    tmp = memCalloc(1, sizeof(Value));
    tmp->hashtable = (Var **)calloc(MAX_SIZE, sizeof(Var *));
    setGC(&tmp->gc_status);
    tmp->gc_next = NULL;
    tmp->gc_last = NULL;

    if (list_tmp == NULL){
        inter->hash_base = tmp;
        tmp->gc_last = NULL;
        goto return_;
    }

    for (PASS; list_tmp->gc_next != NULL; list_tmp = list_tmp->gc_next)
        PASS;
    list_tmp->gc_next = tmp;
    tmp->gc_last = list_tmp;

    return_:
    return tmp;
}

void freeHashTable(HashTable **value) {
    HashTable *free_value = *value;
    freeBase(free_value, return_);
    memFree(free_value->hashtable);

    if ((*value)->gc_next != NULL)
        (*value)->gc_next->gc_last = (*value)->gc_last;
    *value = (*value)->gc_next;

    memFree(free_value);
    return_: return;
}

VarList *makeVarList(Inter *inter) {
    VarList *tmp = calloc(1, sizeof(VarList));
    tmp->next = NULL;
    tmp->hashtable = makeHashTable(inter);
    tmp->default_var = NULL;
    return tmp;
}

VarList *freeVarList(VarList *vl, bool self){
    freeBase(vl, return_);
    for (PASS; vl->default_var != NULL; vl->default_var = freeDefaultVar(vl->default_var))
        PASS;
    if (self){
        VarList *next_var = vl->next;
        memFree(vl);
        return next_var;
    }
    return_:
    return vl;
}

DefaultVar *makeDefaultVar(char *name, NUMBER_TYPE times) {
    DefaultVar *tmp;
    tmp = memCalloc(1, sizeof(DefaultVar));
    tmp->name = memStrcpy(name);
    tmp->times = times;
    tmp->next = NULL;
    return tmp;
}

DefaultVar *freeDefaultVar(DefaultVar *dv) {
    DefaultVar *next = dv->next;
    memFree(dv->name);
    memFree(dv);
    return next;
}

DefaultVar *connectDefaultVar(DefaultVar *base, char *name, NUMBER_TYPE times) {
    for (DefaultVar **tmp = &base; PASS; tmp = &(*tmp)->next){
        if (*tmp == NULL){
            *tmp = makeDefaultVar(name, times);
            break;
        }
        if (eqString((*tmp)->name, name)){
            (*tmp)->times = times;
            break;
        }
    }
    return base;
}

NUMBER_TYPE findDefault(DefaultVar *base, char *name) {
    for (DefaultVar **tmp = &base; *tmp != NULL; tmp = &(*tmp)->next)
        if (eqString((*tmp)->name, name))
            return (*tmp)->times;
    return 0;
}

/**
 * hashTable使用time33算法
 * @param key
 * @return
 */
HASH_INDEX time33(char *key){ // hash function
    HASH_INDEX hash = 5381;
    while(*key)
        hash += (hash << (HASH_INDEX)5) + (*key++);
    return (hash & (HASH_INDEX)0x7FFFFFFF) % MAX_SIZE;
}

void addVarCore(Var **base, char *name, LinkValue *value, LinkValue *name_, Inter *inter) {
    for (PASS; true; base = &(*base)->next) {
        if (*base == NULL) {
            *base = makeVar(name, value, name_, inter);
            break;
        } else if (eqString((*base)->name, name)) {
            (*base)->value->value = value->value;
            (*base)->value->father = value->father;
            break;
        }
    }
}

void addVar(char *name, LinkValue *value, LinkValue *name_, INTER_FUNCTIONSIG_CORE) {
    HASH_INDEX index = time33(name);
    addVarCore(&var_list->hashtable->hashtable[index], name, value, name_, inter);
}

void updateHashTable(HashTable *update, HashTable *new, Inter *inter) {
    for (int i = 0; i < MAX_SIZE; i++)
        for (Var *tmp = new->hashtable[i]; tmp != NULL; tmp = tmp->next)
            addVarCore(&update->hashtable[i], tmp->name, tmp->value, tmp->name_, inter);
}


LinkValue *findVar(char *name, bool del_var, INTER_FUNCTIONSIG_CORE) {
    LinkValue *tmp = NULL;
    HASH_INDEX index = time33(name);

    for (Var **base = &var_list->hashtable->hashtable[index]; *base != NULL; base = &(*base)->next){
        if (eqString((*base)->name, name)){
            tmp = (*base)->value;
            if (del_var) {
                Var *next = (*base)->next;
                (*base)->next = NULL;
                *base = next;
            }
            goto return_;
        }
    }
    return_:
    return copyLinkValue(tmp, inter);
}

LinkValue *findFromVarList(char *name, NUMBER_TYPE times, bool del_var, INTER_FUNCTIONSIG_CORE) {
    LinkValue *tmp = NULL;
    NUMBER_TYPE base = findDefault(var_list->default_var, name) + times;
    for (NUMBER_TYPE i = 0; i < base && var_list->next != NULL; i++)
        var_list = var_list->next;
    for (PASS; var_list != NULL && tmp == NULL; var_list = var_list->next)
        tmp = findVar(name, del_var, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return tmp;
}

void addFromVarList(char *name, LinkValue *name_, NUMBER_TYPE times, LinkValue *value, INTER_FUNCTIONSIG_CORE) {
    NUMBER_TYPE base = findDefault(var_list->default_var, name) + times;
    for (NUMBER_TYPE i = 0; i < base && var_list->next != NULL; i++)
        var_list = var_list->next;
    addVar(name, value, name_, CALL_INTER_FUNCTIONSIG_CORE(var_list));
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
    VarList *new = NULL;
    VarList **tmp = &new;
    for (int i=0; base != NULL; tmp = &(*tmp)->next, base = base->next,i++)
        *tmp = copyVarListCore(base, inter);
    if (n_new)
        return pushVarList(new, inter);
    return new;
}

VarList *connectVarListBack(VarList *base, VarList *back){
    VarList **tmp = NULL;
    for (tmp = &base; *tmp != NULL; tmp = &(*tmp)->next)
        PASS;
    *tmp = back;
    return base;
}

bool comparVarList(VarList *dest, VarList *src) {
    for (PASS; src != NULL; src = src->next)
        if (src->hashtable == dest->hashtable)
            return true;
    return false;
}

VarList *connectSafeVarListBack(VarList *base, VarList *back){
    VarList **last_node = &base;
    for (PASS; *last_node != NULL; ){
        if ((*last_node)->hashtable == back->hashtable)
            *last_node = freeVarList(*last_node, true);
        else
            last_node = &(*last_node)->next;
    }
    *last_node = back;
    return base;
}

VarList *makeObjectVarList(FatherValue *value, Inter *inter){
    VarList *tmp = makeVarList(inter);
    for (PASS; value != NULL; value = value->next) {
        VarList *new = copyVarList(value->value->value->object.var, false, inter);
        tmp = connectVarListBack(tmp, new);
    }
    return tmp;
}
