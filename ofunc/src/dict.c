#include "__ofunc.h"

void registeredDict(RegisteredFunctionSig){
    LinkValue *object = makeLinkValue(inter->data.dict, inter->base_father, inter);
//    VarList *object_var = object->value->object.var;
//    VarList *object_backup = NULL;
//    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("dict", false, object, father, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

//    object_backup = object_var->next;
//    object_var->next = inter->var_list;
//    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
//    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDict(Inter *inter){
    Value *dict = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&dict->gc_status);
    inter->data.dict = dict;
}
