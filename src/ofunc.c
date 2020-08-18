#include "__virtualmath.h"

static Registered base_func_list[] = {registeredIOFunction,
                                      registeredObject,
                                      NULL};

void registeredBaseFunction(struct LinkValue *father, Inter *inter){
    for (Registered *list = base_func_list; *list != NULL; list++)
        (*list)(CALL_RegisteredFunction(father, inter->var_list));
}
