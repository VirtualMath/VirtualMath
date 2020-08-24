#include "__ofunc.h"

ResultType pass_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    base->value->type = pass_;
    setResult(result, inter, belong);
    return result->type;
}

void registeredEllipisis(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.pass_, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_init, pass_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("ellipsis", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseEllipisis(Inter *inter){
    Value *pass_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&pass_->gc_status);
    inter->data.pass_ = pass_;
}
