#include "__ofunc.h"

ResultType pointer_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    setResultCore(result);
    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_pointer;
    value->value->data.pointer.pointer = NULL;
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType pointer_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"p", .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value == NULL)
        goto return_;
    switch (ap[1].value->value->type){
        case V_int:
            base->value->data.pointer.pointer = (void *)ap[1].value->value->data.int_.num;
            break;
        case V_none:
        case V_ell:
            base->value->data.pointer.pointer = NULL;
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), LINEFILE, true, CNEXT_NT);
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

void registeredPointer(R_FUNC){
    LinkValue *object = inter->data.pointer;
    NameFunc tmp[] = {{inter->data.object_new, pointer_new, class_free_},
                      {inter->data.object_init, pointer_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"pointer", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBasePointer(Inter *inter){
    LinkValue *pointer = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&pointer->gc_status);
    inter->data.pointer = pointer;
}
