#include "__virtualmath.h"
#include "clib.h"

static struct InFo{
    char *name;
    Registered reg;
} ManagerInFo[] = {{"sys", registeredSysFunction},
                   {NULL, NULL}};

bool checkCLib(char *file) {
    for (struct InFo *info = ManagerInFo; info->name != NULL; info++)
        if (eqString(file, info->name))
            return true;
    return false;
}

void importClibCore(char *file, struct LinkValue *belong, FUNC_CORE){
    for (struct InFo *info = ManagerInFo; info->name != NULL; info++) {
        if (eqString(file, info->name))
            info->reg(CR_FUNC(belong, var_list));
    }
}
