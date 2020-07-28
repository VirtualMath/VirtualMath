#include "__virtualmath.h"

Inter *makeInter(char *debug){
    Inter *tmp = memCalloc(1, sizeof(Inter));
    tmp->base = NULL;
    tmp->link_base = NULL;
    tmp->statement = makeStatement();
    tmp->var_list = makeVarList(tmp);
    tmp->log_dir = memStrcpy(debug, 0, false, false);
    makeValue(tmp);  // 注册None值
    if (debug != NULL && !args.stdout_inter){
        char *debug_dir = memStrcat(debug, INTER_LOG);
        tmp->debug = fopen(debug_dir, "w");
        memFree(debug_dir);
    }
    else
        tmp->debug = stdout;
    return tmp;
}

void freeInter(Inter *inter, bool self){
    freeBase(inter, return_);
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
    memFree(inter->log_dir);
    if (inter->log_dir != NULL)
        fclose(inter->debug);
    if (self){
        memFree(inter);
    }
    return_:
    return;
}
