#include "__ofunc.h"

void registeredBool(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.bool_, inter->base_father, inter);
//    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("bool", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
//    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseBool(Inter *inter){
    Value *bool_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.bool_ = bool_;
}
