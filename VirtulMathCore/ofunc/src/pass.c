#include "__ofunc.h"

ResultType pass_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "pass.new", true, CFUNC_NT(var_list, result, belong));
        return R_error;
    }
    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_ell;
    run_init(value, arg, 0, "pass.new", CFUNC_NT(var_list, result, belong));
    return result->type;
}

void registeredEllipisis(R_FUNC){
    LinkValue *object = inter->data.pass_;
    NameFunc tmp[] = {{inter->data.object_new, pass_new, class_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"ellipsis", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseEllipisis(Inter *inter){
    LinkValue *pass_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&pass_->gc_status);
    inter->data.pass_ = pass_;
}
