#include "__run.h"

static Registered base_func_list[] = {registeredIOFunction,
                                      registeredSysFunction,
                                      registeredObject,
                                      registeredVObject,
                                      registeredNum,
                                      registeredStr,
                                      registeredBool,
                                      registeredEllipisis,
                                      registeredFunction,
                                      registeredDict,
                                      registeredList,
                                      registeredListIter,
                                      registeredDictIter,
                                      NULL};

void registeredBaseFunction(struct LinkValue *father, Inter *inter){
    for (Registered *list = base_func_list; *list != NULL; list++)
        (*list)(CALL_REGISTERED_FUNCTION(father, inter->var_list));
}

void registeredFunctionName(Inter *inter){
    makeBaseObject(inter);
    makeBaseVObject(inter);
    makeBaseNum(inter);
    makeBaseStr(inter);
    makeBaseBool(inter);
    makeBaseEllipisis(inter);
    makeBaseFunction(inter);
    makeBaseDict(inter);
    makeBaseList(inter);
    makeBaseListIter(inter);
    makeBaseDictIter(inter);
}
