#include "__run.h"

static Registered base_func_list[] = {registeredVObject,
                                      registeredInt,
                                      registeredDou,
                                      registeredStr,
                                      registeredBool,
                                      registeredEllipisis,
                                      registeredFunction,
                                      registeredDict,
                                      registeredList,
                                      registeredListIter,
                                      registeredDictIter,
                                      registeredExcIter,
                                      registeredFile,
                                      registeredLib,
                                      registeredPointer,

                                      registeredSysFunction,
                                      registeredIOFunction,
                                      NULL};

void registeredBaseFunction(struct LinkValue *father, Inter *inter){
    for (Registered *list = base_func_list; *list != NULL; list++)
        (*list)(CR_FUNC(father, inter->var_list));
}

void presetting(Inter *inter) {
    LinkValue *func = inter->data.base_obj[B_FUNCTION];
    LinkValue *func_new = NULL;
    LinkValue *func_init = NULL;

    functionPresetting(func, &func_new, &func_init, inter);
    strFunctionPresetting(func, func_new, func_init, inter);

    functionPresettingLast(func, func_new, func_init, inter);
}

void registeredFunctionName(Inter *inter, LinkValue *belong){
    makeBaseObject(inter, belong);

    makeBaseVObject(inter);

    makeBaseInt(inter);
    makeBaseDou(inter);
    makeBaseBool(inter);
    makeBaseEllipisis(inter);
    makeBaseFunction(inter);
    makeBaseDict(inter);
    makeBaseList(inter);
    makeBaseListIter(inter);
    makeBaseDictIter(inter);
    makeExcIter(inter);
    makeBaseFile(inter);
    makeBaseLib(inter);
    makeBasePointer(inter);

    makeBaseStr(inter);
    presetting(inter);

    registeredObject(inter->base_belong, CFUNC_CORE(inter->var_list));
    registeredBaseFunction(inter->base_belong, inter);
}
