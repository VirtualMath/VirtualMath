#include "__ofunc.h"

void registeredEllipisis(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.pass_, inter->base_father, inter);
    gc_addTmpLink(&object->gc_status);
    addStrVar("ellipsis", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseEllipisis(Inter *inter){
    Value *pass_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&pass_->gc_status);
    inter->data.pass_ = pass_;
}
