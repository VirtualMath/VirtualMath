#include "__ofunc.h"

ResultType vm_super(OfficialFunctionSig){
    Value *arg_father = NULL;
    Value *arg_child = NULL;
    LinkValue *next_father = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=name_value, .name="class_", .must=1, .long_arg=false},
                           {.type=name_value, .name="obj_", .must=1, .long_arg=false},
                           {.must=-1}};
    {
        parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father));
        if (!run_continue(result))
            return result->type;
        freeResult(result);
    }

    arg_father = ap[0].value->value;
    arg_child = ap[1].value->value;
    if (arg_child == arg_father) {
        if (arg_child->object.father != NULL){
            result->value = copyLinkValue(arg_child->object.father->value, inter);
            result->type = operation_return;
            gc_addTmpLink(&result->value->gc_status);
        } else
            setResultError(result, inter, "SuperException", "Don't get next father", 0, "sys", father, true);
        return result->type;
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
    NameFunc tmp[] = {{"super", vm_super, free_}, {NULL, NULL}};
    iterNameFunc(tmp, father, CALL_INTER_FUNCTIONSIG_CORE(var_list));
}
