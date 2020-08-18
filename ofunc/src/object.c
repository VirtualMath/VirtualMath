#include "__ofunc.h"

ResultType object_new_(OfficialFunctionSig){
    LinkValue *value = NULL;
    LinkValue *_init_ = NULL;
    setResultCore(result);

    {
        VarList *new_var = NULL;
        Value *new_object = NULL;
        Argument *father_arg = makeValueArgument(arg->next->data.value);
        FatherValue *object_father = setFather(father_arg);
        freeArgument(father_arg, true);
        new_var = copyVarList(arg->next->data.value->value->object.out_var, false, inter);
        new_object = makeObject(inter, NULL, new_var, object_father);
        value = makeLinkValue(new_object, father, inter);
        setResultOperation(result, value);
    }

    char *init_name = setStrVarName(inter->data.object_init, false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    _init_ = findFromVarList(init_name, 0, false, CALL_INTER_FUNCTIONSIG_CORE(value->value->object.var));
    memFree(init_name);

    if (_init_ != NULL){
        Result _init_result;
        Argument *init_arg = arg->next->next;
        setResultCore(&_init_result);
        _init_->father = value;

        gc_addTmpLink(&_init_->gc_status);
        callBackCore(_init_, init_arg, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &_init_result, value));
        gc_freeTmpLink(&_init_->gc_status);
        if (!run_continue_type(_init_result.type)){
            freeResult(result);
            *result = _init_result;
            goto return_;
        }
        freeResult(&_init_result);
    }

    return_:
    return result->type;
}

void registeredObject(RegisteredFunctionSig){
    Value *object = inter->data.object;
    VarList *object_var = object->object.var;
    NameFunc tmp[] = {{"__new__", object_new_}, {NULL, NULL}};
    LinkValue *name_ = NULL;
    char *name = NULL;
    gc_addTmpLink(&object->gc_status);

    object_var->next = inter->var_list;
    iterNameFunc(tmp, father, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = NULL;

    name = setStrVarName("object", false, CALL_INTER_FUNCTIONSIG_CORE(var_list));
    name_ = makeLinkValue(makeStringValue(name, inter), father, inter);
    addFromVarList(name, name_, 0, makeLinkValue(object, father, inter), CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    memFree(name);
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter){
    Value *object = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&object->gc_status);
    inter->data.object = object;
}
