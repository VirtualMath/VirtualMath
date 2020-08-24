#include "__ofunc.h"

typedef void (*base_opt)(LinkValue *, Result *, struct Inter *, VarList *var_list, Value *, Value *);

void vobject_add_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num + right->data.num.num, inter);
    else if(left->type == string && right->type == string){
        char *new_string = memStrcat(left->data.str.str, right->data.str.str, false, false);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(E_TypeException, "Get Not Support Value", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
}

void vobject_sub_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num - right->data.num.num, inter);
    else
        setResultError(E_TypeException, "Get Not Support Value", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
}

void vobject_mul_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num * right->data.num.num, inter);
    else if(left->type == number && right->type == string) {
        Value *tmp = left;
        left = right;
        right = tmp;
        goto mul_str;
    }
    else if(left->type == string && right->type == number) mul_str: {
        char *new_string = memStrcpySelf(left->data.str.str, right->data.num.num);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(E_TypeException, "Get Not Support Value", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
}

void vobject_div_base(LinkValue *belong, Result *result, struct Inter *inter, VarList *var_list, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number) {
        lldiv_t div_result = lldiv(left->data.num.num, right->data.num.num);
        result->value->value = makeNumberValue(div_result.quot, inter);
    }
    else
        setResultError(E_TypeException, "Get Not Support Value", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
}

ResultType vobject_opt_core(OFFICAL_FUNCTIONSIG, base_opt func){
    Value *left = NULL;
    Value *right = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="left", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }

    left = ap[0].value->value;
    right = ap[1].value->value;

    func(belong, result, inter, var_list, left, right);

    return result->type;
}

ResultType vobject_add(OFFICAL_FUNCTIONSIG){
    return vobject_opt_core(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), vobject_add_base);
}

ResultType vobject_sub(OFFICAL_FUNCTIONSIG){
    return vobject_opt_core(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), vobject_sub_base);
}

ResultType vobject_mul(OFFICAL_FUNCTIONSIG){
    return vobject_opt_core(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), vobject_mul_base);
}

ResultType vobject_div(OFFICAL_FUNCTIONSIG){
    return vobject_opt_core(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), vobject_div_base);
}

ResultType vobject_bool(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    bool result_ = false;
    Value *value = NULL;
    Value *return_value = NULL;
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    }
    value = ap[0].value->value;
    switch (value->type) {
        case number:
            result_ = value->data.num.num != 0;
            break;
        case string:
            result_ = memStrlen(value->data.str.str) > 0;
            break;
        case bool_:
            result_ = value->data.bool_.bool_;
            break;
        case pass_:
        case none:
            result_ = false;
            break;
        case list:
            result_ = value->data.list.size > 0;
            break;
        case dict:
            result_ = value->data.dict.size > 0;
            break;
        default:
            setResultError(E_TypeException, "\"vobject.__bool__\" gets an unsupported value type",
                           0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
    }
    return_value = makeBoolValue(result_, inter);
    setResultOperationBase(result, makeLinkValue(return_value, belong, inter));
    return result->type;
}

ResultType vobject_repo(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *repo = NULL;
    Value *value = NULL;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    value = ap[0].value->value;

    switch (value->type){
        case number: {
            char str[30] = {};
            snprintf(str, 30, "%lld", value->data.num.num);
            repo = memStrcpy(str);
            break;
        }
        case string:
            repo = memStrcpy(value->data.str.str);
            break;
        case function: {
            char str[30] = {};
            snprintf(str, 30, "(function on %p)", value);
            repo = memStrcpy(str);
            break;
        }
        case none:
            repo = memStrcpy("(null)");
            break;
        case class: {
            char str[30] = {};
            snprintf(str, 30, "(class on %p)", value);
            repo = memStrcpy(str);
            break;
        }
        case bool_:
            if (value->data.bool_.bool_)
                repo = memStrcpy("true");
            else
                repo = memStrcpy("false");
            break;
        case pass_:
            repo = memStrcpy("...");
            break;
        default:
            setResultError(E_TypeException, "vobject.__repo__ gets unsupported data", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
            return error_return;
    }
    setResultOperationBase(result, makeLinkValue(makeStringValue(repo, inter), belong, inter));
    memFree(repo);
    return result->type;
}

void registeredVObject(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.vobject, inter->base_father, inter);
    NameFunc tmp[] = {{"__add__", vobject_add, object_free_},
                      {"__sub__", vobject_sub, object_free_},
                      {"__mul__", vobject_mul, object_free_},
                      {"__div__", vobject_div, object_free_},
                      {"__bool__", vobject_bool, object_free_},
                      {"__repo__", vobject_repo, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("vobject", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseVObject(Inter *inter){
    Value *vobject = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&vobject->gc_status);
    inter->data.vobject = vobject;
}
