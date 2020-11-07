#include "__ofunc.h"

static ResultType object_new(O_FUNC){
    LinkValue *value = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

#define OBJ_OPT(M_NAME_, NAME, FUNC) static ResultType object_##FUNC (O_FUNC) {  \
    LinkValue *left, *right;  \
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},  \
                           {.type=name_value, .name=L"left", .must=1, .long_arg=false},  \
                           {.type=name_value, .name=L"right", .must=1, .long_arg=false},  \
                           {.must=-1}};  \
    setResultCore(result);  \
    parserArgumentUnion(ap, arg, CNEXT_NT);  \
    if (!CHECK_RESULT(result))  \
        return result->type;  \
    freeResult(result);  \
    left = ap[1].value;  \
    right = ap[2].value;  \
    if (left->value == ap[0].value->value)  \
        runOperationFromValue(right, left, right, inter->data.mag_func[M_##M_NAME_], LINEFILE, CNEXT_NT);  /*调用左边的数值*/  \
    else  \
        setResultError(E_TypeException, CUL_ERROR(NAME), LINEFILE, true, CNEXT_NT);  \
    return result->type;  \
}

OBJ_OPT(ADD, Add, add)
OBJ_OPT(SUB, Sub, sub)
OBJ_OPT(MUL, Mul, mul)
OBJ_OPT(DIV, Div, div)
OBJ_OPT(INTDIV, Int Div, intdiv)
OBJ_OPT(MOD, Mod, mod)
OBJ_OPT(POW, Pow, pow)

OBJ_OPT(MOREEQ, More Eq, moreeq)
OBJ_OPT(LESSEQ, Less Eq, lesseq)
OBJ_OPT(MORE, More, more)
OBJ_OPT(LESS, Less, less)

OBJ_OPT(BAND, Bit And, band)
OBJ_OPT(BOR, Bit Or, bor)
OBJ_OPT(BXOR, Bit Xor, bxor)
OBJ_OPT(BL, Bit Left, bl)
OBJ_OPT(BR, Bit Right, br)

#undef OBJ_OPT

static ResultType object_eq (O_FUNC) {
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"left", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"right", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    makeBoolValue(ap[1].value->value == ap[2].value->value, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType object_noteq (O_FUNC) {
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"left", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"right", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    makeBoolValue(ap[1].value->value != ap[2].value->value, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType objectRepoStrCore(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo;
    wchar_t *name;
    wchar_t *type;
    LinkValue *name_value;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    name_value = findAttributes(inter->data.mag_func[M_NAME], false, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (name_value != NULL){
        if (name_value->value->type == V_str)
            name = name_value->value->data.str.str;
        else {
            setResultError(E_TypeException, ONLY_ACC(obj.__name__, str), LINEFILE, true, CNEXT_NT);
            return result->type;
        }
    } else
        name = L"unknown";

    if (ap[0].value->value->type == V_none) {
        repo = memWidecpy(L"null");
    } else {
        size_t len;
        if (ap[0].value->value->type == V_class)
            type = L"class";
        else
            type = L"object";
        len = memWidelen(name) + 30;
        repo = memWide(len);
        swprintf(repo, len, L"(%ls: %ls on %p)", type, name, ap[0].value->value);
    }

    makeStringValue(repo, LINEFILE, CNEXT_NT);
    memFree(repo);
    return result->type;
}

void registeredObject(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_OBJECT];
    NameFunc tmp[] = {{inter->data.mag_func[M_ADD], object_add, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SUB], object_sub, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MUL], object_mul, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DIV], object_div, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_INTDIV], object_intdiv, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOD], object_mod, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_POW], object_pow, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_EQ], object_eq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_NOTEQ], object_noteq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOREEQ], object_moreeq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESSEQ], object_lesseq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MORE], object_more, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESS], object_less, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_BAND], object_band, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BOR], object_bor, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BXOR], object_bxor, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BL], object_bl, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BR], object_br, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_NEW],  object_new,  fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_REPO], objectRepoStrCore, fp_all, .var=nfv_notpush},
                      {inter->data.mag_func[M_STR],  objectRepoStrCore,  fp_all, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"object", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter, LinkValue *belong){
    LinkValue *g_belong;
    Value *object = makeClassValue(inter->var_list, inter, NULL);

    {
        Value *global_belong = makeObject(inter, copyVarList(inter->var_list, false, inter), NULL, true, NULL);
        g_belong = makeLinkValue(global_belong, belong, auto_aut, inter);
        gc_freeTmpLink(&global_belong->gc_status);
        inter->base_belong = g_belong;
        gc_addStatementLink(&inter->base_belong->gc_status);
    }

    inter->data.base_obj[B_OBJECT] = makeLinkValue(object, g_belong, auto_aut, inter);
    gc_freeTmpLink(&object->gc_status);
    gc_addStatementLink(&inter->data.base_obj[B_OBJECT]->gc_status);
    for (Inherit *ih=g_belong->value->object.inherit; ih != NULL; ih = ih->next) {
        if (ih->value->value == object)
            ih->value->belong = g_belong;
    }


    {
        Result result;
        Argument *arg = makeValueArgument(makeLinkValue(object, g_belong, auto_aut, inter));
        setResultCore(&result);
        object_new(CO_FUNC(arg, inter->var_list, &result, g_belong));

        result.value->value->type = V_none;
        inter->data.base_obj[B_NONE] = result.value;
        gc_addStatementLink(&inter->data.base_obj[B_NONE]->gc_status);

        freeArgument(arg, true);
        freeResult(&result);
    }
}
