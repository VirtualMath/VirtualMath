#include "__ofunc.h"

LinkValue *douCore(LinkValue *belong, LinkValue *class, Inter *inter) {
    LinkValue *value;
    value = make_new(inter, belong, class);
    value->value->type = V_dou;
    value->value->data.dou.num = 0.;
    return value;
}

static ResultType dou_new(O_FUNC){
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
    value->value->type = V_dou;
    value->value->data.dou.num = 0.;
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType dou_init(O_FUNC){
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
            base->value->data.dou.num = (vdou)ap[1].value->value->data.int_.num;
            break;
        case V_pointer:
            base->value->data.dou.num = (vdou)(vint)(ap[1].value->value->data.pointer.pointer);  // 需要两次转换
            break;
        case V_dou:
            base->value->data.dou.num = ap[1].value->value->data.dou.num;
            break;
        case V_str:
            base->value->data.dou.num = wcstold(ap[1].value->value->data.str.str, NULL);
            break;
        case V_bool:
            base->value->data.dou.num = ap[1].value->value->data.bool_.bool_;
            break;
        case V_none:
        case V_ell:
            base->value->data.dou.num = 0;
            break;
        case V_struct:
            if (ap[1].value->value->data.struct_.len * sizeof(int8_t) >= sizeof(vdou))
                base->value->data.dou.num = *(vdou *)ap[1].value->value->data.struct_.data;  // 转换为 vdou
            else
                setResultError(E_ValueException, NOT_ENOUGH_LEN(dou), LINEFILE, true, CNEXT_NT);  // 出现错误
            break;
        default:
            setResultError(E_TypeException, ERROR_INIT(num), LINEFILE, true, CNEXT_NT);
            return result->type;
    }

    return_:
    setResultBase(result, inter);
    return result->type;
}

void registeredDou(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_DOU];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW], dou_new, fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], dou_init, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"dou", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseDou(Inter *inter){
    LinkValue *dou = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&dou->gc_status);
    inter->data.base_obj[B_DOU] = dou;
}
