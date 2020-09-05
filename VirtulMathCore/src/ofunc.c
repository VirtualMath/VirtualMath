#include "__run.h"

static Registered base_func_list[] = {registeredVObject,
                                      registeredNum,
                                      registeredStr,
                                      registeredBool,
                                      registeredEllipisis,
                                      registeredFunction,
                                      registeredDict,
                                      registeredList,
                                      registeredListIter,
                                      registeredDictIter,
                                      registeredExcIter,

                                      registeredSysFunction,
                                      registeredIOFunction,
                                      NULL};

void registeredBaseFunction(struct LinkValue *father, Inter *inter){
    for (Registered *list = base_func_list; *list != NULL; list++)
        (*list)(CALL_REGISTERED_FUNCTION(father, inter->var_list));
}

void presetting(Inter *inter) {
    LinkValue *func = makeLinkValue(inter->data.function, inter->base_father, inter);
    LinkValue *func_new = NULL;
    LinkValue *func_init = NULL;

    functionPresetting(func, &func_new, &func_init, inter);
    strFunctionPresetting(func, func_new, func_init, inter);
    functionPresettingLast(func, func_new, func_init, inter);
}

void registeredFunctionName(Inter *inter, LinkValue *belong){
    makeBaseObject(inter);
    makeBaseVObject(inter);
    makeBaseNum(inter);
    makeBaseBool(inter);
    makeBaseEllipisis(inter);
    makeBaseFunction(inter);
    makeBaseDict(inter);
    makeBaseList(inter);
    makeBaseListIter(inter);
    makeBaseDictIter(inter);
    makeExcIter(inter);

    {
        Value *global_belong = makeObject(inter, copyVarList(inter->var_list, false, inter), NULL, NULL);
        LinkValue *base_father = makeLinkValue(global_belong, belong, inter);
        gc_addStatementLink(&base_father->gc_status);
        inter->base_father = base_father;
    }

    makeBaseStr(inter);
    presetting(inter);
    registeredObject(inter->base_father, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    {
        Result result;
        setResultCore(&result);
        inter->data.none = makeNoneValue(0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(inter->var_list, &result, belong));
        if (!RUN_TYPE(result.type))
            printError(&result, inter, true);
        else
            gc_addStatementLink(&inter->data.none->gc_status);
        freeResult(&result);
    }
    registeredBaseFunction(inter->base_father, inter);
}
