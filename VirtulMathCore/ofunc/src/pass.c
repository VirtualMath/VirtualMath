#include "__ofunc.h"

ResultType pass_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "pass.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }
    value = make_new(inter, belong, ap[0].value);
    value->value->type = pass_;
    switch (init_new(value, arg, "pass.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}

void registeredEllipisis(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.pass_;
    NameFunc tmp[] = {{inter->data.object_new, pass_new, class_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"ellipsis", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseEllipisis(Inter *inter){
    LinkValue *pass_ = makeBaseChildClass4(inter->data.vobject, inter);
    gc_addStatementLink(&pass_->gc_status);
    inter->data.pass_ = pass_;
}
