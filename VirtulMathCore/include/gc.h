#ifndef VIRTUALMATH_GC_H
#define VIRTUALMATH_GC_H

struct Inter;
struct Value;
struct LinkValue;
struct HashTable;
struct Var;
struct VarList;

#if START_GC
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
#else
struct GCStatus {};
#endif

typedef struct GCStatus GCStatus;


#if START_GC
void setGC(GCStatus *gcs);
void gc_addTmpLink(GCStatus *gcs);
void gc_addStatementLink(GCStatus *gcs);
void gc_freeTmpLink(GCStatus *gcs);
void gc_freeStatementLink(GCStatus *gcs);
void gc_runDelAll(struct Inter *inter);

void gc_freeze(struct Inter *inter, struct VarList *freeze, struct VarList *base, bool is_lock);
void gc_run(struct Inter *inter, struct VarList *run_var, int var_list, int link_value, int value, ...);
#else
#define gc_freeze(...)
#define gc_run(...)
#define setGC(...)
#define gc_addTmpLink(...)
#define gc_addStatementLink(...)
#define gc_freeTmpLink(...)
#define gc_freeStatementLink(...)
#define gc_runDelAll(...)
#endif

#endif //VIRTUALMATH_GC_H
