#include "__ofunc.h"

ResultType vm_print(OfficialFunctionSig){
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=true},
                           {.type=name_value, .name="end", .must=0, .value=NULL},
                           {.must=-1}};
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!run_continue(result))
        return result->type;
    freeResult(result);

    arg = ap[0].arg;
    for (int i=0; i < ap[0].c_count; arg = arg->next,i++)
#if 0
        printLinkValue(arg->data.value, "", "", stdout);
#else
        printValue(arg->data.value->value, stdout, true);
#endif

    if (ap[1].value != NULL && ap[1].value->value->type == string)
        printf("%s", ap[1].value->value->data.str.str);
    else
        printf("\n");

    setResultBase(result, inter, belong);
    return result->type;
}

void registeredIOFunction(RegisteredFunctionSig){
    NameFunc tmp[] = {{"print", vm_print, free_}, {NULL, NULL}};
    iterNameFunc(tmp, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
