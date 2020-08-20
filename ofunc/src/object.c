#include "__ofunc.h"

ResultType object_new_(OfficialFunctionSig){
    LinkValue *value = NULL;
    LinkValue *_init_ = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(result, inter, "ArgumentException", "Too less Argument", 0, "sys", belong, true);
        return error_return;
    }

    {
        VarList *new_var = NULL;
        Value *new_object = NULL;
        Argument *father_arg = makeValueArgument(ap[1].value);
        Inherit *object_father = setFather(father_arg);
        freeArgument(father_arg, true);
        new_var = copyVarList(ap[1].value->value->object.out_var, false, inter);
        new_object = makeObject(inter, NULL, new_var, object_father);
        value = makeLinkValue(new_object, belong, inter);
        setResultOperation(result, value);
    }

    _init_ = findAttributes(inter->data.object_init, false, value, inter);

    if (_init_ != NULL){
        Result _init_result;
        setResultCore(&_init_result);
        _init_->belong = value;

        gc_addTmpLink(&_init_->gc_status);
        callBackCore(_init_, arg, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, &_init_result, value));
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
    LinkValue *object = makeLinkValue(inter->data.object, inter->base_father, inter);
    VarList *object_var = object->value->object.var;
    VarList *object_backup = NULL;
    NameFunc tmp[] = {{"__new__", object_new_, class_static_}, {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("object", false, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));

    object_backup = object_var->next;
    object_var->next = inter->var_list;
    iterNameFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(object_var));
    object_var->next = object_backup;

    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter){
    Value *object = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&object->gc_status);
    inter->data.object = object;
}
