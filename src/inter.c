#include "__virtualmath.h"

Inter *runBaseInter(char *code_file, char *debug_dir, int *status) {
    Result global_result;
    return newInter(code_file, debug_dir, &global_result, status);
}

Inter *newInter(char *code_file, char *debug_dir, Result *global_result, int *status) {
    Inter *global_inter = NULL;
    ParserMessage *pm = NULL;
    *status = 0;

    if (access(code_file, R_OK) != 0){
        *status = 1;
        return NULL;
    }

    if (access(debug_dir, R_OK) != 0)
        debug_dir = NULL;

    global_inter = makeInter(debug_dir);
    pm = makeParserMessage(code_file, debug_dir);

    parserCommandList(pm, global_inter, true, global_inter->statement);
    if (pm->status != success){
        writeLog(pm->paser_debug, ERROR, "Syntax Error: %s\n", pm->status_message);
        writeLog(stderr, ERROR, "Syntax Error: %s\n", pm->status_message);
        goto return_;
    }

    *global_result = globalIterStatement(global_inter);
    if (global_result->type == error_return){
        writeLog(global_inter->data.debug, ERROR, "Run Error\n", NULL);
        writeLog(stderr, ERROR, "Run Error\n", NULL);
    }

    return_:
    freeParserMessage(pm, true);
    return global_inter;
}

Inter *makeInter(char *debug){
    Inter *tmp = memCalloc(1, sizeof(Inter));
    setBaseInterData(tmp);
    tmp->base = NULL;
    tmp->link_base = NULL;
    tmp->statement = makeStatement();
    tmp->var_list = makeVarList(tmp);
    tmp->data.log_dir = memStrcpy(debug, 0, false, false);

    if (debug != NULL && !args.stdout_inter){
        char *debug_dir = memStrcat(debug, INTER_LOG);
        tmp->data.debug = fopen(debug_dir, "w");
        memFree(debug_dir);
    }
    else
        tmp->data.debug = stdout;

    makeValue(tmp);  // 注册None值
    return tmp;
}

void setBaseInterData(struct Inter *inter){
    inter->data.var_str_prefix = "str_";
    inter->data.var_num_prefix = "num_";
    inter->data.var_defualt = "default_var";
    inter->data.debug = NULL;
    inter->data.log_dir = NULL;
}

void freeInter(Inter *inter, bool self){
    freeBase(inter, return_);
    while (inter->base != NULL)
        freeValue(inter->base, inter);

    while (inter->link_base != NULL)
        freeLinkValue(inter->link_base, inter);

    while (inter->hash_base != NULL)
        freeHashTable(inter->hash_base, inter, true);

    freeStatement(inter->statement);
    freeVarList(inter->var_list, true);
    memFree(inter->data.log_dir);
    if (inter->data.log_dir != NULL)
        fclose(inter->data.debug);
    if (self){
        memFree(inter);
    }
    return_:
    return;
}
