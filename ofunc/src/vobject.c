#include "__ofunc.h"

typedef void (*base_opt)(LinkValue *, Result *, struct Inter *, Value *, Value *);

void vobject_add_base(LinkValue *belong, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num + right->data.num.num, inter);
    else if(left->type == string && right->type == string){
        char *new_string = memStrcat(left->data.str.str, right->data.str.str, false, false);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", belong, true);
}

void vobject_sub_base(LinkValue *belong, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num - right->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", belong, true);
}

void vobject_mul_base(LinkValue *belong, Result *result, struct Inter *inter, Value *left, Value *right) {
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
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", belong, true);
}

void vobject_div_base(LinkValue *belong, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, belong, inter));
    if (left->type == number && right->type == number) {
        lldiv_t div_result = lldiv(left->data.num.num, right->data.num.num);
        result->value->value = makeNumberValue(div_result.quot, inter);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", belong, true);
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

    func(belong, result, inter, left, right);

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

void registeredVObject(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.vobject, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__add__", vobject_add, object_free_},
                      {"__sub__", vobject_sub, object_free_},
                      {"__mul__", vobject_mul, object_free_},
                      {"__div__", vobject_div, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("vobject", false, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

    object_backup = object_var->next;
    object_var->next = inter->var_list;
    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseVObject(Inter *inter){
    Value *vobject = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&vobject->gc_status);
    inter->data.vobject = vobject;
}
