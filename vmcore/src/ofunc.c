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

void registeredBaseFunction(LinkValue *belong, Inter *inter){
    for (Registered *list = base_func_list; *list != NULL; list++)
        (*list)(CR_FUNC(belong, inter->var_list));
}

static void registeredStdFile(FILE *std, char *path, char *mode, wchar_t *name, LinkValue *belong, Inter *inter) {
    Result result;
    setResultCore(&result);

    makeFileValue(std, mode, true, path, LINEFILE, CFUNC_NT(inter->var_list, &result, belong));
    if (RUN_TYPE(result.type)) {
        LinkValue *new = result.value;
        result.value = NULL;
        freeResult(&result);
        addStrVar(name, false, false, new, LINEFILE, false, CFUNC_NT(inter->var_list, &result, belong));
        gc_freeTmpLink(&new->gc_status);
        if (!RUN_TYPE(result.type))
            goto error;
        freeResult(&result);
    } else {
        error: printError(&result, inter, true);
    }
}

static void registeredBaseVar(LinkValue *belong, Inter *inter){
    registeredStdFile(stdin, "stdin", "r", L"stdin", belong, inter);
    registeredStdFile(stdout, "stdout", "w", L"stdout", belong, inter);
    registeredStdFile(stderr, "stderr", "w", L"stderr", belong, inter);
}

void registeredFunctionName(LinkValue *belong, Inter *inter) {
    makeBaseObject(inter, belong);  // 在makeBaseObject之前base_belong还是NULL, object要关联于belong参数给定的值(用于import的时候)

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
    functionPresetting(inter->data.base_obj[B_FUNCTION], inter);  // 预设定function的__new__方法

    registeredObject(inter->base_belong, CFUNC_CORE(inter->var_list));
    registeredBaseFunction(inter->base_belong, inter);
    registeredBaseVar(inter->base_belong, inter);
}
