#include "__ofunc.h"

ResultType function_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    LinkValue *func_return;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    func_return = ap[1].value;
    if (func_return == NULL)
        func_return = makeLinkValue(makeNoneValue(inter), belong, inter);
    makeVMFunctionFromValue(base->value, func_return, 0, "sys", inter);
    setResult(result, inter, belong);
    return result->type;
}

void registeredFunction(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.function, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_init, function_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("function", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseFunction(Inter *inter){
    Value *function = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&function->gc_status);
    inter->data.function = function;
}
