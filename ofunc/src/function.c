#include "__ofunc.h"

void registeredFunction(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.function, inter->base_father, inter);
//    VarList *object_var = object->value->object.var;
//    VarList *object_backup = NULL;
//    NameFunc tmp[] = {{NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("function", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

//    object_backup = object_var->next;
//    object_var->next = inter->var_list;
//    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
//    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFunction(Inter *inter){
    Value *function = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&function->gc_status);
    inter->data.function = function;
}
