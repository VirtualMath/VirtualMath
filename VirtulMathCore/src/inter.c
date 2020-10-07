#include "__virtualmath.h"
#define setName(str) memStrToWcs(str, false)

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
    inter->data.var_name[VN_str] = setName("str_");
    inter->data.var_name[VN_num] = setName("num_");
    inter->data.var_name[VN_file] = setName("file_");
    inter->data.var_name[VN_none] = setName("none");
    inter->data.var_name[VN_pass] = setName("ellipsis");
    inter->data.var_name[VN_bool] = setName("bool_");
    inter->data.var_name[VN_class] = setName("class_");
    inter->data.var_name[VN_obj] = setName("obj_");

    inter->data.mag_func[M_INIT] = setName("__init__");
    inter->data.mag_func[M_ENTER] = setName("__enter__");
    inter->data.mag_func[M_EXIT] = setName("__exit__");
    inter->data.mag_func[M_NEW] = setName("__new__");
    inter->data.mag_func[M_ADD] = setName("__add__");
    inter->data.mag_func[M_SUB] = setName("__sub__");
    inter->data.mag_func[M_MUL] = setName("__mul__");
    inter->data.mag_func[M_DIV] = setName("__div__");
    inter->data.mag_func[M_CALL] = setName("__call__");
    inter->data.mag_func[M_DEL] = setName("__del__");
    inter->data.mag_func[M_DOWN] = setName("__down__");
    inter->data.mag_func[M_SLICE] = setName("__slice__");
    inter->data.mag_func[M_ITER] = setName("__iter__");
    inter->data.mag_func[M_NEXT] = setName("__next__");
    inter->data.mag_func[M_REPO] = setName("__repo__");
    inter->data.mag_func[M_BOOL] = setName("__bool__");
    inter->data.mag_func[M_SELF] = setName("__self__");
    inter->data.mag_func[M_NAME] = setName("__name__");
    inter->data.mag_func[M_FATHER] = setName("__father__");
    inter->data.mag_func[M_MESSAGE] = setName("__message__");
    inter->data.mag_func[M_STR] = setName("__str__");
    inter->data.mag_func[M_DOWN_ASSIGMENT] = setName("__down_assignment__");
    inter->data.mag_func[M_SLICE_ASSIGMENT] = setName("__slice_assignment__");
    inter->data.mag_func[M_DOWN_DEL] = setName("__down_del__");
    inter->data.mag_func[M_SLICE_DEL] = setName("__slice_del__");
    inter->data.mag_func[M_ATTR] = setName("__attr__");
    inter->data.mag_func[M_VAL] = setName("__val__");

    inter->data.mag_func[M_INTDIV] = setName("__intdiv__");
    inter->data.mag_func[M_MOD] = setName("__mod__");
    inter->data.mag_func[M_POW] = setName("__pow__");

    inter->data.mag_func[M_BAND] = setName("__bitand__");
    inter->data.mag_func[M_BOR] = setName("__bitor__");
    inter->data.mag_func[M_BXOR] = setName("__bitxor__");
    inter->data.mag_func[M_BNOT] = setName("__bitnot__");
    inter->data.mag_func[M_BL] = setName("__bitleft__");
    inter->data.mag_func[M_BR] = setName("__bitright__");

    inter->data.mag_func[M_EQ] = setName("__eq__");
    inter->data.mag_func[M_MOREEQ] = setName("__moreeq__");
    inter->data.mag_func[M_LESSEQ] = setName("__lesseq__");
    inter->data.mag_func[M_MORE] = setName("__more__");
    inter->data.mag_func[M_LESS] = setName("__less__");
    inter->data.mag_func[M_NOTEQ] = setName("__noteq__");

    inter->data.mag_func[M_AND] = setName("__and__");
    inter->data.mag_func[M_OR] = setName("__or__");
    inter->data.mag_func[M_NOT] = setName("__not__");
    inter->data.mag_func[M_NEGATE] = setName("__negate__");

    inter->data.default_pt_type = free_;
}

void freeBaseInterData(struct Inter *inter){
    gc_freeStatementLink(&inter->base_belong->gc_status);

    for (int i=0; i < BASEOBJSZIE; i ++)
        gc_freeStatementLink(&inter->data.base_obj[i]->gc_status);

    for (int i=0; i < BASEEXCESIZE; i ++)
        gc_freeStatementLink(&inter->data.base_exc[i]->gc_status);

    for (int i=0; i < VARNAMESIZE; i ++)
        memFree(inter->data.var_name[i]);

    for (int i=0; i < MAGFUNCSIZE; i ++)
        memFree(inter->data.mag_func[i]);

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

#endif  // START_GC
#endif  // DBUG