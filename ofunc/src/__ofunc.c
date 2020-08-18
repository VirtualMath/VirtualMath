#include "__ofunc.h"

LinkValue *registeredFunctionCore(OfficialFunction of, char *name, struct LinkValue *father, INTER_FUNCTIONSIG_CORE) {
    LinkValue *value = NULL;
    LinkValue *name_ = NULL;
    char *var_name = setStrVarName(name, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    name_ = makeLinkValue(makeStringValue(var_name, inter), father, inter);
    value = makeLinkValue(makeCFunctionValue(of, var_list, inter), father, inter);
    addFromVarList(var_name, name_, 0, value, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    memFree(var_name);
    return value;
}

void iterNameFunc(NameFunc list[], LinkValue *father, INTER_FUNCTIONSIG_CORE){
    for (PASS; list->of != NULL; list++)
        registeredFunctionCore(list->of, list->name, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
