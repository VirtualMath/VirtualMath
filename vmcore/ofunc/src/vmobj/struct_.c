#include "__ofunc.h"

LinkValue *structCore(LinkValue *belong, LinkValue *class, Inter *inter) {
    LinkValue *value;
    value = make_new(inter, belong, class);
    value->value->type = V_struct;
    value->value->data.struct_.data = NULL;
    value->value->data.struct_.len = 0;
    return value;
}

static ResultType struct_new(O_FUNC){
    LinkValue *value = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    setResultCore(result);
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    value = make_new(inter, belong, ap[0].value);
    value->value->type = V_struct;
    value->value->data.struct_.data = NULL;
    value->value->data.struct_.len = 0;
    run_init(value, arg, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType struct_init(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"data", .must=0, .long_arg=false},
                           {.must=-1}};
    LinkValue *base;
    LinkValue *data;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    base = ap[0].value;
    if ((data = ap[1].value) != NULL){
        memFree(base->value->data.struct_.data);
        base->value->data.struct_.len = 0;
        switch (data->value->type) {
            case V_int:
                MEM_CPY(base->value->data.struct_.data, &(data->value->data.int_.num), sizeof(vint));
                base->value->data.struct_.len = sizeof(vint) / sizeof(vstruct);
                break;
            case V_dou:
                MEM_CPY(base->value->data.struct_.data, &(data->value->data.dou.num), sizeof(vdou));
                base->value->data.struct_.len = sizeof(vdou) / sizeof(vstruct);
                break;
            case V_struct:
                MEM_CPY(base->value->data.struct_.data, &data->value->data.struct_.data, data->value->data.struct_.len * sizeof(vstruct));
                base->value->data.struct_.len = data->value->data.struct_.len;
                break;
            case V_str:
                MEM_CPY(base->value->data.struct_.data, &data->value->data.str.str, memWidelen(data->value->data.str.str) * sizeof(wchar_t));
                base->value->data.struct_.len = (sizeof(wchar_t) * memWidelen(data->value->data.str.str)) / sizeof(vstruct);
                break;
            default:
                setResultError(E_ArgumentException, ONLY_ACC(data, int/str/struct), LINEFILE, true, CNEXT_NT);
                break;
        }
    }
    setResult(result, inter);
    return result->type;
}

static ResultType struct_slice(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.type=only_value, .must=0, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint first;
    vint second;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_struct) {
        setResultError(E_TypeException, INSTANCE_ERROR(struct), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    size = ap[0].value->value->data.struct_.len;

    first = 0;
    second = size;
    for (vint *list[]={&first, &second}, i=0; i < 2; i++) {
        if (ap[i + 1].value != NULL && ap[i + 1].value->value->type == V_int)  // 检查是否存在或是否为数字
            *(list[i]) = ap[i + 1].value->value->data.int_.num;
        else if (ap[i + 1].value != NULL && ap[i + 1].value->value->type != V_none) {  // 若不是数字则报错
            setResultError(E_TypeException, VALUE_ERROR(first/second, num or null), LINEFILE, true, CNEXT_NT);
            return R_error;
        }
    }

    first = first < 0 ? first + size : first;
    second = second < 0 ? second + size : second;
    if (second > size || first >= size || first > size){
        setResultError(E_IndexException, L"index too max", LINEFILE, true, CNEXT_NT);
        return false;
    } else if (first < 0 || second < 0){  // second可以和first相等, 所以second可以为0
        setResultError(E_IndexException, L"index is less than 0", LINEFILE, true, CNEXT_NT);
        return false;
    }

    makeStructValue(ap[0].value->value->data.struct_.data + first, (second - first), LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType struct_down(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    vint size;
    vint index;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    if (ap[0].value->value->type != V_struct){
        setResultError(E_TypeException, INSTANCE_ERROR(struct), LINEFILE, true, CNEXT_NT);
        return R_error;
    }
    if (ap[1].value->value->type != V_int){
        setResultError(E_TypeException, ONLY_ACC(struct index, int), LINEFILE, true, CNEXT_NT);
        return R_error;
    }

    size = ap[0].value->value->data.struct_.len;
    index = ap[1].value->value->data.int_.num;
    if (!checkIndex(&index, &size, CNEXT_NT))
        return result->type;
    makeStructValue(ap[0].value->value->data.struct_.data + index, 1, LINEFILE, CNEXT_NT);
    return result->type;
}

void registeredStruct(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_BOOL];
    NameFunc tmp[] = {{inter->data.mag_func[M_NEW], struct_new, fp_class, .var=nfv_notpush},
                      {inter->data.mag_func[M_INIT], struct_init, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DOWN], struct_down, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SLICE], struct_slice, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"struct", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseStruct(Inter *inter){
    LinkValue *bool_ = makeBaseChildClass(inter->data.base_obj[B_VOBJECT], inter);
    gc_addStatementLink(&bool_->gc_status);
    inter->data.base_obj[B_STRUCT] = bool_;
}
