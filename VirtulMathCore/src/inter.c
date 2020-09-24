#include "__virtualmath.h"
#define setName(str) strToWcs(str, false);

Inter *makeInter(char *out, char *error_, char *in, LinkValue *belong) {
    Inter *tmp = memCalloc(1, sizeof(Inter));
    tmp->base = NULL;
    tmp->link_base = NULL;
    tmp->hash_base = NULL;
    tmp->base_var = NULL;
    tmp->package = NULL;

    setBaseInterData(tmp);
    tmp->var_list = makeVarList(tmp, true);

    if (out != NULL) {
        tmp->data.inter_stdout = fopen(out, "w");
        tmp->data.is_stdout = false;
        if (tmp->data.inter_stdout == NULL)
            goto set_stdout;
    }
    else {
        set_stdout:
        tmp->data.inter_stdout = stdout;
        tmp->data.is_stdout = true;
    }

    if (error_ != NULL) {
        tmp->data.inter_stdout = fopen(error_, "w");
        tmp->data.is_stderr = false;
        if (tmp->data.inter_stdout == NULL)
            goto set_error_;
    }
    else {
        set_error_:
        tmp->data.inter_stderr = stderr;
        tmp->data.is_stderr = true;
    }

    if (in != NULL) {
        tmp->data.inter_stdin = fopen(in, "r");
        tmp->data.is_stdin = false;
        if (tmp->data.inter_stdin == NULL)
            goto set_stdin_;
    }
    else {
        set_stdin_:
        tmp->data.inter_stdin = stdin;
        tmp->data.is_stdin = true;
    }

    registeredFunctionName(tmp, belong);
    return tmp;
}

void setBaseInterData(struct Inter *inter){
    inter->data.var_str_prefix = setName("str_");
    inter->data.var_num_prefix = setName("num_");
    inter->data.var_none = setName("none");
    inter->data.var_pass = setName("ellipsis");
    inter->data.var_bool_prefix = setName("bool_");
    inter->data.var_class_prefix = setName("class_");
    inter->data.var_object_prefix = setName("obj_");
    inter->data.object_init = setName("__init__");
    inter->data.object_enter = setName("__enter__");
    inter->data.object_exit = setName("__exit__");
    inter->data.object_new = setName("__new__");
    inter->data.object_add = setName("__add__");
    inter->data.object_sub = setName("__sub__");
    inter->data.object_mul = setName("__mul__");
    inter->data.object_div = setName("__div__");
    inter->data.object_call = setName("__call__");
    inter->data.object_del = setName("__del__");
    inter->data.object_down = setName("__down__");
    inter->data.object_slice = setName("__slice__");
    inter->data.object_iter = setName("__iter__");
    inter->data.object_next = setName("__next__");
    inter->data.object_repo = setName("__repo__");
    inter->data.object_bool = setName("__bool__");
    inter->data.object_self = setName("__self__");
    inter->data.object_name = setName("__name__");
    inter->data.object_father = setName("__father__");
    inter->data.object_message = setName("__message__");
    inter->data.object_str = setName("__str__");
    inter->data.object_down_assignment = setName("__down_assignment__");
    inter->data.object_slice_assignment = setName("__slice_assignment__");
    inter->data.object_down_del = setName("__down_del__");
    inter->data.object_slice_del = setName("__slice_del__");
    inter->data.default_pt_type = free_;
}

void freeBaseInterData(struct Inter *inter){
    gc_freeStatementLink(&inter->base_belong->gc_status);
    gc_freeStatementLink(&inter->data.object->gc_status);
    gc_freeStatementLink(&inter->data.vobject->gc_status);
    gc_freeStatementLink(&inter->data.num->gc_status);
    gc_freeStatementLink(&inter->data.str->gc_status);
    gc_freeStatementLink(&inter->data.bool_->gc_status);
    gc_freeStatementLink(&inter->data.function->gc_status);
    gc_freeStatementLink(&inter->data.pass_->gc_status);
    gc_freeStatementLink(&inter->data.tuple->gc_status);
    gc_freeStatementLink(&inter->data.list->gc_status);
    gc_freeStatementLink(&inter->data.dict->gc_status);
    gc_freeStatementLink(&inter->data.list_iter->gc_status);
    gc_freeStatementLink(&inter->data.dict_iter->gc_status);
    gc_freeStatementLink(&inter->data.none->gc_status);

    gc_freeStatementLink(&inter->data.base_exc->gc_status);

    gc_freeStatementLink(&inter->data.sys_exc->gc_status);
    gc_freeStatementLink(&inter->data.keyInterrupt_exc->gc_status);
    gc_freeStatementLink(&inter->data.quit_exc->gc_status);

    gc_freeStatementLink(&inter->data.exc->gc_status);
    gc_freeStatementLink(&inter->data.type_exc->gc_status);
    gc_freeStatementLink(&inter->data.arg_exc->gc_status);
    gc_freeStatementLink(&inter->data.per_exc->gc_status);
    gc_freeStatementLink(&inter->data.result_exc->gc_status);
    gc_freeStatementLink(&inter->data.goto_exc->gc_status);
    gc_freeStatementLink(&inter->data.name_exc->gc_status);
    gc_freeStatementLink(&inter->data.assert_exc->gc_status);

    gc_freeStatementLink(&inter->data.key_exc->gc_status);
    gc_freeStatementLink(&inter->data.index_exc->gc_status);
    gc_freeStatementLink(&inter->data.stride_exc->gc_status);
    gc_freeStatementLink(&inter->data.iterstop_exc->gc_status);
    gc_freeStatementLink(&inter->data.super_exc->gc_status);
    gc_freeStatementLink(&inter->data.import_exc->gc_status);
    gc_freeStatementLink(&inter->data.include_exp->gc_status);

    memFree(inter->data.var_num_prefix);
    memFree(inter->data.var_str_prefix);
    memFree(inter->data.var_object_prefix);
    memFree(inter->data.var_class_prefix);
    memFree(inter->data.var_bool_prefix);
    memFree(inter->data.var_pass);
    memFree(inter->data.var_none);
    memFree(inter->data.object_init);
    memFree(inter->data.object_enter);
    memFree(inter->data.object_exit);
    memFree(inter->data.object_new);
    memFree(inter->data.object_add);
    memFree(inter->data.object_sub);
    memFree(inter->data.object_mul);
    memFree(inter->data.object_div);
    memFree(inter->data.object_call);
    memFree(inter->data.object_del);
    memFree(inter->data.object_down);
    memFree(inter->data.object_slice);
    memFree(inter->data.object_iter);
    memFree(inter->data.object_next);

    memFree(inter->data.object_repo);
    memFree(inter->data.object_bool);
    memFree(inter->data.object_name);
    memFree(inter->data.object_self);
    memFree(inter->data.object_father);
    memFree(inter->data.object_message);
    memFree(inter->data.object_str);
    memFree(inter->data.object_down_assignment);
    memFree(inter->data.object_slice_assignment);
    memFree(inter->data.object_down_del);
    memFree(inter->data.object_slice_del);

    if (!inter->data.is_stdout)
        fclose(inter->data.inter_stdout);
    if (!inter->data.is_stderr)
        fclose(inter->data.inter_stderr);
    if (!inter->data.is_stdin)
        fclose(inter->data.inter_stdin);
}

void freeInter(Inter *inter, bool show_gc) {
    FREE_BASE(inter, return_);
    gc_runDelAll(inter);
    freeBaseInterData(inter);
    freePackage(inter->package);
#if DEBUG
    if (show_gc && (printf("\nEnter '1' to show gc: "), getc(stdin) == '1')) {
        printGC(inter);
        while (getc(stdin) != '\n')
            PASS;
    }
#endif
    freeVarList(inter->var_list);
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

    gc_runDelAll(new);
    freeBaseInterData(new);
    freeVarList(new->var_list);

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
    if (base->package == NULL)
        base->package = new->package;
    memFree(new);
}

Inter *deriveInter(LinkValue *belong, Inter *inter) {
    Inter *import_inter = makeInter(NULL, NULL, NULL, belong);
    import_inter->data.inter_stdout = inter->data.inter_stdout;
    import_inter->data.inter_stderr = inter->data.inter_stderr;
    import_inter->data.inter_stdin = inter->data.inter_stdin;
    import_inter->data.is_stdout = true;
    import_inter->data.is_stderr = true;
    import_inter->data.is_stdin = true;
    return import_inter;
}

#if DEBUG
/* ***********************DEBUG 专用函数*********************************** */

void printGC(Inter *inter){
#if START_GC
    long int lv_st = 0;
    long int lv_tmp = 0;
    long int v_st = 0;
    long int v_tmp = 0;
    long int h_tmp = 0;
    printLinkValueGC("\n\nprintLinkValueGC TAG : freeInter", inter, &lv_tmp, &lv_st);
    printValueGC("\nprintValueGC TAG : freeInter", inter, &v_tmp, &v_st);
    printVarGC("\nprintVarGC TAG : freeInter", inter);
    printHashTableGC("\nprintHashTableGC TAG : freeInter", inter, &h_tmp);
    printf("\n");

    printf("linkvalue tmp       link = %ld\n", lv_tmp);
    printf("linkvalue statement link = %ld\n", lv_st);
    printf("    value tmp       link = %ld\n", v_tmp);
    printf("    value statement link = %ld\n", v_st);
    printf("hashtable tmp       link = %ld\n", h_tmp);
    printf("      tmp link     count = %ld\n", lv_tmp + v_tmp + h_tmp);
    printf("statement link     count = %ld\n", lv_st + v_st);
#endif

}

#if START_GC
void printLinkValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link) {
    LinkValue *base = inter->link_base;
    long tmp = 0;
    long st = 0;
    printf("%s\n", tag);
    while (base != NULL) {
        tmp += labs(base->gc_status.tmp_link);
        st += labs(base->gc_status.statement_link);
        if (base->gc_status.tmp_link != 0 || base->gc_status.statement_link != 0) {
            printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
            printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
            printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
            printLinkValue(base, "value = ", "\n", stdout);
            printf("-------------------------------------------\n");
        }
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("st link = %ld\n", st);
    printf("printLinkValueGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
    if (st_link != NULL) {
        *st_link = st;
    }
}

void printValueGC(char *tag, Inter *inter, long *tmp_link, long *st_link) {
    Value *base = inter->base;
    long tmp = 0;
    long st = 0;
    printf("%s\n", tag);
    while (base != NULL) {
        tmp += labs(base->gc_status.tmp_link);
        st += labs(base->gc_status.statement_link);
        if (base->gc_status.tmp_link != 0 || base->gc_status.statement_link != 0) {
            printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
            printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
            printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
            printf("value = ");
            printValue(base, stdout, true, true);
            printf("\n-------------------------------------------\n");
        }
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("st link = %ld\n", st);
    printf("printValueGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
    if (st_link != NULL)
        *st_link = st;
}

void printVarGC(char *tag, Inter *inter){
    Var *base = inter->base_var;
    printf("%s\n", tag);
    while (base != NULL) {
        if (base->gc_status.tmp_link != 0) {
            printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
            printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
            printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
        }

        printf("str_name = %ls\n", base->name);
        printf("name = ");
        printValue(base->name_->value, stdout, false, true);
        printf("\nvalue = ");
        printValue(base->value->value, stdout, false, true);
        printf("\n-------------------------------------------\n");
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
        if (base->gc_status.tmp_link != 0) {
            printf("inter->link_base.tmp_link       = %ld :: %p\n", base->gc_status.tmp_link, base);
            printf("inter->link_base.statement_link = %ld :: %p\n", base->gc_status.statement_link, base);
            printf("inter->link_base.link           = %ld :: %p\n", base->gc_status.link, base);
            printf("-------------------------------------------\n");
        }
        base = base->gc_next;
    }
    printf("tmp link = %ld\n", tmp);
    printf("printHashTableGC TAG : END\n");
    if (tmp_link != NULL)
        *tmp_link = tmp;
}

void printToken(Token *tk) {
    if (tk->token_type >= 0) {
        wchar_t *tmp = tk->data.str, *second_tmp = tk->data.second_str;
        if (eqWide(tmp,  L"\n"))
            tmp = L"\\n";
        if (eqWide(second_tmp, L"\n"))
            second_tmp = L"\\n";
        printf("<token str = ('%ls','%ls'), type = %d>", tmp, second_tmp, tk->token_type);
    }
    else
        printf("<token statement, type = %d>", tk->token_type);

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
#endif  // START_GC
#endif  // DBUG