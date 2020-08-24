#include "__ofunc.h"

void registeredFunction(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.function, inter->base_father, inter);
//    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("function", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
//    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFunction(Inter *inter){
    Value *function = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&function->gc_status);
    inter->data.function = function;
}
