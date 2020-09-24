#include "__ofunc.h"

LinkValue *registeredFunctionCore(OfficialFunction of, wchar_t *name, INTER_FUNCTIONSIG_NOT_ST) {
    LinkValue *value = NULL;
    makeCFunctionValue(of, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    value = result->value;
    result->value = NULL;
    freeResult(result);
    addStrVar(name, false, true, value, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    gc_freeTmpLink(&value->gc_status);
    return value;
}

bool iterNameFunc(NameFunc *list, INTER_FUNCTIONSIG_NOT_ST){
    setResultCore(result);
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return false;
        value->value->data.function.function_data.pt_type = list->type;
        freeResult(result);
    }
    return true;
}

bool iterClassFunc(NameFunc *list, INTER_FUNCTIONSIG_NOT_ST){
    VarList *object_var = belong->value->object.var;
    VarList *object_backup = object_var->next;
    enum FunctionPtType bak = inter->data.default_pt_type;
    bool return_ = true;

    setResultCore(result);
    object_var->next = var_list;
    inter->data.default_pt_type = object_free_;

    gc_freeze(inter, object_backup, NULL, true);
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, CALL_INTER_FUNCTIONSIG_NOT_ST(object_var, result, belong));
        if (!CHECK_RESULT(result)) {
            return_ = false;
            break;
        }
        value->value->data.function.function_data.pt_type = list->type;
        freeResult(result);
    }
    gc_freeze(inter, object_backup, NULL, false);

    object_var->next = object_backup;
    inter->data.default_pt_type = bak;

    return return_;
}

void iterBaseNameFunc(NameFunc *list, LinkValue *father, INTER_FUNCTIONSIG_CORE){
    Result result;
    setResultCore(&result);
    if (!iterNameFunc(list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &result, father)))
        printError(&result, inter, true);
    freeResult(&result);
}

void iterBaseClassFunc(NameFunc *list, LinkValue *father, INTER_FUNCTIONSIG_CORE){
    Result result;
    setResultCore(&result);
    if (!iterClassFunc(list, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &result, father)))
        printError(&result, inter, true);
    freeResult(&result);
}

LinkValue *makeBaseChildClass(LinkValue *inherit, Inter *inter) {
    Inherit *father_value = NULL;
    Value *new = NULL;
    {
        Argument *arg = makeValueArgument(inherit);
        gc_addTmpLink(&inherit->gc_status);
        father_value = setFather(arg);
        freeArgument(arg, true);
        gc_freeTmpLink(&inherit->gc_status);
    }
    new = makeClassValue(inter->var_list, inter, father_value);
    return makeLinkValue(new, inter->base_belong, inter);
}

bool checkIndex(vnum *index, const vnum *size, INTER_FUNCTIONSIG_NOT_ST){
    setResultCore(result);
    if (*index < 0)
        *index = *size + *index;
    if (*index >= *size){
        setResultError(E_IndexException, "Index too max", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    } else if (*index < 0){
        setResultError(E_IndexException, "Index is less than 0", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    }
    return true;  // true - 保持result为setResultCore的结果
}

bool checkSlice(vnum *first, vnum *second, const vnum *stride, vnum size, INTER_FUNCTIONSIG_NOT_ST){
    setResultCore(result);
    *first = *first < 0 ? *first + size : *first;
    *second = *second < 0 ? *second + size : *second;
    if (*second > size || *first >= size){
        setResultError(E_IndexException, "Index too max", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    } else if (*first < 0 || *second <= 0){
        setResultError(E_IndexException, "Index is less than 0", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    }

    if (*stride == 0 || *first > *second && stride > 0 || *first < *second && stride < 0){
        setResultError(E_StrideException, "Stride is 0 or Unfinished iteration", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return false;
    }
    return true;
}

void addBaseClassVar(wchar_t *name, LinkValue *obj, LinkValue *belong, Inter *inter) {
    Result result;
    setResultCore(&result);
    addStrVar(name, false, true, obj, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(inter->var_list, &result, belong));
    if (!RUN_TYPE(result.type))
        printError(&result, inter, true);
    freeResult(&result);
}

void newObjectSettingPresetting(LinkValue *func, LinkValue *name, Inter *inter) {
    Result result;
    setResultCore(&result);
    newObjectSetting(name, 0, "sys", func, &result, inter, NULL);
    if (RUN_TYPE(result.type))
        printError(&result, inter, true);
    freeResult(&result);
}
