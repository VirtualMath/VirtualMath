#include "__ofunc.h"

ResultType object_new(OFFICAL_FUNCTIONSIG){
    LinkValue *value = NULL;
    setResultCore(result);
    ArgumentParser ap[] = {{.type=only_value, .must=1, .long_arg=false},
                           {.must=-1}};
    int status = 1;
    arg = parserValueArgument(ap, arg, &status, NULL);
    if (status != 1){
        setResultError(E_ArgumentException, FEW_ARG, 0, "object.new", true, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
        return error_return;
    }

    value = make_new(inter, belong, ap[0].value);
    switch (init_new(value, arg, "object.new", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong))) {
        case 1:
            freeResult(result);
            setResultOperation(result, value);
            break;
        default:
            break;
    }
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

    len = memStrlen(name) + 30;
    repo = memCalloc(len, sizeof(char ));
    snprintf(repo, len, "(%s: %s on %p)", type, name, ap[0].value->value);
    makeStringValue(repo, 0, "object.repo", CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong));
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
    addBaseClassVar("object", object, belong, inter);
    iterBaseClassFunc(tmp, object, CALL_INTER_FUNCTIONSIG_CORE(inter->var_list));
    gc_freeTmpLink(&object->gc_status);
}

void makeBaseObject(Inter *inter){
    Value *object = makeClassValue(inter->var_list, inter, NULL);
    gc_addStatementLink(&object->gc_status);
    inter->data.object = object;
}
