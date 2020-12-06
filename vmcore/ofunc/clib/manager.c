#include "__virtualmath.h"

void importClibCore(char *file, struct LinkValue *belong, FUNC_CORE){
    void *dl;
    Registered reg;
    if (file != NULL) {
        dl = dlopen(file, RTLD_NOW);
        reg = dlsym(dl, "registered");
        if (reg != NULL) {
            reg(CR_FUNC(belong, var_list));
            makeClibInfoToInter(dl, inter);
        } else
            dlclose(dl);
    }
}
