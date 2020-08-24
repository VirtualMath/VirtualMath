#include "__ofunc.h"

static Value *makeException(Value *father, Inter *inter){
    Value *exc = makeBaseChildClass(father, inter);
    gc_addStatementLink(&exc->gc_status);
    return exc;
}

static void addException(char *name, Value *exc, LinkValue *belong, Inter *inter){
    addStrVar(name, false, true, makeLinkValue(exc, inter->base_father, inter),
              belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
}

ResultType base_exception_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="message", .must=0, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    addAttributes(inter->data.object_message, false, ap[1].value, ap[0].value, inter);
    setResultBase(result, inter, belong);
    return result->type;
}

void registeredExcIter(REGISTERED_FUNCTIONSIG){
    struct {
        char *name;
        Value *value;
    } setList[] = {{"Exception", inter->data.exc},
                   {"TypeException", inter->data.type_exc},
                   {"ArgumentException", inter->data.arg_exc},
                   {"PermissionsException", inter->data.per_exc},
                   {"ResultException", inter->data.result_exc},
                   {"GotoException", inter->data.goto_exc},
                   {"NameException", inter->data.name_exc},
                   {"AssertException", inter->data.assert_exc},
                   {"IndexException", inter->data.index_exc},
                   {"KeyException", inter->data.key_exc},
                   {"StrideException", inter->data.stride_exc},
                   {"IncludeException", inter->data.include_exp},
                   {"ImportException", inter->data.import_exc},
                   {"IterStopException", inter->data.iterstop_exc},
                   {"SuperException", inter->data.super_exc},
                   {NULL, NULL}};
    {
        LinkValue *object = makeLinkValue(inter->data.base_exc, inter->base_father, inter);
        NameFunc tmp[] = {{"__init__", base_exception_init, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addStrVar("BaseException", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
    for (int i=0; setList[i].name != NULL; i++)
        addException(setList[i].name, setList[i].value, belong, inter);
}

void makeExcIter(Inter *inter){
    inter->data.base_exc = makeException(inter->data.object, inter);
    inter->data.exc = makeException(inter->data.base_exc, inter);

    inter->data.type_exc = makeException(inter->data.exc, inter);
    inter->data.arg_exc = makeException(inter->data.exc, inter);
    inter->data.per_exc = makeException(inter->data.exc, inter);
    inter->data.result_exc = makeException(inter->data.exc, inter);
    inter->data.goto_exc = makeException(inter->data.exc, inter);
    inter->data.name_exc = makeException(inter->data.exc, inter);
    inter->data.assert_exc = makeException(inter->data.exc, inter);

    inter->data.key_exc = makeException(inter->data.exc, inter);
    inter->data.index_exc = makeException(inter->data.exc, inter);
    inter->data.stride_exc = makeException(inter->data.exc, inter);

    inter->data.iterstop_exc = makeException(inter->data.exc, inter);
    inter->data.super_exc = makeException(inter->data.exc, inter);
    inter->data.import_exc = makeException(inter->data.exc, inter);
    inter->data.include_exp = makeException(inter->data.exc, inter);
}
