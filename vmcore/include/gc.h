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
#define gc_addTmpLink(gcs) ((gcs)->tmp_link ++)
#define gc_addStatementLink(gcs) ((gcs)->statement_link ++)
#define gc_addLink(gcs) ((gcs)->link ++)
#define gc_freeTmpLink(gcs) ((gcs)->tmp_link --)
#define gc_freeStatementLink(gcs) ((gcs)->statement_link --)
#define setGC(gcs) ((gcs)->continue_ = false, (gcs)->link = 0, (gcs)->tmp_link = 0, (gcs)->statement_link = 0, (gcs)->c_value = not_free)
void gc_runDelAll(struct Inter *inter);
void gc_run(struct Inter *inter, struct VarList *run_var, int var_list, int link_value, int value, ...);
#else
#define gc_addTmpLink(gcs) ((void)0)
#define gc_addStatementLink(gcs) ((void)0)
#define gc_addLink(gcs) ((void)0)
#define gc_freeTmpLink(gcs) ((void)0)

#define gc_run(...) ((void)0)
#define setGC(...) ((void)0)
#define gc_freeStatementLink(gcs) ((void)0)
#define gc_runDelAll(...) ((void)0)
#endif

#endif //VIRTUALMATH_GC_H
