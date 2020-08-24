#include "__ofunc.h"

ResultType num_init(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="num", .must=1, .long_arg=false},
                           {.must=-1}};
    LinkValue *base = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    base->value->type = number;
    switch (ap[1].value->value->type){
        case number:
            base->value->data.num.num = ap[1].value->value->data.num.num;
            break;
        case string:
            base->value->data.num.num = strtoll(ap[1].value->value->data.str.str, NULL, 10);
            break;
        case bool_:
            base->value->data.num.num = ap[1].value->value->data.bool_.bool_;
            break;
        case none:
        case pass_:
            base->value->data.num.num = 0;
            break;
        default:
            setResultError(E_TypeException, "Get A Not Support Type For Int", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return result->type;
    }
    setResultBase(result, inter, belong);
    return result->type;
}

void registeredNum(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.num, inter->base_father, inter);
    NameFunc tmp[] = {{"__init__", num_init, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("num", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseNum(Inter *inter){
    Value *num = makeBaseChildClass(inter->data.vobject, inter);
    gc_addStatementLink(&num->gc_status);
    inter->data.num = num;
}
