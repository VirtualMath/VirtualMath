#include "__ofunc.h"

LinkValue *registeredFunctionCore(OfficialFunction of, wchar_t *name, enum NameFuncVar nfv, FUNC_NT) {
    LinkValue *value = NULL;
    switch (nfv) {
        case nfv_notpush:
            makeCFunctionValue(of, LINEFILE, true, false, CNEXT_NT);
            break;
        case nfv_inline:
            makeCFunctionValue(of, LINEFILE, false, true, CNEXT_NT);
            break;
        default:
            makeCFunctionValue(of, LINEFILE, true, true, CNEXT_NT);
            break;
    }
    GET_RESULT(value, result);
    addStrVar(name, false, true, value, LINEFILE, false, CNEXT_NT);
    gc_freeTmpLink(&value->gc_status);
    return value;
}

bool iterNameFunc(NameFunc *list, FUNC_NT){
    setResultCore(result);
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, list->var, CNEXT_NT);
        if (!CHECK_RESULT(result))
            return false;
        value->value->data.function.function_data.pt_type = list->type;
        freeResult(result);
    }
    return true;
}

bool iterClassFunc(NameFunc *list, FUNC_NT){
    VarList *object_var = belong->value->object.var;
    VarList *object_backup = object_var->next;
    enum FunctionPtType bak = inter->data.default_pt_type;
    bool return_ = true;

    setResultCore(result);
    object_var->next = var_list;
    inter->data.default_pt_type = fp_func_obj;

    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, list->var, CFUNC_NT(object_var, result, belong));
        if (!CHECK_RESULT(result)) {
            return_ = false;
            break;
        }
        value->value->data.function.function_data.pt_type = list->type;
        freeResult(result);
    }

    object_var->next = object_backup;
    inter->data.default_pt_type = bak;

    return return_;
}

void iterBaseNameFunc(NameFunc *list, LinkValue *belong, FUNC_CORE){
    Result result;
    setResultCore(&result);
    if (!iterNameFunc(list, CFUNC_NT(var_list, &result, belong)))
        printError(&result, inter, true);
    freeResult(&result);
}

void iterBaseClassFunc(NameFunc *list, LinkValue *belong, FUNC_CORE){
    Result result;
    setResultCore(&result);
    if (!iterClassFunc(list, CFUNC_NT(var_list, &result, belong)))
        printError(&result, inter, true);
    freeResult(&result);
}

LinkValue *makeBaseChildClass(LinkValue *inherit, Inter *inter) {
    Inherit *father_value;
    Value *new;
    LinkValue *re;
    {
        Argument *arg = makeValueArgument(inherit);
        gc_addTmpLink(&inherit->gc_status);
        father_value = setFather(arg);
        freeArgument(arg, true);
        gc_freeTmpLink(&inherit->gc_status);
    }
    new = makeClassValue(inter->var_list, inter, father_value);
    re = makeLinkValue(new, inter->base_belong, auto_aut, inter);
    gc_freeTmpLink(&new->gc_status);
    return re;
}

bool checkIndex(vint *index, const vint *size, FUNC_NT){
    setResultCore(result);
    if (*index < 0)
        *index = *size + *index;
    if (*index >= *size){
        setResultError(E_IndexException, L"index too max", LINEFILE, true, CNEXT_NT);
        return false;
    } else if (*index < 0){
        setResultError(E_IndexException, L"index is less than 0", LINEFILE, true, CNEXT_NT);
        return false;
    }
    return true;  // true - 保持result为setResultCore的结果
}

bool checkSlice(vint *first, vint *second, const vint *stride, vint size, FUNC_NT){
    setResultCore(result);
    *first = *first < 0 ? *first + size : *first;
    *second = *second < 0 ? *second + size : *second;
    if (*second > size || *first >= size){
        setResultError(E_IndexException, L"index too max", LINEFILE, true, CNEXT_NT);
        return false;
    } else if (*first < 0 || *second <= 0){
        setResultError(E_IndexException, L"index is less than 0", LINEFILE, true, CNEXT_NT);
        return false;
    }

    if (*stride == 0 || *first > *second && stride > 0 || *first < *second && stride < 0){
        setResultError(E_StrideException, L"stride is 0 or Unfinished iteration", LINEFILE, true, CNEXT_NT);
        return false;
    }
    return true;
}

void addBaseClassVar(wchar_t *name, LinkValue *obj, LinkValue *belong, Inter *inter) {
    Result result;
    setResultCore(&result);
    addStrVar(name, false, true, obj, LINEFILE, false, CFUNC_NT(inter->var_list, &result, belong));
    if (!RUN_TYPE(result.type))
        printError(&result, inter, true);
    freeResult(&result);
}