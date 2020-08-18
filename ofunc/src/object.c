#include "__ofunc.h"

ResultType object_new_(OfficialFunctionSig){
    setResultBase(result, inter, father);
    printf("object.__new__\n");
    return result->type;
}

void registeredObject(RegisteredFunctionSig){
    Value *object = inter->data.object;
    VarList *object_var = object->object.var;
    NameFunc tmp[] = {{"__new__", object_new_}, {NULL, NULL}};
    LinkValue *name_ = NULL;
    char *name = NULL;
    gc_addTmpLink(&object->gc_status);

    object_var->next = inter->var_list;
    iterNameFunc(tmp, father, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = NULL;

    name = setStrVarName("object", false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    name_ = makeLinkValue(makeStringValue(name, inter), father, inter);
    addFromVarList(name, name_, 0, makeLinkValue(object, father, inter), CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    memFree(name);
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter){
    Value *object = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&object->gc_status);
    inter->data.object = object;
}
