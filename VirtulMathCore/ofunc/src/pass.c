#include "__ofunc.h"

ResultType pass_new(O_FUNC){
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
    value->value->type = V_ell;

    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

void registeredEllipisis(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_PASS];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW], pass_new, class_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"ellipsis", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseEllipisis(Inter *inter){
    LinkValue *pass_ = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&pass_->gc_status);
    inter->data.base_obj[B_PASS] = pass_;
}
