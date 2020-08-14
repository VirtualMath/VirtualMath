#ifndef VIRTUALMATH_VAR_H
#define VIRTUALMATH_VAR_H

#define MAX_SIZE (1024)

struct Var{
    GCStatus gc_status;
    char *name;
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

struct VarList{
    struct HashTable *hashtable;
    struct VarList *next;
};

typedef struct Var Var;
typedef struct HashTable HashTable;
typedef struct VarList VarList;

Var *makeVar(char *name, LinkValue *value, LinkValue *name_, Inter *inter);
void freeVar(Var **var);

HashTable *makeHashTable(Inter *inter);
void freeHashTable(HashTable **value);

VarList *makeVarList(Inter *inter);
VarList *freeVarList(VarList *vl, bool self);

HASH_INDEX time33(char *key);
LinkValue *findVar(char *name, VarList *var_list, bool del_var);
LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, bool del_var);
void addVar(char *name, LinkValue *value, LinkValue *name_, INTER_FUNCTIONSIG_CORE);
void addFromVarList(char *name, LinkValue *name_, NUMBER_TYPE times, LinkValue *value, INTER_FUNCTIONSIG_CORE);

VarList *pushVarList(VarList *base, Inter *inter);
VarList *popVarList(VarList *base);
VarList *copyVarList(VarList *base, bool n_new, Inter *inter);
VarList *connectVarListBack(VarList *base, VarList *back);
bool comparVarList(VarList *dest, VarList *src);
VarList *connectSafeVarListBack(VarList *base, VarList *back);
VarList *makeObjectVarList(FatherValue *value, Inter *inter);

#endif //VIRTUALMATH_VAR_H
