#ifndef VIRTUALMATH_GC_H
#define VIRTUALMATH_GC_H

#define START_GC true

struct GCStatus{
    long int tmp_link;  // tmp link的次数
    long int statement_link;  // statement link的次数
    long int link;  // 被直接link的次数
    bool continue_;  // 是否迭代过
};

typedef struct GCStatus GCStatus;
struct Inter;
struct Value;
struct LinkValue;
struct HashTable;
struct Var;
struct VarList;


void resetGC(GCStatus *gcs);
void setGC(GCStatus *gcs);
void gcAddTmp(GCStatus *gcs);
void gcAddLink(GCStatus *gcs);
void gcAddStatementLink(GCStatus *gcs);
void gcFreeStatementLink(GCStatus *gcs);
void gcFreeTmpLink(GCStatus *gcs);
bool setIterAlready(GCStatus *gcs);
bool needFree(GCStatus *gcs);

void gc_freeBase(struct Inter *inter);
void gc_checkBase(struct Inter *inter);
void gc_resetBase(struct Inter *inter);

void gc_iterValue(struct Value *value);
void gc_iterLinkValue(struct LinkValue *value);
void gc_iterHashTable(struct HashTable *ht);
void gc_iterVar(struct Var *var);
void gc_varList(struct VarList *vl);

void iterFreezeVarList(struct VarList *freeze, struct VarList *base, bool is_lock);
void runFREEZE(struct Inter *inter, struct VarList *freeze, struct VarList *base, bool is_lock);

void runGC(struct Inter *inter, int var_list, int link_value, int value, ...);

#endif //VIRTUALMATH_GC_H
