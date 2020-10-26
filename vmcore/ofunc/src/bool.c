#include "__ofunc.h"

LinkValue *boolCore(LinkValue *belong, LinkValue *class, Inter *inter) {
    LinkValue *value;
    value = make_new(inter, belong, class);
    value->value->type = V_bool;
    value->value->data.bool_.bool_ = false;
    return value;
}

ResultType bool_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_bool;
    value->value->data.bool_.bool_ = false;
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType bool_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    bool new = false;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value != NULL) {
        new = checkBool(ap[1].value, LINEFILE, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    base->value->data.bool_.bool_ = new;
    setResultBase(result, inter);
    return result->type;
}

void registeredBool(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_BOOL];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW], bool_new,   fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], bool_init, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"bool", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseBool(Inter *inter){
    LinkValue *bool_ = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.base_obj[B_BOOL] = bool_;
}
