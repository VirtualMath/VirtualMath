#include "__ofunc.h"

typedef void (*base_opt)(FUNC_VOBJ);

#define OPERATION_DEFAULT(M_NAME_, ERR_NAME) do{ if (is_left) {runOperationFromValue(right, left, right, inter->data.mag_func[M_##M_NAME_], LINEFILE, CNEXT_NT);} else {setResultError(E_TypeException, CUL_ERROR(ERR_NAME), LINEFILE, true, CNEXT_NT);} }while(0)

void vobject_add_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->value->type == V_int && right->value->type == V_int)
        makeIntValue(left->value->data.int_.num + right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeDouValue(left->value->data.int_.num + right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_int)
        makeDouValue(left->value->data.dou.num + right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeDouValue(left->value->data.dou.num + right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_str && right->value->type == V_str) {
        wchar_t *new_string = memWidecat(left->value->data.str.str, right->value->data.str.str, false, false);
        makeStringValue(new_string, LINEFILE, CNEXT_NT);
        memFree(new_string);
    } else
        OPERATION_DEFAULT(ADD, Add);
}

void vobject_sub_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->value->type == V_int && right->value->type == V_int)
        makeIntValue(left->value->data.int_.num - right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeDouValue(left->value->data.int_.num - right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_int)
        makeDouValue(left->value->data.dou.num - right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeDouValue(left->value->data.dou.num - right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_pointer && right->value->type == V_pointer)
        makeIntValue((char *) left->value->data.pointer.pointer - (char *) right->value->data.pointer.pointer, LINEFILE,
                     CNEXT_NT);
    else
        OPERATION_DEFAULT(SUB, Sub);
}

void vobject_mul_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->value->type == V_int && right->value->type == V_int)
        makeIntValue(left->value->data.int_.num * right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeDouValue(left->value->data.int_.num * right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_int)
        makeDouValue(left->value->data.dou.num * right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeDouValue(left->value->data.dou.num * right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_str) {
        Value *tmp = left->value;
        left->value = right->value;
        right->value = tmp;
        goto mul_str;
    } else if (left->value->type == V_str && right->value->type == V_int)
        mul_str:
        {
            wchar_t *new_string = memWidecpySelf(left->value->data.str.str, right->value->data.int_.num);
            makeStringValue(new_string, LINEFILE, CNEXT_NT);
            memFree(new_string);
        }
    else
        OPERATION_DEFAULT(MUL, Mul);
}

void vobject_div_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->value->type == V_int && right->value->data.int_.num == 0 || right->value->type == V_dou &&
                                                                           !(right->value->data.dou.num !=
                                                                             0))  // !(right->value->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_ValueException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_int) {
        lldiv_t div_result = lldiv(left->value->data.int_.num, right->value->data.int_.num);
        makeIntValue(div_result.quot, LINEFILE, CNEXT_NT);
    } else if (left->value->type == V_dou && right->value->type == V_int)
        makeDouValue(left->value->data.dou.num / (vdou) right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeDouValue((vdou) left->value->data.int_.num / right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeDouValue(left->value->data.dou.num / right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else
        OPERATION_DEFAULT(DIV, Div);
}

void vobject_intdiv_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->value->type == V_int && right->value->data.int_.num == 0 || right->value->type == V_dou &&
                                                                           (vint) right->value->data.dou.num ==
                                                                           0)  // !(right->value->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_TypeException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_int) {
        lldiv_t div_result = lldiv(left->value->data.int_.num, right->value->data.int_.num);
        makeIntValue(div_result.quot, LINEFILE, CNEXT_NT);
    } else if (left->value->type == V_dou && right->value->type == V_int)
        makeIntValue((vint) left->value->data.dou.num / right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeIntValue(left->value->data.int_.num / (vint) right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeIntValue((vint) left->value->data.dou.num / (vint) right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else
        OPERATION_DEFAULT(INTDIV, Int Div);
}

void vobject_mod_base(FUNC_VOBJ) {
    setResultCore(result);
    if (right->value->type == V_int &&
        right->value->data.int_.num == 0)  // !(right->value->data.dou.num != 0) 因为long double检查是否位0时容易出错
        setResultError(E_TypeException, L"divisor mustn't be 0", LINEFILE, true, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_int) {
        lldiv_t div_result = lldiv(left->value->data.int_.num, right->value->data.int_.num);
        makeIntValue(div_result.rem, LINEFILE, CNEXT_NT);
    } else
        OPERATION_DEFAULT(MOD, Mod);
}

void vobject_pow_base(FUNC_VOBJ) {
    setResultCore(result);
    errno = 0;  // 初始化error
    vdou re;
    if (left->value->type == V_int && right->value->type == V_int)
        re = pow(left->value->data.int_.num, right->value->data.int_.num);
    else if (left->value->type == V_dou && right->value->type == V_int)
        re = powl(left->value->data.dou.num, right->value->data.int_.num);
    else if (left->value->type == V_int && right->value->type == V_dou)
        re = powl(left->value->data.int_.num, right->value->data.dou.num);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        re = powl(left->value->data.dou.num, right->value->data.dou.num);
    else {
        OPERATION_DEFAULT(POW, Pow);
        return;
    }
    if (errno != 0)
        setResultFromERR(E_ValueException, CNEXT_NT);
    else
        makeDouValue(re, LINEFILE, CNEXT_NT);
}

void vobject_eq_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->value->type == V_int && right->value->type == V_int)
        makeBoolValue(left->value->data.int_.num == right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeBoolValue(left->value->data.int_.num == right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_int)
        makeBoolValue(left->value->data.dou.num == right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeBoolValue(left->value->data.dou.num == right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_pointer && right->value->type == V_pointer)
        makeBoolValue((char *) left->value->data.pointer.pointer == (char *) right->value->data.pointer.pointer,
                      LINEFILE, CNEXT_NT);
    else if (left->value->type == V_str && right->value->type == V_str)
        makeBoolValue(eqWide(left->value->data.str.str, left->value->data.str.str), LINEFILE, CNEXT_NT);
    else
        OPERATION_DEFAULT(EQ, Eq);
}

void vobject_noteq_base(FUNC_VOBJ) {
    setResultCore(result);
    if (left->value->type == V_int && right->value->type == V_int)
        makeBoolValue(left->value->data.int_.num != right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_int && right->value->type == V_dou)
        makeBoolValue(left->value->data.int_.num != right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_int)
        makeBoolValue(left->value->data.dou.num != right->value->data.int_.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_dou && right->value->type == V_dou)
        makeBoolValue(left->value->data.dou.num != right->value->data.dou.num, LINEFILE, CNEXT_NT);
    else if (left->value->type == V_pointer && right->value->type == V_pointer)
        makeBoolValue((char *) left->value->data.pointer.pointer != (char *) right->value->data.pointer.pointer,
                      LINEFILE, CNEXT_NT);
    else if (left->value->type == V_str && right->value->type == V_str)
        makeBoolValue(!(eqWide(left->value->data.str.str, left->value->data.str.str)), LINEFILE, CNEXT_NT);
    else
        OPERATION_DEFAULT(NOTEQ, Not Eq);
}

#define BITMACRO(SYMBOL, M_NAME_, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->value->type == V_int && right->value->type == V_int) \
        makeIntValue(left->value->data.int_.num SYMBOL right->value->data.int_.num, LINEFILE, CNEXT_NT); \
    else OPERATION_DEFAULT(M_NAME_, TYPE); \
}

BITMACRO(&, BAND, band, Bit And)

BITMACRO(|, BOR, bor, Bit Or)

BITMACRO(^, BXOR, bxor, Bit Xor)

#undef BITMACRO

#define BITMOVEMACRO(SYMBOL1, SYMBOL2, M_NAME_, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->value->type == V_int && right->value->type == V_int) { \
        if (right->value->data.int_.num >= 0) \
            makeIntValue(left->value->data.int_.num SYMBOL1 (unsigned)right->value->data.int_.num, LINEFILE, CNEXT_NT); \
        else \
            makeIntValue(left->value->data.int_.num SYMBOL2 (unsigned)(-right->value->data.int_.num), LINEFILE, CNEXT_NT); \
    } else OPERATION_DEFAULT(M_NAME_, TYPE); \
}

BITMOVEMACRO(<<, >>, BL, bl, Bit Left)

BITMOVEMACRO(>>, <<, BR, br, Bit Right)

#undef BITMOVEMACRO

#define COMPAREMACRO(SYMBOL, M_NAME_, NAME, TYPE) void vobject_##NAME##_base(FUNC_VOBJ) { \
    setResultCore(result); \
    if (left->value->type == V_int && right->value->type == V_int) \
        makeBoolValue(left->value->data.int_.num SYMBOL right->value->data.int_.num, LINEFILE, CNEXT_NT); \
    else if (left->value->type == V_int && right->value->type == V_dou) \
        makeBoolValue(left->value->data.int_.num SYMBOL right->value->data.dou.num, LINEFILE, CNEXT_NT); \
    else if (left->value->type == V_dou && right->value->type == V_int) \
        makeBoolValue(left->value->data.dou.num SYMBOL right->value->data.int_.num, LINEFILE, CNEXT_NT); \
    else if (left->value->type == V_dou && right->value->type == V_dou) \
        makeBoolValue(left->value->data.dou.num SYMBOL right->value->data.dou.num, LINEFILE, CNEXT_NT); \
    else if (left->value->type == V_pointer && right->value->type == V_pointer) \
        makeBoolValue((char *)left->value->data.pointer.pointer SYMBOL (char *)right->value->data.pointer.pointer, LINEFILE, CNEXT_NT); \
    else OPERATION_DEFAULT(M_NAME_, TYPE); \
}

COMPAREMACRO(>, MORE, more, More)

COMPAREMACRO(>=, MOREEQ, moreeq, More Eq)

COMPAREMACRO(<=, LESSEQ, lesseq, Less Eq)

COMPAREMACRO(<, LESS, less, Less Eq)

#undef COMPAREMACRO

static ResultType vobject_opt_core(O_FUNC, base_opt func) {
    LinkValue *left;
    LinkValue *right;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"left", .must=1, .long_arg=false},
                           {.type=name_value, .name=L"right", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    left = ap[1].value;
    right = ap[2].value;

    func(CFUNC_VOBJ(var_list, result, belong, left, right, (ap[0].value->value ==
                                                            left->value)));  // 如果 (ap[0].value->value == left->value) 为 true 则代表 is_left 模式
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
    switch (left->value->type) {
        case V_int:
            makeIntValue(-(left->value->data.int_.num), LINEFILE, CNEXT_NT);
            break;
        case V_dou:
            makeDouValue(-(left->value->data.dou.num), LINEFILE, CNEXT_NT);
            break;
        case V_str: {
            wchar_t *new = memWidecpySelf(left->value->data.str.str, -1);
            makeStringValue(new, LINEFILE, CNEXT_NT);
            memFree(new);
            break;
        }
        case V_bool:
            makeBoolValue(!(left->value->data.bool_.bool_), LINEFILE, CNEXT_NT);
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
    switch (left->value->type) {
        case V_int:
            makeIntValue(~(unsigned long long) (left->value->data.int_.num), LINEFILE, CNEXT_NT);
            break;
        case V_bool:
            makeBoolValue(!(left->value->data.bool_.bool_), LINEFILE, CNEXT_NT);
            break;
        case V_none:
            setResult(result, inter);
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(Negate), LINEFILE, true, CNEXT_NT);
            break;
    }
}

static ResultType vobject_negate(O_FUNC) {
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"val", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    vobject_negate_base(CFUNC_VOBJR(var_list, result, belong, ap[1].value));
    return result->type;
}

static ResultType vobject_bnot(O_FUNC) {
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name=L"val", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CNEXT_NT);
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    vobject_bnot_base(CFUNC_VOBJR(var_list, result, belong, ap[1].value));
    return result->type;
}

static ResultType vobject_bool(O_FUNC) {
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
        case V_struct:
            result_ = value->data.struct_.len > 0;
            break;
        default:
            setResultError(E_TypeException, CUL_ERROR(bool), LINEFILE, true, CNEXT_NT);
            return R_error;
    }
    makeBoolValue(result_, LINEFILE, CNEXT_NT);
    return result->type;
}

static ResultType vobject_repo(O_FUNC) {
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

    switch (value->type) {  // node和list以及dict都不再此处设定
        case V_int: {
            char str[30] = {NUL};
            snprintf(str, 30, "%lld", value->data.int_.num);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_pointer: {
            char str[30] = {NUL};
            snprintf(str, 30, "%p", value->data.pointer.pointer);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_dou: {
            char str[30] = {NUL};
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
        case V_struct:
            if (value->data.struct_.len != 0){
                char *tmp = memStrcpy("(");
                for (vint i = 0; i < value->data.struct_.len; i ++) {
                    char val[20];
                    snprintf(val, 20, "%x ", value->data.struct_.data[i]);
                    tmp = memStrcat(tmp, val, true, false);
                }
                tmp[memStrlen(tmp) - 1] = ')';  // 去掉一个空格, 改成括号
                repo = memStrToWcs(tmp, true);
                break;
            } else
                repo = memWidecpy(L"(struct NULL)");
            break;
        case V_func: {
            char str[30] = {NUL};
            snprintf(str, 30, "(func on %p)", value);
            repo = memStrToWcs(str, false);
            break;
        }
        case V_class: {
            char str[30] = {NUL};
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
            setResultError(E_TypeException, CUL_ERROR(repo / str), LINEFILE, true, CNEXT_NT);
            return R_error;
    }
    makeStringValue(repo, LINEFILE, CNEXT_NT);
    memFree(repo);
    return result->type;
}

void registeredVObject(R_FUNC) {
    LinkValue *object = inter->data.base_obj[B_VOBJECT];
    NameFunc tmp[] = {{inter->data.mag_func[M_ADD],    vobject_add,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_SUB],    vobject_sub,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MUL],    vobject_mul,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_DIV],    vobject_div,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_INTDIV], vobject_intdiv, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOD],    vobject_mod,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_POW],    vobject_pow,    fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_EQ],     vobject_eq,     fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_NOTEQ],  vobject_noteq,  fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MOREEQ], vobject_moreeq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESSEQ], vobject_lesseq, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_MORE],   vobject_more,   fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_LESS],   vobject_less,   fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_BAND],   vobject_band,   fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BOR],    vobject_bor,    fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BXOR],   vobject_bxor,   fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BL],     vobject_bl,     fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BR],     vobject_br,     fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_NEGATE], vobject_negate, fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_BNOT],   vobject_bnot,   fp_obj, .var=nfv_notpush},

                      {inter->data.mag_func[M_BOOL],   vobject_bool,   fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_REPO],   vobject_repo,   fp_obj, .var=nfv_notpush},
                      {inter->data.mag_func[M_STR],    vobject_repo,   fp_obj, .var=nfv_notpush},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addBaseClassVar(L"vobject", object, belong, inter);
    iterBaseClassFunc(tmp, object, CFUNC_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseVObject(Inter *inter) {
    LinkValue *vobject = makeBaseChildClass(inter->data.base_obj[B_OBJECT], inter);
    gc_addStatementLink(&vobject->gc_status);
    inter->data.base_obj[B_VOBJECT] = vobject;
}
