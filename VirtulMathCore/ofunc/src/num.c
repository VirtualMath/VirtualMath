#include "__ofunc.h"

ResultType num_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "num", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    setResultCore(result);
    value = make_new(inter, belong, ap[0].value);
    value->value->type = number;
    value->value->data.num.num = 0;
    switch (init_new(value, arg, "num", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}

ResultType num_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="num", .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value == NULL)
        goto return_;
    switch (ap[1].value->value->type){
        case number:
            base->value->data.num.num = ap[1].value->value->data.num.num;
            break;
        case string:
            base->value->data.num.num = wcstoll(ap[1].value->value->data.str.str, NULL, 10);
            break;
        case bool_:
            base->value->data.num.num = ap[1].value->value->data.bool_.bool_;
            break;
        case none:
        case pass_:
            base->value->data.num.num = 0;
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), 0, "num", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

void registeredNum(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.num;
    NameFunc tmp[] = {{inter->data.object_new, num_new, class_free_},
                      {inter->data.object_init, num_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar("num", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseNum(Inter *inter){
    LinkValue *num = makeBaseChildClass4(inter->data.vobject, inter);
    gc_addStatementLink(&num->gc_status);
    inter->data.num = num;
}
