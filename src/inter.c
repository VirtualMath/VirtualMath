#include "__virtualmath.h"

Inter *makeInter(){
    Inter *tmp = memCalloc(1, sizeof(Inter));
    tmp->base = NULL;
    tmp->link_base = NULL;
    tmp->statement = makeStatement();
    tmp->var_list = makeVarList(tmp);
    return tmp;
}

void freeInter(Inter *inter, bool self){
    while (inter->base != NULL){
        freeValue(inter->base, inter);
    }
    while (inter->link_base != NULL){
        freeLinkValue(inter->link_base, inter);
    }
    while (inter->hash_base != NULL){
        freeHashTable(inter->hash_base, inter);
    }
    freeStatement(inter->statement);
    freeVarList(inter->var_list, true);
    if (self){
        memFree(inter);
    }
}
