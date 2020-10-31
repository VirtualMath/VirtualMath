#include "__ofunc.h"
typedef void (*base_opt)(FUNC_VOBJ);
void vobject_add_base(FUNC_VOBJ) {
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

void vobject_sub_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeIntValue(left->data.int_.num - right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeDouValue(left->data.int_.num - right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num - right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num - right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_pointer && right->type == V_pointer)
        makeIntValue((char *)left->data.pointer.pointer - (char *)right->data.pointer.pointer, LINEFILE, CNEXT_NT);
    else
        setResultError(E_TypeException, CUL_ERROR(Sub), LINEFILE, true, CNEXT_NT);
}

void vobject_mul_base(FUNC_VOBJ) {
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

void vobject_div_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->type == V_int && right->data.int_.num == 0 || right->type == V_dou && !(right->data.dou.num != 0))  // !(right->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_ValueException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->type == V_int && right->type == V_int) {
        lldiv_t div_result = lldiv(left->data.int_.num, right->data.int_.num);
        makeIntValue(div_result.quot, LINEFILE, CNEXT_NT);
    } else if (left->type == V_dou && right->type == V_int)
        makeDouValue(left->data.dou.num / (vdou)right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeDouValue((vdou)left->data.int_.num / right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeDouValue(left->data.dou.num / right->data.dou.num, LINEFILE, CNEXT_NT);
    else
        setResultError(E_TypeException, CUL_ERROR(Div), LINEFILE, true, CNEXT_NT);
}

void vobject_intdiv_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->type == V_int && right->data.int_.num == 0 || right->type == V_dou && (vint)right->data.dou.num == 0)  // !(right->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_TypeException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->type == V_int && right->type == V_int) {
        lldiv_t div_result = lldiv(left->data.int_.num, right->data.int_.num);
        makeIntValue(div_result.quot, LINEFILE, CNEXT_NT);
    } else if (left->type == V_dou && right->type == V_int)
        makeIntValue((vint)left->data.dou.num / right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeIntValue(left->data.int_.num / (vint)right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeIntValue((vint)left->data.dou.num / (vint)right->data.dou.num, LINEFILE, CNEXT_NT);
    else
        setResultError(E_TypeException, CUL_ERROR(Div), LINEFILE, true, CNEXT_NT);
}

void vobject_mod_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->type == V_int && right->data.int_.num == 0)  // !(right->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_TypeException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->type == V_int && right->type == V_int) {
        lldiv_t div_result = lldiv(left->data.int_.num, right->data.int_.num);
        makeIntValue(div_result.rem, LINEFILE, CNEXT_NT);
    } else
        setResultError(E_TypeException, CUL_ERROR(Div), LINEFILE, true, CNEXT_NT);
}

void vobject_pow_base(FUNC_VOBJ) {
    setResultCore(result);
    errno = 0;  // 初始化error
    vdou re;
    if (left->type == V_int && right->type == V_int)
        re = pow(left->data.int_.num, right->data.int_.num);
    else if (left->type == V_dou && right->type == V_int)
        re = powl(left->data.dou.num, right->data.int_.num);
    else if (left->type == V_int && right->type == V_dou)
        re = powl(left->data.int_.num, right->data.dou.num);
    else if (left->type == V_dou && right->type == V_dou)
        re = powl(left->data.dou.num, right->data.dou.num);
    else {
        setResultError(E_TypeException, CUL_ERROR(Div), LINEFILE, true, CNEXT_NT);
        return;
    }
    if (errno != 0)
        setResultFromERR(E_ValueException, CNEXT_NT);
    else
        makeDouValue(re, LINEFILE, CNEXT_NT);
}

void vobject_eq_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeBoolValue(left->data.int_.num == right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeBoolValue(left->data.int_.num == right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeBoolValue(left->data.dou.num == right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeBoolValue(left->data.dou.num == right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_pointer && right->type == V_pointer)
        makeBoolValue((char *)left->data.pointer.pointer == (char *)right->data.pointer.pointer, LINEFILE, CNEXT_NT);
    else if (left->type == V_str && right->type == V_str)
        makeBoolValue(eqWide(left->data.str.str, left->data.str.str), LINEFILE, CNEXT_NT);
    else
        makeBoolValue(left == right, LINEFILE, CNEXT_NT);
}

void vobject_noteq_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->type == V_int && right->type == V_int)
        makeBoolValue(left->data.int_.num != right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_int && right->type == V_dou)
        makeBoolValue(left->data.int_.num != right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_int)
        makeBoolValue(left->data.dou.num != right->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_dou && right->type == V_dou)
        makeBoolValue(left->data.dou.num != right->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->type == V_pointer && right->type == V_pointer)
        makeBoolValue((char *)left->data.pointer.pointer != (char *)right->data.pointer.pointer, LINEFILE, CNEXT_NT);
    else if (left->type == V_str && right->type == V_str)
        makeBoolValue(!(eqWide(left->data.str.str, left->data.str.str)), LINEFILE, CNEXT_NT);
    else
        makeBoolValue(left != right, LINEFILE, CNEXT_NT);
}

#define BITMACRO(SYMBOL, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->type == V_int && right->type == V_int) \
        makeIntValue(left->data.int_.num SYMBOL right->data.int_.num, LINEFILE, CNEXT_NT); \
    else \
        setResultError(E_TypeException, CUL_ERROR(TYPE), LINEFILE, true, CNEXT_NT); \
}

BITMACRO(&, band, Bit And)
BITMACRO(|, bor, Bit Or)
BITMACRO(^, bxor, Bit Xor)
#undef BITMACRO

#define BITMOVEMACRO(SYMBOL1, SYMBOL2, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->type == V_int && right->type == V_int) { \
        if (right->data.int_.num >= 0) \
            makeIntValue(left->data.int_.num SYMBOL1 (unsigned)right->data.int_.num, LINEFILE, CNEXT_NT); \
        else \
            makeIntValue(left->data.int_.num SYMBOL2 (unsigned)(-right->data.int_.num), LINEFILE, CNEXT_NT); \
    } else \
        setResultError(E_TypeException, CUL_ERROR(TYPE), LINEFILE, true, CNEXT_NT); \
}

BITMOVEMACRO(<<, >>, bl, Bit Left)
BITMOVEMACRO(>>, <<, br, Bit Right)
#undef BITMOVEMACRO

#define COMPAREMACRO(SYMBOL, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->type == V_int && right->type == V_int) \
        makeBoolValue(left->data.int_.num SYMBOL right->data.int_.num, LINEFILE, CNEXT_NT); \
    else if (left->type == V_int && right->type == V_dou) \
        makeBoolValue(left->data.int_.num SYMBOL right->data.dou.num, LINEFILE, CNEXT_NT); \
    else if (left->type == V_dou && right->type == V_int) \
        makeBoolValue(left->data.dou.num SYMBOL right->data.int_.num, LINEFILE, CNEXT_NT); \
    else if (left->type == V_dou && right->type == V_dou) \
        makeBoolValue(left->data.dou.num SYMBOL right->data.dou.num, LINEFILE, CNEXT_NT); \
    else if (left->type == V_pointer && right->type == V_pointer) \
        makeBoolValue((char *)left->data.pointer.pointer SYMBOL (char *)right->data.pointer.pointer, LINEFILE, CNEXT_NT); \
    else \
        setResultError(E_TypeException, CUL_ERROR(TYPE), LINEFILE, true, CNEXT_NT); \
}

COMPAREMACRO(>, more, More)
COMPAREMACRO(>=, moreeq, More Eq)
COMPAREMACRO(<=, lesseq, Less Eq)
COMPAREMACRO(<, less, Less Eq)
#undef COMPAREMACRO

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

    func(CFUNC_VOBJ(var_list, result, belong, left, right));
    return result->type;
}

#define COMPAREFUNCMACRO(TYPE) ResultType vobject_##TYPE(O_FUNC){ return vobject_opt_core(CO_FUNC(arg, var_list, result, belong), vobject_##TYPE##_base); }
COMPAREFUNCMACRO(add)
COMPAREFUNCMACRO(sub)
COMPAREFUNCMACRO(mul)
COMPAREFUNCMACRO(div)
COMPAREFUNCMACRO(intdiv)
COMPAREFUNCMACRO(mod)
COMPAREFUNCMACRO(pow)
COMPAREFUNCMACRO(eq)
COMPAREFUNCMACRO(noteq)
COMPAREFUNCMACRO(moreeq)
COMPAREFUNCMACRO(lesseq)
COMPAREFUNCMACRO(more)
COMPAREFUNCMACRO(less)
COMPAREFUNCMACRO(band)
COMPAREFUNCMACRO(bor)
COMPAREFUNCMACRO(bxor)
COMPAREFUNCMACRO(bl)
COMPAREFUNCMACRO(br)
#undef COMPAREFUNCMACRO

void vobject_negate_base(FUNC_VOBJR) {
    setResultCore(result);
    switch (left->type) {
        case V_int:
            makeIntValue(-(left->data.int_.num), LINEFILE, CNEXT_NT);
            break;
        case V_dou:
            makeDouValue(-(left->data.dou.num), LINEFILE, CNEXT_NT);
            break;
        case V_str: {
            wchar_t *new = memWidecpySelf(left->data.str.str, -1);
            makeStringValue(new, LINEFILE, CNEXT_NT);
            memFree(new);
            break;
        }
        case V_bool:
            makeBoolValue(!(left->data.bool_.bool_), LINEFILE, CNEXT_NT);
            break;
        case V_none:
            setResult(result, inter);
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(Negate), LINEFILE, true, CNEXT_NT);
            break;
    }
}

void vobject_bnot_base(FUNC_VOBJR) {
    setResultCore(result);
    switch (left->type) {
        case V_int:
            makeIntValue(~(unsigned long long)(left->data.int_.num), LINEFILE, CNEXT_NT);
            break;
        case V_bool:
            makeBoolValue(!(left->data.bool_.bool_), LINEFILE, CNEXT_NT);
            break;
        case V_none:
            setResult(result, inter);
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(Negate), LINEFILE, true, CNEXT_NT);
            break;
    }
}

ResultType vobject_negate(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    vobject_negate_base(CFUNC_VOBJR(var_list, result, belong, ap[0].value->value));
    return result->type;
}

ResultType vobject_bnot(O_FUNC){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    vobject_bnot_base(CFUNC_VOBJR(var_list, result, belong, ap[0].value->value));
    return result->type;
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
        case V_pointer:
            result_ = value->data.pointer.pointer != NULL;
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

    switch (value->type){  // node和list以及dict都不再此处设定
        case V_int: {
            char str[30] = { NUL };
            snprintf(str, 30, "%lld", value->data.int_.num);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_pointer: {
            char str[30] = { NUL };
            snprintf(str, 30, "%p", value->data.pointer.pointer);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_dou: {
            char str[30] = { NUL };
            if (value->data.dou.num != 0)
                snprintf(str, 30, "%Lg", value->data.dou.num);
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
        case V_file: {
            size_t len = memStrlen(value->data.file.path) + memStrlen(value->data.file.mode) + 20;
            char str[len];  // 变长数组
            snprintf(str, len, "(file on %s %s)", value->data.file.path, value->data.file.mode);
            repo = memStrToWcs(str, false);
            break;
        }
        default:
            setResultError(E_TypeException, CUL_ERROR(repo/str), LINEFILE, true, CNEXT_NT);
            return R_error;
    }
    makeStringValue(repo, LINEFILE, CNEXT_NT);
    memFree(repo);
    return result->type;
}

void registeredVObject(R_FUNC){
    LinkValue *object = inter->data.base_obj[B_VOBJECT];
    NameFunc tmp[] = {{inter->data.mag_func[M_ADD], vobject_add, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SUB], vobject_sub, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MUL], vobject_mul, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DIV], vobject_div, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BOOL], vobject_bool, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_REPO], vobject_repo, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_STR], vobject_repo, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_INTDIV], vobject_intdiv, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOD], vobject_mod, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_POW], vobject_pow, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_EQ], vobject_eq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_NOTEQ], vobject_noteq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOREEQ], vobject_moreeq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESSEQ], vobject_lesseq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MORE], vobject_more, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESS], vobject_less, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_BAND], vobject_band, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BOR], vobject_bor, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BXOR], vobject_bxor, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BL], vobject_bl, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BR], vobject_br, fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_NEGATE], vobject_negate, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BNOT], vobject_bnot, fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"vobject", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseVObject(Inter *inter){
    LinkValue *vobject = makeBaseChildClass(inter->data.base_obj[B_OBJECT], inter);
    gc_addStatementLink(&vobject->gc_status);
    inter->data.base_obj[B_VOBJECT] = vobject;
}
