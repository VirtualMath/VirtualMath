#include "__ofunc.h"

ResultType num_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "num", true, CFUNC_NT(var_list, result, belong));
        return R_error;
    }

    setResultCore(result);
    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_num;
    value->value->data.num.num = 0;
    run_init(value, arg, 0, "num.new", CFUNC_NT(var_list, result, belong));
    return result->type;
}

ResultType num_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"num", .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CFUNC_NT(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value == NULL)
        goto return_;
    switch (ap[1].value->value->type){
        case V_num:
            base->value->data.num.num = ap[1].value->value->data.num.num;
            break;
        case V_str:
            base->value->data.num.num = wcstoll(ap[1].value->value->data.str.str, NULL, 10);
            break;
        case V_bool:
            base->value->data.num.num = ap[1].value->value->data.bool_.bool_;
            break;
        case V_none:
        case V_ell:
            base->value->data.num.num = 0;
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), 0, "num", true, CFUNC_NT(var_list, result, belong));
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

void registeredNum(R_FUNC){
    LinkValue *object = inter->data.num;
    NameFunc tmp[] = {{inter->data.object_new, num_new, class_free_},
                      {inter->data.object_init, num_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"num", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseNum(Inter *inter){
    LinkValue *num = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&num->gc_status);
    inter->data.num = num;
}
