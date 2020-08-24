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

struct DefaultVar{
    char *name;
    NUMBER_TYPE times;
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

Var *makeVar(char *name, LinkValue *value, LinkValue *name_, Inter *inter);
void freeVar(Var **var);

HashTable *makeHashTable(Inter *inter);
void freeHashTable(HashTable **value);

VarList *makeVarList(Inter *inter, bool make_hash);
VarList *freeVarList(VarList *vl);

HASH_INDEX time33(char *key);
LinkValue *findVar(char *name, VarOperation operating, Inter *inter, HashTable *ht);
LinkValue *findFromVarList(char *name, NUMBER_TYPE times, VarOperation operating, INTER_FUNCTIONSIG_CORE);
void addVar(char *name, LinkValue *value, LinkValue *name_, Inter *inter, HashTable *ht);
void updateHashTable(HashTable *update, HashTable *new, Inter *inter);
void addFromVarList(char *name, LinkValue *name_, NUMBER_TYPE times, LinkValue *value, INTER_FUNCTIONSIG_CORE);

VarList *pushVarList(VarList *base, Inter *inter);
VarList *popVarList(VarList *base);
VarList *copyVarListCore(VarList *base, Inter *inter);
VarList *copyVarList(VarList *base, bool n_new, Inter *inter);
VarList *connectVarListBack(VarList *base, VarList *back);
bool comparVarList(VarList *dest, VarList *src);
VarList *makeObjectVarList(Inherit *value, Inter *inter, VarList *base);

NUMBER_TYPE findDefault(DefaultVar *base, char *name);
DefaultVar *connectDefaultVar(DefaultVar *base, char *name, NUMBER_TYPE times);
DefaultVar *freeDefaultVar(DefaultVar *dv);
DefaultVar *makeDefaultVar(char *name, NUMBER_TYPE times);

#endif //VIRTUALMATH_VAR_H
