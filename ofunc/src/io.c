#include "__ofunc.h"

ResultType vm_print(OfficialFunctionSig){
    setResultBase(result, inter, father);
    if (arg == NULL || arg->next == NULL){
        setResultError(result, inter, "ArgumentException", "Don't get any Argument for print", 0, "sys", father, true);
        return error_return;
    }
    for (arg = arg->next->next; arg != NULL; arg = arg->next)
        printValue(arg->data.value->value, stdout, true);
    printf("\n");
    return result->type;
}

void registeredIOFunction(RegisteredFunctionSig){
    NameFunc tmp[] = {{"print", vm_print}, {NULL, NULL}};
    iterNameFunc(tmp, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
