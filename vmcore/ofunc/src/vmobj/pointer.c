#include "__ofunc.h"

static ResultType pointer_new(O_FUNC){
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

static ResultType pointer_init(O_FUNC){
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
        case V_none:
        case V_ell:
            base->value->data.pointer.pointer = NULL;
            break;
        case V_int:
            base->value->data.pointer.pointer = &ap[1].value->value->data.int_.num;
            break;
        case V_dou:
            base->value->data.pointer.pointer = &ap[1].value->value->data.dou.num;
            break;
        case V_bool:
            base->value->data.pointer.pointer = &ap[1].value->value->data.bool_.bool_;
            break;
        case V_str:
            base->value->data.pointer.pointer = &ap[1].value->value->data.str.str;
            break;
        case V_file:
            base->value->data.pointer.pointer = &ap[1].value->value->data.file.file;
            break;
        case V_lib:
            base->value->data.pointer.pointer = &ap[1].value->value->data.lib.handle;
            break;
        case V_dict:
            base->value->data.pointer.pointer = &ap[1].value->value->data.dict.dict;
            break;
        case V_list:
            base->value->data.pointer.pointer = &ap[1].value->value->data.list.list;
            break;
        case V_struct:
            base->value->data.pointer.pointer = &ap[1].value->value->data.struct_.data;
            break;
        case V_pointer:
            base->value->data.pointer.pointer = &ap[1].value->value->data.pointer.pointer;
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), LINEFILE, true, CNEXT_NT);
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

static ResultType pointer_set(O_FUNC){  // 设置指针的内容
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
        case V_none:
        case V_ell:
            base->value->data.pointer.pointer = NULL;
            break;
        case V_int:
            base->value->data.pointer.pointer = (void *)ap[1].value->value->data.int_.num;
            break;
        case V_bool:
            base->value->data.pointer.pointer = ap[1].value->value->data.bool_.bool_ ? (void *)1 : NULL;
            break;
        case V_pointer:
            base->value->data.pointer.pointer = ap[1].value->value->data.pointer.pointer;
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
    LinkValue *object = inter->data.base_obj[B_POINTER];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW], pointer_new, fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], pointer_init, fp_obj, .var=nfv_notpush},
                      {L"set", pointer_set, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"pointer", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBasePointer(Inter *inter){
    LinkValue *pointer = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&pointer->gc_status);
    inter->data.base_obj[B_POINTER] = pointer;
}
