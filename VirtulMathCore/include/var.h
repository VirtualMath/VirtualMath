#ifndef VIRTUALMATH_VAR_H
#define VIRTUALMATH_VAR_H

#define MAX_SIZE (8)

struct Var{
    GCStatus gc_status;
    wchar_t *name;
    struct LinkValue *value;
    struct LinkValue *name_;
    struct Var *next;
    struct Var *gc_next;
    struct Var *gc_last;
};

struct HashTable{
    GCStatus gc_status;
    struct Var **hashtable;
    struct HashTable *gc_next;
    struct HashTable *gc_last;
};

struct DefaultVar{
    wchar_t *name;
    vnum times;
    struct DefaultVar *next;
};

struct VarList{
    struct HashTable *hashtable;
    struct DefaultVar *default_var;
    struct VarList *next;
};

enum VarOperation {
    get_var,
    read_var,
    del_var,
};

typedef struct Var Var;
typedef struct HashTable HashTable;
typedef struct DefaultVar DefaultVar;
typedef struct VarList VarList;
typedef enum VarOperation VarOperation;

Var *makeVar(wchar_t *name, LinkValue *value, LinkValue *name_, Inter *inter);
void freeVar(Var **var);

HashTable *makeHashTable(Inter *inter);
void freeHashTable(HashTable **value);

VarList *makeVarList(Inter *inter, bool make_hash);
VarList *freeVarList(VarList *vl);

vhashn time33(wchar_t *key);
LinkValue *findVar(wchar_t *name, VarOperation operating, Inter *inter, HashTable *ht);
LinkValue *findFromVarList(wchar_t *name, vnum times, VarOperation operating, INTER_FUNCTIONSIG_CORE);
void addVar(wchar_t *name, LinkValue *value, LinkValue *name_, Inter *inter, HashTable *ht);
void updateHashTable(HashTable *update, HashTable *new, Inter *inter);
void addFromVarList(wchar_t *name, LinkValue *name_, vnum times, LinkValue *value, INTER_FUNCTIONSIG_CORE);

VarList *pushVarList(VarList *base, Inter *inter);
VarList *popVarList(VarList *base);
VarList *copyVarListCore(VarList *base, Inter *inter);
VarList *copyVarList(VarList *base, bool n_new, Inter *inter);
VarList *connectVarListBack(VarList *base, VarList *back);
bool comparVarList(VarList *dest, VarList *src);
VarList *makeObjectVarList(Inherit *value, Inter *inter, VarList *base);

vnum findDefault(DefaultVar *base, wchar_t *name);
DefaultVar *connectDefaultVar(DefaultVar *base, wchar_t *name, vnum times);
DefaultVar *freeDefaultVar(DefaultVar *dv);
DefaultVar *makeDefaultVar(wchar_t *name, vnum times);

#endif //VIRTUALMATH_VAR_H
