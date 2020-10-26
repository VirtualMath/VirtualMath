#include "__virtualmath.h"

Var *makeVar(wchar_t *name, LinkValue *value, LinkValue *name_, Inter *inter) {
    Var *tmp;
    MACRO_CALLOC(tmp, 1, sizeof(Var));
    setGC(&tmp->gc_status);
    tmp->name = memWidecpy(name);
    tmp->value = value;
    tmp->name_ = name_;
    tmp->next = NULL;
    tmp->gc_next = NULL;

    tmp->gc_next = inter->base_var;
    tmp->gc_last = NULL;
    if (inter->base_var != NULL)
        inter->base_var->gc_last = tmp;
    inter->base_var = tmp;
    return tmp;
}

void freeVar(Var **var) {
    Var *free_value = *var;
    FREE_BASE(free_value, return_);
    memFree(free_value->name);

    if ((*var)->gc_next != NULL)
        (*var)->gc_next->gc_last = (*var)->gc_last;
    *var = (*var)->gc_next;

    memFree(free_value);
    return_: return;
}

HashTable *makeHashTable(Inter *inter) {
    HashTable *tmp;
    MACRO_CALLOC(tmp, 1, sizeof(HashTable));
    MACRO_CALLOC(tmp->hashtable, MAX_SIZE, sizeof(Var *));
    setGC(&tmp->gc_status);
    gc_addTmpLink(&tmp->gc_status);
    tmp->gc_next = NULL;

    // hashTable 不算入 inter 的 run_gc 中
    tmp->gc_next = inter->hash_base;
    tmp->gc_last = NULL;
    if (inter->hash_base != NULL)
        inter->hash_base->gc_last = tmp;
    inter->hash_base = tmp;
    return tmp;
}

void freeHashTable(HashTable **value) {
    HashTable *free_value = *value;
    FREE_BASE(free_value, return_);
    memFree(free_value->hashtable);

    if ((*value)->gc_next != NULL)
        (*value)->gc_next->gc_last = (*value)->gc_last;
    *value = (*value)->gc_next;

    memFree(free_value);
    return_: return;
}

VarList *makeVarList(Inter *inter, bool make_hash, HashTable *hs) {
    VarList *tmp = calloc(1, sizeof(VarList));
    tmp->next = NULL;
    if (make_hash)
        tmp->hashtable = makeHashTable(inter);
    else {
        assert(hs != NULL);
        tmp->hashtable = hs;
        gc_addTmpLink(&tmp->hashtable->gc_status);
    }
    tmp->default_var = NULL;
    return tmp;
}

VarList *freeVarList(VarList *vl) {
    VarList *next_var = NULL;
    FREE_BASE(vl, return_);
    next_var = vl->next;
    for (PASS; vl->default_var != NULL; vl->default_var = freeDefaultVar(vl->default_var))
        PASS;
    if (vl->hashtable != NULL)
        gc_freeTmpLink(&vl->hashtable->gc_status);
    memFree(vl);
    return_:
    return next_var;
}

DefaultVar *makeDefaultVar(wchar_t *name, vint times) {
    DefaultVar *tmp;
    tmp = memCalloc(1, sizeof(DefaultVar));
    tmp->name = memWidecpy(name);
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

DefaultVar *connectDefaultVar(DefaultVar *base, wchar_t *name, vint times) {
    for (DefaultVar **tmp = &base; PASS; tmp = &(*tmp)->next){
        if (*tmp == NULL){
            *tmp = makeDefaultVar(name, times);
            break;
        }
        if (eqWide((*tmp)->name, name)){
            (*tmp)->times = times;
            break;
        }
    }
    return base;
}

vint findDefault(DefaultVar *base, wchar_t *name) {
    for (DefaultVar **tmp = &base; *tmp != NULL; tmp = &(*tmp)->next)
        if (eqWide((*tmp)->name, name))
            return (*tmp)->times;
    return 0;
}

/**
 * hashTable使用time33算法
 * @param key
 * @return
 */
vhashn time33(wchar_t *key){ // hash func
    vhashn hash = 5381;
    while(*key)
        hash += (hash << (vhashn)5) + (*key++);
    return (hash & (vhashn)0x7FFFFFFF) % MAX_SIZE;
}

static void addVarCore(Var **base, wchar_t *name, LinkValue *value, LinkValue *name_, Inter *inter) {
    for (PASS; true; base = &(*base)->next) {
        if (*base == NULL) {
            *base = makeVar(name, value, name_, inter);
            break;
        } else if (eqWide((*base)->name, name)) {
            (*base)->value = value;
            break;
        }
    }
}

void addVar(wchar_t *name, LinkValue *value, LinkValue *name_, Inter *inter, HashTable *ht) {
    vhashn index = time33(name);
    addVarCore(&ht->hashtable[index], name, value, name_, inter);
}

void updateHashTable(HashTable *update, HashTable *new, Inter *inter) {
    for (int i = 0; i < MAX_SIZE; i++)
        for (Var *tmp = new->hashtable[i]; tmp != NULL; tmp = tmp->next)
            addVarCore(&update->hashtable[i], tmp->name, tmp->value, tmp->name_, inter);
}


LinkValue *findVar(wchar_t *name, VarOperation operating, Inter *inter, HashTable *ht) {
    LinkValue *tmp = NULL;
    vhashn index = time33(name);
    for (Var **base = &ht->hashtable[index]; *base != NULL; base = &(*base)->next){
        if (eqWide((*base)->name, name)){
            tmp = (*base)->value;
            if (operating == del_var) {
                Var *next = (*base)->next;
                (*base)->next = NULL;
                *base = next;
            }
            goto return_;
        }
    }
    return_: return tmp;
}

/**
 * @param name
 * @param times
 * @param operating read_var-不复制读取 get_var-复制读取 del_var-删除且返回(不复制)
 * @param inter
 * @param var_list
 * @return
 */
LinkValue *findFromVarList(wchar_t *name, vint times, VarOperation operating, FUNC_CORE) {
    LinkValue *tmp = NULL;
    vint base = findDefault(var_list->default_var, name) + times;
    for (vint i = 0; i < base && var_list->next != NULL; i++)
        var_list = var_list->next;
    if (operating == del_var && var_list != NULL)
        tmp = findVar(name, del_var, inter, var_list->hashtable);
    else {
        for (PASS; var_list != NULL && tmp == NULL; var_list = var_list->next)
            tmp = findVar(name, operating, inter, var_list->hashtable);
    }
    return tmp;
}

void addFromVarList(wchar_t *name, LinkValue *name_, vint times, LinkValue *value, FUNC_CORE) {
    vint base = findDefault(var_list->default_var, name) + times;
    for (vint i = 0; i < base && var_list->next != NULL; i++)
        var_list = var_list->next;
    addVar(name, value, name_, inter, var_list->hashtable);
}

VarList *pushVarList(VarList *base, Inter *inter){
    VarList *new = makeVarList(inter, true, NULL);
    new->next = base;
    return new;
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

VarList *makeObjectVarList(Inherit *value, Inter *inter, VarList *base) {
    VarList *tmp = base == NULL ? makeVarList(inter, true, NULL) : base;
    VarList *next = tmp;
    assert(tmp != NULL);
    while (next->next != NULL)
        next = next->next;
    for (PASS; value != NULL;next = next->next, value = value->next)
        next->next = copyVarListCore(value->value->value->object.var, inter);  // 复制一个
    return tmp;
}
