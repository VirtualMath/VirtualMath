#include "__ofunc.h"

ResultType bool_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    setResultCore(result);
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
    LinkValue *object = inter->data.bool_;
    NameFunc tmp[] = {{inter->data.object_new, bool_new, class_free_},
                      {inter->data.object_init, bool_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"bool", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseBool(Inter *inter){
    LinkValue *bool_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.bool_ = bool_;
}
