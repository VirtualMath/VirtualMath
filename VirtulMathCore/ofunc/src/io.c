#include "__ofunc.h"

ResultType vm_print(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=true},
                           {.type=name_value, .name=L"end", .must=0, .value=NULL},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    arg = ap[0].arg;
    for (int i=0; i < ap[0].c_count; arg = arg->next,i++){
        freeResult(result);
        wchar_t *tmp = getRepoStr(arg->data.value, true, 0, "sys", CFUNC_NT(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        if (i != 0)
            fprintf(inter->data.inter_stdout, " ");
        fprintf(inter->data.inter_stdout, "%ls", tmp);
    }

    if (ap[1].value != NULL && ap[1].value->value->type == V_str)
        fprintf(inter->data.inter_stdout, "%ls", ap[1].value->value->data.str.str);
    else
        fprintf(inter->data.inter_stdout, "\n");

    setResult(result, inter);
    return result->type;
}

ResultType vm_input(O_FUNC){
    setResultCore(result);
    ArgumentParser ap[] = {{.type=name_value, .name=L"message", .must=0, .value=NULL},
                           {.must=-1}};
    wchar_t *str = memWidecpy(L"\0");
    wint_t ch;
    parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value != NULL)
        printValue(ap[0].value->value, inter->data.inter_stdout, false, true);

    while ((ch = fgetwc(inter->data.inter_stdin)) != '\n' && ch != WEOF)
        str = memWideCharcpy(str, 1, true, true, ch);

    makeStringValue(str, 0, "sys", CFUNC_NT(var_list, result, belong));
    memFree(str);
    return result->type;
}

void registeredIOFunction(R_FUNC){
    NameFunc tmp[] = {{L"print", vm_print, free_},
                      {L"input", vm_input, free_},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}
