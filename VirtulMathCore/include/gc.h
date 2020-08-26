#ifndef VIRTUALMATH_GC_H
#define VIRTUALMATH_GC_H
#define START_GC true

struct Inter;
struct Value;
struct LinkValue;
struct HashTable;
struct Var;
struct VarList;

struct GCStatus{
    long int tmp_link;  // tmp link的次数
    long int statement_link;  // statement link的次数
    long int link;  // 被直接link的次数
    enum {
        not_free,
        run_del,
        need_free,
    } c_value;  // value的计数 (先call __del__ 后释放)
    bool continue_;  // 是否迭代过
};

typedef struct GCStatus GCStatus;

void resetGC(GCStatus *gcs);
void setGC(GCStatus *gcs);
void gc_addTmpLink(GCStatus *gcs);
void gc_addLink(GCStatus *gcs);
void gc_addStatementLink(GCStatus *gcs);
void gc_freeStatementLink(GCStatus *gcs);
void gc_freeTmpLink(GCStatus *gcs);
bool gc_IterAlready(GCStatus *gcs);
bool gc_needFree(GCStatus *gcs);
void gc_resetValue(struct Value *value);
bool gc_needFreeValue(struct Value *value);

void gc_freeBase(struct Inter *inter);
void gc_checkDel(struct Inter *inter);
void gc_runDelAll(struct Inter *inter);
void gc_runDel(struct Inter *inter, struct VarList *var_list);
void gc_checkBase(struct Inter *inter);
void gc_resetBase(struct Inter *inter);

void gc_iterValue(struct Value *value);
void gc_iterLinkValue(struct LinkValue *value);
void gc_iterHashTable(struct HashTable *ht);
void gc_iterVar(struct Var *var);
void gc_varList(struct VarList *vl);

void gc_iterFreezeVarList(struct VarList *freeze, struct VarList *base, bool is_lock);
void gc_freeze(struct Inter *inter, struct VarList *freeze, struct VarList *base, bool is_lock);

void gc_run(struct Inter *inter, struct VarList *run_var, int var_list, int link_value, int value, ...);

#endif //VIRTUALMATH_GC_H
