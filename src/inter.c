#include "__virtualmath.h"

Inter *makeInter(char *debug, LinkValue *father) {
    Inter *tmp = memCalloc(1, sizeof(Inter));
    LinkValue *base_father = NULL;
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

    registeredFunctionName(tmp);

    tmp->data.none = makeNoneValue(tmp);
    gc_addStatementLink(&tmp->data.none->gc_status);

    {
        VarList *out_var = copyVarList(tmp->var_list, false, tmp);
        Value *base_father_value = makeObject(tmp, out_var, NULL, NULL);
        base_father = makeLinkValue(base_father_value, father, tmp);
        gc_addStatementLink(&base_father->gc_status);
        tmp->base_father = base_father;
    }

    registeredBaseFunction(base_father, tmp);
    return tmp;
}

void setBaseInterData(struct Inter *inter){
    inter->data.object = NULL;
    inter->data.vobject = NULL;
    inter->data.num = NULL;
    inter->data.str = NULL;
    inter->data.none = NULL;
    inter->data.var_str_prefix = memStrcpy("str_");
    inter->data.var_num_prefix = memStrcpy("num_");
    inter->data.var_defualt = memStrcpy("default_var");
    inter->data.object_init = memStrcpy("__init__");
    inter->data.object_enter = memStrcpy("__enter__");
    inter->data.object_exit = memStrcpy("__exit__");
    inter->data.object_new = memStrcpy("__new__");
    inter->data.object_add = memStrcpy("__add__");
    inter->data.object_sub = memStrcpy("__sub__");
    inter->data.object_mul = memStrcpy("__mul__");
    inter->data.object_div = memStrcpy("__div__");
    inter->data.default_pt_type = free_;

}

void freeBaseInterData(struct Inter *inter){
    gc_freeStatementLink(&inter->data.object->gc_status);
    gc_freeStatementLink(&inter->data.vobject->gc_status);
    gc_freeStatementLink(&inter->data.num->gc_status);
    gc_freeStatementLink(&inter->data.str->gc_status);
    gc_freeStatementLink(&inter->data.bool_->gc_status);
    gc_freeStatementLink(&inter->data.function->gc_status);
    gc_freeStatementLink(&inter->data.pass_->gc_status);
    gc_freeStatementLink(&inter->data.list->gc_status);
    gc_freeStatementLink(&inter->data.dict->gc_status);
    memFree(inter->data.var_num_prefix);
    memFree(inter->data.var_str_prefix);
    memFree(inter->data.var_defualt);
    memFree(inter->data.object_init);
    memFree(inter->data.object_enter);
    memFree(inter->data.object_exit);
    memFree(inter->data.object_new);
    memFree(inter->data.object_add);
    memFree(inter->data.object_sub);
    memFree(inter->data.object_mul);
    memFree(inter->data.object_div);

    memFree(inter->data.log_dir);
    if (inter->data.log_dir != NULL) {
        fclose(inter->data.debug);
        fclose(inter->data.error);
    }
}

void freeInter(Inter *inter, bool show_gc) {
    freeBase(inter, return_);

    gc_freeStatementLink(&inter->base_father->gc_status);
    inter->base_father = NULL;

    freeVarList(inter->var_list);
    freeBaseInterData(inter);

    if (show_gc && (printf("Enter '1' to show gc: "), getc(stdin) == '1')) {
        printGC(inter);
        while (getc(stdin) != '\n')
            PASS;
    }

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

    gc_freeStatementLink(&new->base_father->gc_status);
    new->base_father = NULL;

    freeVarList(new->var_list);
    freeBaseInterData(new);

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
    memFree(new);
}

/* ***********************DEBUG 专用函数*********************************** */

void printGC(Inter *inter){
    long int st_lv = 0;
    long int tmp_lv = 0;
    long int st_v = 0;
    long int tmp_v = 0;
    long int tmp_h = 0;
    printLinkValueGC("\n\nprintLinkValueGC TAG : freeInter", inter, &tmp_lv, &st_lv);
    printValueGC("\nprintValueGC TAG : freeInter", inter, &tmp_v, &st_v);
    printVarGC("\nprintVarGC TAG : freeInter", inter);
    printHashTableGC("\nprintHashTableGC TAG : freeInter", inter, &tmp_h);
    printf("\n");
    printf("linkvalue tmp link   = %ld\n", tmp_lv);
    printf("linkvalue tmp link   = %ld\n", tmp_v);
    printf("hashtable tmp link   = %ld\n", tmp_h);
    printf("statement tmp link   = %ld\n", st_lv);
    printf("statement tmp link   = %ld\n", st_v);
    printf("      tmp link count = %ld\n", tmp_lv + tmp_v + tmp_h);
    printf("statement link count = %ld\n", st_lv + st_v);

}

void printLinkValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link) {
    LinkValue *base = inter->link_base;
    long tmp = 0;
    long st = 0;
    printf("%s\n", tag);
    while (base != NULL) {
        tmp += labs(base->gc_status.tmp_link);
        st += labs(base->gc_status.statement_link);
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printLinkValue(base, "value = ", "\n", stdout);
        printf("-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("st link = %ld\n", st);
    printf("printLinkValueGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
    if (st_link != NULL)
        *st_link = tmp;
}

void printValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link) {
    Value *base = inter->base;
    long tmp = 0;
    long st = 0;
    printf("%s\n", tag);
    while (base != NULL) {
        tmp += labs(base->gc_status.tmp_link);
        st += labs(base->gc_status.statement_link);
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printf("value = ");
        printValue(base, stdout, true);
        printf("\n-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("st link = %ld\n", st);
    printf("printValueGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
    if (st_link != NULL)
        *st_link = tmp;
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

void printHashTableGC(char *tag, Inter *inter, long *tmp_link) {
    HashTable *base = inter->hash_base;
    long tmp = 0;
    printf("%s\n", tag);
    while (base != NULL) {
        tmp += labs(base->gc_status.tmp_link);
        printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
        printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
        printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        printf("-------------------------------------------\n");
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("printHashTableGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
}

void printToken(Token *tk) {
    if (tk->token_type >= 0) {
        char *tmp = tk->data.str, *second_tmp = tk->data.second_str;
        if (!strcmp(tmp, "\n")) {
            tmp = "\\n";
        }
        if (!strcmp(second_tmp, "\n")) {
            second_tmp = "\\n";
        }
        if (tmp[0] == EOF) {
            tmp = "(EOF)";
        }
        printf("<token str = ('%s','%s'), type = %d>", tmp, second_tmp, tk->token_type);
    }
    else{
        printf("<token statement, type = %d>", tk->token_type);
    }

}

void printTokenStream(TokenStream *ts) {
    printf("token_list: ");
    Token *tmp = ts->token_list;
    int i = 0;
    while (tmp != NULL){
        if (i > 0)
            printf("-");
        printToken(tmp);
        tmp = tmp->next;
        i++;
    }
    printf("\n");
}