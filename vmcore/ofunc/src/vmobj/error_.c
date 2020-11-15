#include "__ofunc.h"

static LinkValue *makeException(LinkValue *father, Inter *inter){
    LinkValue *exc = makeBaseChildClass(father, inter);
    gc_addStatementLink(&exc->gc_status);
    return exc;
}

static ResultType base_exception_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"message", .must=0, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    if (addAttributes(inter->data.mag_func[M_MESSAGE], false, ap[1].value, LINEFILE, true, CFUNC_NT(var_list, result, ap[0].value)))
        setResult(result, inter);
    return result->type;
}

void registeredExcIter(R_FUNC){
    struct {
        wchar_t *name;
        LinkValue *value;
    } setList[] = {{L"Exception", inter->data.base_exc[E_Exception]},
                   {L"SystemException", inter->data.base_exc[E_SystemException]},
                   {L"KeyboardInterrupt", inter->data.base_exc[E_KeyInterrupt]},
                   {L"QuitException", inter->data.base_exc[E_QuitException]},
                   {L"TypeException", inter->data.base_exc[E_TypeException]},
                   {L"ValueException", inter->data.base_exc[E_ValueException]},
                   {L"ArgumentException", inter->data.base_exc[E_ArgumentException]},
                   {L"PermissionsException", inter->data.base_exc[E_PermissionsException]},
                   {L"ResultException", inter->data.base_exc[E_ResultException]},
                   {L"GotoException", inter->data.base_exc[E_GotoException]},
                   {L"NameException", inter->data.base_exc[E_NameExceptiom]},
                   {L"AssertException", inter->data.base_exc[E_AssertException]},
                   {L"IndexException", inter->data.base_exc[E_IndexException]},
                   {L"KeyException", inter->data.base_exc[E_KeyException]},
                   {L"StrideException", inter->data.base_exc[E_StrideException]},
                   {L"IncludeException", inter->data.base_exc[E_IndexException]},
                   {L"ImportException", inter->data.base_exc[E_ImportException]},
                   {L"IterStopException", inter->data.base_exc[E_StopIterException]},
                   {L"SuperException", inter->data.base_exc[E_SuperException]},
                   {NULL, NULL}};
    {
        LinkValue *object = inter->data.base_exc[E_BaseException];
        NameFunc tmp[] = {{L"__init__", base_exception_init, fp_obj, .var=nfv_notpush},
                          {NULL, NULL}};
        gc_addTmpLink(&object->gc_status);
        addBaseClassVar(L"BaseException", object, belong, inter);
        iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
        gc_freeTmpLink(&object->gc_status);
    }
    for (int i=0; setList[i].name != NULL; i++)
        addBaseClassVar(setList[i].name, setList[i].value, belong, inter);
}

void makeExcIter(Inter *inter){
    inter->data.base_exc[E_BaseException] = makeException(inter->data.base_obj[B_OBJECT], inter);

    inter->data.base_exc[E_SystemException] = makeException(inter->data.base_exc[E_BaseException], inter);
    inter->data.base_exc[E_Exception] = makeException(inter->data.base_exc[E_BaseException], inter);

    inter->data.base_exc[E_KeyInterrupt] = makeException(inter->data.base_exc[E_SystemException], inter);
    inter->data.base_exc[E_QuitException] = makeException(inter->data.base_exc[E_SystemException], inter);

    inter->data.base_exc[E_TypeException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_ValueException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_ArgumentException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_PermissionsException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_ResultException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_GotoException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_NameExceptiom] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_AssertException] = makeException(inter->data.base_exc[E_Exception], inter);

    inter->data.base_exc[E_KeyException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_IndexException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_StrideException] = makeException(inter->data.base_exc[E_Exception], inter);

    inter->data.base_exc[E_StopIterException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_SuperException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_ImportException] = makeException(inter->data.base_exc[E_Exception], inter);
    inter->data.base_exc[E_IncludeException] = makeException(inter->data.base_exc[E_Exception], inter);
}
