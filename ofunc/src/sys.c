#include "__ofunc.h"

ResultType vm_super(OfficialFunctionSig){
    Value *arg_father = NULL;
    Value *arg_child = NULL;
    LinkValue *next_father = NULL;
    setResultCore(result);

    arg = arg->next->next;
    if (arg != NULL && arg->next != NULL){
        arg_father = arg->data.value->value;
        arg_child = arg->next->data.value->value;
        if (arg_child == arg_father) {
            if (arg_child->object.father != NULL){
                result->value = copyLinkValue(arg_child->object.father->value, inter);
                result->type = operation_return;
                gc_addTmpLink(&result->value->gc_status);
            } else
                setResultError(result, inter, "SuperException", "Don't get next father", 0, "sys", father, true);
            return result->type
        }
    } else{
        setResultError(result, inter, "ArgumentException", "Don't get Enough Argument", 0, "sys", father, true);
        return error_return;
    }

    for (FatherValue *self_father = arg_child->object.father; self_father != NULL; self_father = self_father->next) {
        if (self_father->value->value == arg_father) {
            if (self_father->next != NULL)
                next_father = copyLinkValue(self_father->next->value, inter);
            break;
        }
    }

    if (next_father != NULL){
        result->value = next_father;
        result->type = operation_return;
        gc_addTmpLink(&result->value->gc_status);
    }
    else
        setResultError(result, inter, "SuperException", "Don't get next father", 0, "sys", father, true);

    return result->type;
}

void registeredSysFunction(RegisteredFunctionSig){
    NameFunc tmp[] = {{"super", vm_super}, {NULL, NULL}};
    iterNameFunc(tmp, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
