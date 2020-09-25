#include "__ofunc.h"

ResultType bool_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "bool.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return R_error;
    }

    setResultCore(result);
    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_bool;
    value->value->data.bool_.bool_ = false;
    switch (init_new(value, arg, "bool.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
    return result->type;
}

ResultType bool_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    bool new = false;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if (ap[1].value != NULL) {
        new = checkBool(ap[1].value, 0, "bool.init", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    base->value->data.bool_.bool_ = new;
    setResultBase(result, inter);
    return result->type;
}

void registeredBool(REGISTERED_FUNCTIONSIG){
    LinkValue *object = inter->data.bool_;
    NameFunc tmp[] = {{inter->data.object_new, bool_new, class_free_},
                      {inter->data.object_init, bool_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"bool", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseBool(Inter *inter){
    LinkValue *bool_ = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.bool_ = bool_;
}
