#include "__ofunc.h"

ResultType object_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    LinkValue *_init_ = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "object", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
        setResultError(E_ArgumentException, MANY_ARG, 0, "object", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));

    return_:
    return result->type;
}

ResultType objectRepoStrCore(OFFICAL_FUNCTIONSIG, bool is_repo){
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    char *repo;
    char *name;
    char *type;
    size_t len;
    LinkValue *name_value;
    setResultCore(result);
    parserArgumentUnion(ap, arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
    if (!CHECK_RESULT(result))
        return result->type;
    freeResult(result);

    name_value = findAttributes(inter->data.object_name, false, ap[0].value, inter);
    if (name_value != NULL){
        gc_addTmpLink(&name_value->gc_status);
        name = getRepoStr(name_value, is_repo, 0, "sys", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        gc_freeTmpLink(&name_value->gc_status);
        if (!CHECK_RESULT(result))
            return result->type;
        freeResult(result);
    } else
        name = "unknown";

    if (ap[0].value->value->type == class)
        type = "class";
    else
        type = "object";

    len = memStrlen(name) + 26;
    repo = memCalloc(len, sizeof(char ));
    snprintf(repo, len, "(%s: %s on %p)", type, name, ap[0].value->value);
    setResultOperationBase(result, makeLinkValue(makeStringValue(repo, inter), belong, inter));
    memFree(repo);

    return result->type;
}

ResultType object_repo(OFFICAL_FUNCTIONSIG){
    return objectRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), true);
}

ResultType object_str(OFFICAL_FUNCTIONSIG){
    return objectRepoStrCore(CALL_OFFICAL_FUNCTION(arg, var_list, result, belong), false);
}

void registeredObject(REGISTERED_FUNCTIONSIG){
    LinkValue *object = makeLinkValue(inter->data.object, inter->base_father, inter);
    NameFunc tmp[] = {{inter->data.object_new,  object_new,  class_free_},
                      {inter->data.object_repo, object_repo, all_free_},
                      {inter->data.object_str,  object_str,  all_free_},
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