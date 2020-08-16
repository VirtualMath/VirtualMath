#include "__virtualmath.h"

Inter *makeInter(char *debug) {
    Inter *tmp = memCalloc(1, sizeof(Inter));
    Value *none_value = NULL;
    setBaseInterData(tmp);
    tmp->base = NULL;
    tmp->link_base = NULL;
    tmp->hash_base = NULL;
    tmp->base_var = NULL;

    tmp->var_list = makeVarList(tmp, true);
    tmp->data.log_dir = memStrcpy(debug);

    if (debug != NULL && !args.stdout_inter){
#ifdef __unix__
        char *debug_dir = memStrcat(debug, "/inter.log", false, false), *error_dir = memStrcat(debug, "/inter_error.log", false, false);
#else // __unix__
        char *debug_dir = memStrcat(debug, "\inter.log", false, false), *error_dir = memStrcat(debug, "\inter_error.log", false, false);
#endif // __unix__
        tmp->data.debug = fopen(debug_dir, "w");
        tmp->data.error = fopen(error_dir, "w");
        memFree(debug_dir);
        memFree(error_dir);
    }
    else {
        tmp->data.debug = stdout;
        tmp->data.error = stderr;
    }
    none_value = makeNoneValue(tmp);  // 注册None值
    gc_addStatementLink(&none_value->gc_status);
    return tmp;
}

void setBaseInterData(struct Inter *inter){
    inter->data.var_str_prefix = memStrcpy("str_");
    inter->data.var_num_prefix = memStrcpy("num_");
    inter->data.var_defualt = memStrcpy("default_var");
    inter->data.object_init = memStrcpy("__init__");
    inter->data.debug = NULL;
    inter->data.log_dir = NULL;
}

void freeBaseInterData(struct Inter *inter){
    memFree(inter->data.var_defualt);
    memFree(inter->data.var_num_prefix);
    memFree(inter->data.var_str_prefix);
    memFree(inter->data.object_init);
    memFree(inter->data.log_dir);

    if (inter->data.log_dir != NULL) {
        fclose(inter->data.debug);
        fclose(inter->data.error);
    }
}

void freeInter(Inter *inter, bool show_gc) {
    freeBase(inter, return_);

    if (show_gc && (printf("Enter '1' to show gc: "), getc(stdin) == '1')) {
        printLinkValueGC("\n\nprintLinkValueGC TAG : freeInter", inter);
        printValueGC("\nprintValueGC TAG : freeInter", inter);
        printVarGC("\nprintVarGC TAG : freeInter", inter);
        printHashTableGC("\nprintHashTableGC TAG : freeInter", inter);
        while (getc(stdin) != '\n')
            PASS;
    }

    freeVarList(inter->var_list);
    freeBaseInterData(inter);
    while (inter->base != NULL)
        freeValue(&inter->base);
    while (inter->base_var != NULL)
        freeVar(&inter->base_var);
    while (inter->link_base != NULL)
        freeLinkValue(&inter->link_base);
    while (inter->hash_base != NULL)
        freeHashTable(&inter->hash_base);
    memFree(inter);
    return_:
    return;
}

void mergeInter(Inter *new, Inter *base){
    Value **base_value = NULL;
    LinkValue **base_linkValue = NULL;
    HashTable **base_hash = NULL;
    Var **base_var = NULL;

    for (base_value = &base->base; *base_value != NULL; base_value = &(*base_value)->gc_next)
            PASS;
    for (base_linkValue = &base->link_base; *base_linkValue != NULL; base_linkValue = &(*base_linkValue)->gc_next)
            PASS;
    for (base_hash = &base->hash_base; *base_hash != NULL; base_hash = &(*base_hash)->gc_next)
            PASS;
    for (base_var = &base->base_var; *base_var != NULL; base_var = &(*base_var)->gc_next)
            PASS;

    *base_value = new->base;
    *base_linkValue = new->link_base;
    *base_hash = new->hash_base;
    *base_var = new->base_var;

    freeVarList(new->var_list);
    freeBaseInterData(new);
    memFree(new);
}

/* ***********************DEBUG 专用函数*********************************** */

void printLinkValueGC(char *tag, Inter *inter){
    LinkValue *base = inter->link_base;
    printf("%s\n", tag);
    while (base != NULL) {
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printLinkValue(base, "value = ", "\n", stdout);
        printf("-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("printLinkValueGC TAG : END\n");
}

void printValueGC(char *tag, Inter *inter){
    Value *base = inter->base;
    printf("%s\n", tag);
    while (base != NULL) {
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printf("value = ");
        printValue(base, stdout);
        printf("\n-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("printValueGC TAG : END\n");
}

void printVarGC(char *tag, Inter *inter){
    Var *base = inter->base_var;
    printf("%s\n", tag);
    while (base != NULL) {
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printf("value :\n");
        printLinkValue(base->name_, "name_: ", "\n", stdout);
        printLinkValue(base->value, "value_: ", "\n", stdout);
        printf("str_name = %s\n", base->name);
        printf("-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("printVarGC TAG : END\n");
}

void printHashTableGC(char *tag, Inter *inter){
    HashTable *base = inter->hash_base;
    printf("%s\n", tag);
    while (base != NULL) {
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printf("-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("printHashTableGC TAG : END\n");
}