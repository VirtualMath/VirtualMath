#include "__ofunc.h"

ResultType int_new(O_FUNC){
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
    value->value->type = V_int;
    value->value->data.int_.num = 0;
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType int_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"num", .must=0, .long_arg=false},
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
            base->value->data.int_.num = ap[1].value->value->data.int_.num;
            break;
        case V_dou:
            base->value->data.int_.num = (vint)ap[1].value->value->data.dou.num;
            break;
        case V_str:
            base->value->data.int_.num = wcstoll(ap[1].value->value->data.str.str, NULL, 10);
            break;
        case V_bool:
            base->value->data.int_.num = ap[1].value->value->data.bool_.bool_;
            break;
        case V_none:
        case V_ell:
            base->value->data.int_.num = 0;
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), LINEFILE, true, CNEXT_NT);
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

void registeredInt(R_FUNC){
    LinkValue *object = inter->data.int_;
    NameFunc tmp[] = {{inter->data.object_new,  int_new,  class_free_},
                      {inter->data.object_init, int_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"int", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseInt(Inter *inter){
    LinkValue *int_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&int_->gc_status);
    inter->data.int_ = int_;
}
