#include "__ofunc.h"


static LinkValue *makeException(LinkValue *father, Inter *inter){
    LinkValue *exc = makeBaseChildClass(father, inter);
    gc_addStatementLink(&exc->gc_status);
    return exc;
}

ResultType base_exception_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"message", .must=0, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (addAttributes(inter->data.object_message, false, ap[1].value, 0, "BaseException.init", ap[0].value, result,
                      inter, var_list))
        setResult(result, inter);
    return result->type;
}

void registeredExcIter(REGISTERED_FUNCTIONSIG){
    struct {
        wchar_t *name;
        LinkValue *value;
    } setList[] = {{L"Exception", inter->data.exc},
                   {L"SystemException", inter->data.sys_exc},
                   {L"KeyboardInterrupt", inter->data.keyInterrupt_exc},
                   {L"QuitException", inter->data.quit_exc},
                   {L"TypeException", inter->data.type_exc},
                   {L"ArgumentException", inter->data.arg_exc},
                   {L"PermissionsException", inter->data.per_exc},
                   {L"ResultException", inter->data.result_exc},
                   {L"GotoException", inter->data.goto_exc},
                   {L"NameException", inter->data.name_exc},
                   {L"AssertException", inter->data.assert_exc},
                   {L"IndexException", inter->data.index_exc},
                   {L"KeyException", inter->data.key_exc},
                   {L"StrideException", inter->data.stride_exc},
                   {L"IncludeException", inter->data.include_exp},
                   {L"ImportException", inter->data.import_exc},
                   {L"IterStopException", inter->data.iterstop_exc},
                   {L"SuperException", inter->data.super_exc},
                   {NULL, NULL}};
    {
        LinkValue *object = inter->data.base_exc;
        NameFunc tmp[] = {{L"__init__", base_exception_init, object_free_},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar(L"BaseException", object, belong, inter);
        iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
    for (int i=0; setList[i].name != NULL; i++)
        addBaseClassVar(setList[i].name, setList[i].value, belong, inter);
}

void makeExcIter(Inter *inter){
    inter->data.base_exc = makeException(inter->data.object, inter);
    inter->data.sys_exc = makeException(inter->data.base_exc, inter);
    inter->data.exc = makeException(inter->data.base_exc, inter);

    inter->data.keyInterrupt_exc = makeException(inter->data.sys_exc, inter);
    inter->data.quit_exc = makeException(inter->data.sys_exc, inter);

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
