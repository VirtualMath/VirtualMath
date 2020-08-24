#include "__ofunc.h"

ResultType bool_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    bool new;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    new = checkBool(ap[1].value, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    base->value->type = bool_;
    base->value->data.bool_.bool_ = new;
    setResult(result, inter, belong);
    return result->type;
}

void registeredBool(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.bool_, inter->base_father, inter);
    NameFunc tmp[] = {{"__init__", bool_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("bool", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseBool(Inter *inter){
    Value *bool_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.bool_ = bool_;
}
