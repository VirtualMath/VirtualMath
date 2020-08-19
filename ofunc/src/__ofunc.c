#include "__ofunc.h"

LinkValue *registeredFunctionCore(OfficialFunction of, char *name, LinkValue *father, INTER_FUNCTIONSIG_CORE) {
    LinkValue *value = makeLinkValue(makeCFunctionValue(of, var_list, inter), father, inter);
    addStrVar(name, false, value, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return value;
}

void iterNameFunc(NameFunc list[], LinkValue *father, INTER_FUNCTIONSIG_CORE){
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        value->value->data.function.function_data.pt_type = list->type;
    }
}

Value *makeBaseChildClass(Value *father, Inter *inter) {
    FatherValue *father_value = NULL;
    Value *num = NULL;
    {
        LinkValue *father_ = makeLinkValue(father, inter->base_father, inter);
        Argument *arg = makeValueArgument(father_);
        gc_addTmpLink(&father_->gc_status);
        father_value = setFather(arg);
        freeArgument(arg, true);
        gc_freeTmpLink(&father_->gc_status);
    }
    num = makeClassValue(copyVarList(inter->var_list, false, inter), inter, father_value);
    return num;
}
