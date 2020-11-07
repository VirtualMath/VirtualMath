#include "lib.h"

static ResultType vm_selfunCore(O_FUNC){
    time_t t;
    if (arg != NULL) {
        setResultError(E_ArgumentException, MANY_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    time(&t);
    makeIntValue(t, LINEFILE, CNEXT_NT);
    return result->type;
}

void registeredTimeLib(R_FUNC){
    NameFunc tmp[] = {{L"time", vm_selfunCore, fp_no_, .var=nfv_notpush},
                      {NULL, NULL}};
    iterBaseNameFunc(tmp, belong, CFUNC_CORE(var_list));
}