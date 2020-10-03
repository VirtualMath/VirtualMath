#include "__ofunc.h"

typedef void (*base_opt)(LinkValue *, Result *, struct Inter *, VarList *var_list, Value *, Value *);

void vobject_add_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeIntValue(left->data.int_.num + right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeDouValue(left->data.int_.num + right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num + right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num + right->data.dou.num, LINEFILE, CNEXT_NT);
    else if(left->type == V_str && right->type == V_str){
        wchar_t *new_string = memWidecat(left->data.str.str, right->data.str.str, false, false);
        makeStringValue(new_string, LINEFILE, CNEXT_NT);
        memFree(new_string);
    } else
        setResultError(E_TypeException, CUL_ERROR(Add), LINEFILE, true, CNEXT_NT);
}

void vobject_sub_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeIntValue(left->data.int_.num - right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeDouValue(left->data.int_.num - right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num - right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num - right->data.dou.num, LINEFILE, CNEXT_NT);
    else
        setResultError(E_TypeException, CUL_ERROR(Sub), LINEFILE, true, CNEXT_NT);
}

void vobject_mul_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeIntValue(left->data.int_.num * right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeDouValue(left->data.int_.num * right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num * right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num * right->data.dou.num, LINEFILE, CNEXT_NT);
    else if(left->type == V_int && right->type == V_str) {
        Value *tmp = left;
        left = right;
        right = tmp;
        goto mul_str;
    } else if(left->type == V_str && right->type == V_int) mul_str: {
        wchar_t *new_string = memWidecpySelf(left->data.str.str, right->data.int_.num);
        makeStringValue(new_string, LINEFILE, CNEXT_NT);
        memFree(new_string);
    } else
        setResultError(E_TypeException, CUL_ERROR(Mul), LINEFILE, true, CNEXT_NT);
}

void vobject_div_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultCore(result);
    if (right->type == V_int && right->data.int_.num == 0 || right->type == V_dou && !(right->data.dou.num != 0))  // !(right->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_TypeException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->type == V_int && right->type == V_int) {
        lldiv_t div_result = lldiv(left->data.int_.num, right->data.int_.num);
        makeIntValue(div_result.quot, LINEFILE, CNEXT_NT);
    } else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num / (vdou)right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue((vdou)left->data.dou.num / right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num / right->data.int_.num, LINEFILE, CNEXT_NT);
    else
        setResultError(E_TypeException, CUL_ERROR(Div), LINEFILE, true, CNEXT_NT);
}

ResultType vobject_opt_core(O_FUNC, base_opt func){
    Value *left = NULL;
    Value *right = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"right", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    left = ap[0].value->value;
    right = ap[1].value->value;

    func(belong, result, inter, var_list, left, right);
    return result->type;
}

ResultType vobject_add(O_FUNC){
    return vobject_opt_core(CO_FUNC(arg, var_list, result, belong), vobject_add_base);
}

ResultType vobject_sub(O_FUNC){
    return vobject_opt_core(CO_FUNC(arg, var_list, result, belong), vobject_sub_base);
}

ResultType vobject_mul(O_FUNC){
    return vobject_opt_core(CO_FUNC(arg, var_list, result, belong), vobject_mul_base);
}

ResultType vobject_div(O_FUNC){
    return vobject_opt_core(CO_FUNC(arg, var_list, result, belong), vobject_div_base);
}

ResultType vobject_bool(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    bool result_ = false;
    Value *value = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    value = ap[0].value->value;
    switch (value->type) {
        case V_int:
            result_ = value->data.int_.num != 0;
            break;
        case V_dou:
            result_ = value->data.dou.num != 0;
            break;
        case V_str:
            result_ = memWidelen(value->data.str.str) > 0;
            break;
        case V_bool:
            result_ = value->data.bool_.bool_;
            break;
        case V_ell:
        case V_none:
            result_ = false;
            break;
        case V_list:
            result_ = value->data.list.size > 0;
            break;
        case V_dict:
            result_ = value->data.dict.size > 0;
            break;
        case V_lib:
            result_ = value->data.lib.handle != NULL;
            break;
        case V_file:
            result_ = value->data.file.file != NULL;
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(bool), LINEFILE, true, CNEXT_NT);
            return R_error;
    }
    makeBoolValue(result_, LINEFILE, CNEXT_NT);
    return result->type;
}

ResultType vobject_repo(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    wchar_t *repo = NULL;
    Value *value = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    switch (value->type){
        case V_int: {
            char str[30] = { NUL };
            snprintf(str, 30, "%lld", value->data.int_.num);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_dou: {
            char str[30] = { NUL };
            if (value->data.dou.num != 0)
                snprintf(str, 30, "%Lf", value->data.dou.num);
            else
                str[0] = '0';
            repo = memStrToWcs(str, false);
            break;
        }
        case V_str:
            repo = memWidecpy(value->data.str.str);
            break;
        case V_func: {
            char str[30] = { NUL };
            snprintf(str, 30, "(func on %p)", value);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_class: {
            char str[30] = { NUL };
            snprintf(str, 30, "(class on %p)", value);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_bool:
            if (value->data.bool_.bool_)
                repo = memStrToWcs("true", false);
            else
                repo = memStrToWcs("false", false);
            break;
        case V_ell:
            repo = memStrToWcs("...", false);
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(repo/str), LINEFILE, true, CNEXT_NT);
            return R_error;
    }
    makeStringValue(repo, LINEFILE, CNEXT_NT);
    memFree(repo);
    return result->type;
}

void registeredVObject(R_FUNC){
    LinkValue *object = inter->data.vobject;
    NameFunc tmp[] = {{inter->data.object_add, vobject_add, object_free_},
                      {inter->data.object_sub, vobject_sub, object_free_},
                      {inter->data.object_mul, vobject_mul, object_free_},
                      {inter->data.object_div, vobject_div, object_free_},
                      {inter->data.object_bool, vobject_bool, object_free_},
                      {inter->data.object_repo, vobject_repo, object_free_},
                      {inter->data.object_str, vobject_repo, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"vobject", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseVObject(Inter *inter){
    LinkValue *vobject = makeBaseChildClass(inter->data.object, inter);
    gc_addStatementLink(&vobject->gc_status);
    inter->data.vobject = vobject;
}
