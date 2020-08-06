#ifndef VIRTUALMATH_VAR_H
#define VIRTUALMATH_VAR_H

#define MAX_SIZE (1024)

struct Var{
    char *name;
    struct LinkValue *value;
    struct LinkValue *name_;
    struct Var *next;
};

struct HashTable{
    GCStatus gc_status;
    struct Var **hashtable;
    struct HashTable *next;
    struct HashTable *last;
};

struct VarList{
    struct HashTable *hashtable;
    struct VarList *next;
};

typedef struct Var Var;
typedef struct HashTable HashTable;
typedef struct VarList VarList;

Var *makeVar(char *name, LinkValue *value, LinkValue *name_);
Var *freeVar(Var *var, bool self);

HashTable *makeHashTable(Inter *inter);
HashTable * freeHashTable(HashTable *ht, Inter *inter);

VarList *makeVarList(Inter *inter);
VarList *freeVarList(VarList *vl, bool self);

HASH_INDEX time33(char *key);
LinkValue *findVar(char *name, VarList *var_list, bool del_var);
LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, bool del_var);
void addVar(char *name, LinkValue *value, LinkValue *name_, VarList *var_list);
void addFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, LinkValue *value, LinkValue *name_);

VarList *pushVarList(VarList *base, Inter *inter);
VarList *popVarList(VarList *base);
VarList *copyVarList(VarList *base, bool n_new, Inter *inter);

#endif //VIRTUALMATH_VAR_H
