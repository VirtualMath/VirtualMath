#include "__ofunc.h"
ResultType vm_printCore(OfficialFunctionSig, int type);

ResultType vm_print(OfficialFunctionSig){
    return vm_printCore(CALL_OfficialFunction(arg, var_list, result, belong), 2);
}

ResultType vm_printAll(OfficialFunctionSig){
    return vm_printCore(CALL_OfficialFunction(arg, var_list, result, belong), 1);
}

ResultType vm_printLink(OfficialFunctionSig){
    return vm_printCore(CALL_OfficialFunction(arg, var_list, result, belong), 0);
}

ResultType vm_printCore(OfficialFunctionSig, int type){
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=true},
                           {.type=name_value, .name="end", .must=0, .value=NULL},
                           {.must=-1}};
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!run_continue(result))
        return result->type;
    freeResult(result);

    arg = ap[0].arg;
    for (int i=0; i < ap[0].c_count; arg = arg->next,i++){
        if (i != 0)
            fprintf(inter->data.inter_stdout, " ");
        if (type == 0)
            printLinkValue(arg->data.value, "", "", inter->data.inter_stdout);
        else if (type == 1)
            printValue(arg->data.value->value, inter->data.inter_stdout, true);
        else
            printValue(arg->data.value->value, inter->data.inter_stdout, false);
    }

    if (ap[1].value != NULL && ap[1].value->value->type == string)
        fprintf(inter->data.inter_stdout, "%s", ap[1].value->value->data.str.str);
    else
        fprintf(inter->data.inter_stdout, "\n");

    setResultBase(result, inter, belong);
    return result->type;
}

void registeredIOFunction(RegisteredFunctionSig){
    NameFunc tmp[] = {{"print", vm_print, free_},
                      {"print_link", vm_printLink, free_},
                      {"print_all", vm_printAll, free_},
                      {NULL, NULL}};
    iterNameFunc(tmp, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
