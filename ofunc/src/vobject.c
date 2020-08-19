#include "__ofunc.h"

typedef void (*base_opt)(LinkValue *, Result *, struct Inter *, Value *, Value *);

void vobject_add_base(LinkValue *father, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, father, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num + right->data.num.num, inter);
    else if(left->type == string && right->type == string){
        char *new_string = memStrcat(left->data.str.str, right->data.str.str, false, false);
        result->value->value = makeStringValue(new_string, inter);
        memFree(new_string);
    }
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", father, true);
}

void vobject_sub_base(LinkValue *father, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, father, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num - right->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", father, true);
}

void vobject_mul_base(LinkValue *father, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, father, inter));
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
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", father, true);
}

void vobject_div_base(LinkValue *father, Result *result, struct Inter *inter, Value *left, Value *right) {
    setResultOperationBase(result, makeLinkValue(NULL, father, inter));
    if (left->type == number && right->type == number)
        result->value->value = makeNumberValue(left->data.num.num / right->data.num.num, inter);
    else
        setResultError(result, inter, "TypeException", "Get Not Support Value", 0, "sys", father, true);
}

ResultType vobject_opt_core(OfficialFunctionSig, base_opt func){
    Value *left = NULL;
    Value *right = NULL;
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=name_value, .name="left", .must=1, .long_arg=false},
                           {.must=-1}};
    setResultCore(result);
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        if (!run_continue(result))
            return result->type;
        freeResult(result);
    }

    left = ap[0].value->value;
    right = ap[1].value->value;

    func(father, result, inter, left, right);

    return result->type;
}

ResultType vobject_add(OfficialFunctionSig){
    return vobject_opt_core(CALL_OfficialFunction(arg, var_list, result, father), vobject_add_base);
}

ResultType vobject_sub(OfficialFunctionSig){
    return vobject_opt_core(CALL_OfficialFunction(arg, var_list, result, father), vobject_sub_base);
}

ResultType vobject_mul(OfficialFunctionSig){
    return vobject_opt_core(CALL_OfficialFunction(arg, var_list, result, father), vobject_mul_base);
}

ResultType vobject_div(OfficialFunctionSig){
    return vobject_opt_core(CALL_OfficialFunction(arg, var_list, result, father), vobject_div_base);
}

void registeredVObject(RegisteredFunctionSig){
    LinkValue *object = makeLinkValue(inter->data.vobject, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__add__", vobject_add, object_free_},
                      {"__sub__", vobject_sub, object_free_},
                      {"__mul__", vobject_mul, object_free_},
                      {"__div__", vobject_div, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("vobject", false, object, father, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

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
