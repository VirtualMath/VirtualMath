#include "__ofunc.h"

void registeredStr(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.str, inter->base_father, inter);
//    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("str", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
//    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseStr(Inter *inter){
    Value *str = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&str->gc_status);
    inter->data.str = str;
}
