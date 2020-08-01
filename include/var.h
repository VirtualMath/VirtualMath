#ifndef VIRTUALMATH_VAR_H
#define VIRTUALMATH_VAR_H
#include "inter.h"

#define MAX_SIZE (1024)

typedef struct Var{
    char *name;
    struct LinkValue *value;
    struct Var *next;
} Var;

typedef struct HashTable{
    struct Var **hashtable;
    int count;
    struct HashTable *next;
    struct HashTable *last;
} HashTable;

typedef struct VarList{
    struct HashTable *hashtable;
    struct VarList *next;
} VarList;

VarList *makeVarList(Inter *inter);
VarList *freeVarList(VarList *vl, bool self);
LinkValue *findFromVarList(char *name, VarList *var_list, NUMBER_TYPE times);
void addFromVarList(char *name, VarList *var_list, NUMBER_TYPE times, LinkValue *value);
void freeHashTable(HashTable *ht, Inter *inter);

VarList *pushVarList(VarList *base, Inter *inter);
VarList *popVarList(VarList *base);
VarList *copyVarList(VarList *base, bool n_new, Inter *inter);

#endif //VIRTUALMATH_VAR_H
