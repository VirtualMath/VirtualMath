#include "__ofunc.h"

LinkValue *registeredFunctionCore(OfficialFunction of, char *name, LinkValue *belong, INTER_FUNCTIONSIG_CORE) {
    LinkValue *value = makeLinkValue(makeCFunctionValue(of, var_list, inter), belong, inter);
    addStrVar(name, false, true, value, belong, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    return value;
}

void iterNameFunc(NameFunc list[], LinkValue *father, INTER_FUNCTIONSIG_CORE){
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
        value->value->data.function.function_data.pt_type = list->type;
    }
}

void iterClassFunc(NameFunc list[], LinkValue *father, INTER_FUNCTIONSIG_CORE){
    VarList *object_backup = father->value->object.var->next;
    VarList *object_var = father->value->object.var;
    enum FunctionPtType bak = inter->data.default_pt_type;
    object_var->next = var_list;
    inter->data.default_pt_type = object_free_;
    gc_freeze(inter, object_backup, NULL, true);
    for (PASS; list->of != NULL; list++) {
        LinkValue *value = registeredFunctionCore(list->of, list->name, father, CALL_INTER_FUNCTIONSIG_CORE(object_var));
        value->value->data.function.function_data.pt_type = list->type;
    }
    gc_freeze(inter, object_backup, NULL, false);
    object_var->next = object_backup;
    inter->data.default_pt_type = bak;
}

Value *makeBaseChildClass(Value *inherit, Inter *inter) {
    Inherit *father_value = NULL;
    Value *num = NULL;
    {
        LinkValue *father_ = makeLinkValue(inherit, inter->base_father, inter);
        Argument *arg = makeValueArgument(father_);
        gc_addTmpLink(&father_->gc_status);
        father_value = setFather(arg);
        freeArgument(arg, true);
        gc_freeTmpLink(&father_->gc_status);
    }
    num = makeClassValue(copyVarList(inter->var_list, false, inter), inter, father_value);
    return num;
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
