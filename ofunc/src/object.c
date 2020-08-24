#include "__ofunc.h"

ResultType object_new_(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    LinkValue *_init_ = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, "Too less Argument", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    {
        Inherit *object_father = getInheritFromValueCore(ap[0].value);
        VarList *new_var = copyVarList(ap[0].value->value->object.out_var, false, inter);
        Value *new_object = makeObject(inter, NULL, new_var, object_father);
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
        if (!RUN_TYPE(_init_result.type)){
            freeResult(result);
            *result = _init_result;
            goto return_;
        }
        freeResult(&_init_result);
    } else if (arg != NULL)
        setResultError(E_ArgumentException, "Too many Exception", 0, "sys", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    return_:
    return result->type;
}

ResultType object_repo_(OFFICAL_FUNCTIONSIG){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char repo[200] = {};
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);
    snprintf(repo, 200, "(object on %p)", ap[0].value->value);
    setResultOperationBase(result, makeLinkValue(makeStringValue(repo, inter), belong, inter));
    return result->type;
}

void registeredObject(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.object, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_new, object_new_, class_free_},
                      {inter->data.object_repo, object_repo_, object_free_},
                      {NULL, NULL}};
    gc_addTmpLink(&object->gc_status);
    addStrVar("object", false, true, object, belong, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    iterClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter){
    Value *object = makeClassValue(copyVarList(inter->var_list, false, inter), inter, NULL);
    gc_addStatementLink(&object->gc_status);
    inter->data.object = object;
}
