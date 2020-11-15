#include "__virtualmath.h"

#define CHECK_CLIB(path, dl) ((dl = dlopen(path, RTLD_NOW)) != NULL) && (dlsym(dl, "registered") != NULL)

bool checkCLib(char **file) {
    char *lib_file = strncmp(*file, "libvm", 5) == 0 ? memStrcpy(*file) : memStrcatIter("libvm", false, *file, ".so", NULL);
    bool return_ = false;
    void *tmp_dl;

    char arr_cwd[200];
    char *p_cwd = NULL;
    getcwd(arr_cwd, 200);
    p_cwd = memStrcatIter(arr_cwd, false, SEP, lib_file, NULL);  // 以NULL结尾表示结束
    if (CHECK_CLIB(p_cwd, tmp_dl)) {
        return_ = true;
        dlclose(tmp_dl);
    }
    memFree(p_cwd);


    if (!return_) {
        char *path = memStrcpy(getenv("VIRTUALMATHPATH"));
        for (char *tmp = strtok(path, ";"), *new_dir; tmp != NULL; tmp = strtok(NULL, ";")) {
            if (*(tmp + (memStrlen(tmp) - 1)) != SEP_CH)
                new_dir = memStrcatIter(tmp, false, SEP, lib_file, NULL);  // 以NULL结尾表示结束
            else
                new_dir = memStrcat(tmp, lib_file, false, false);

            if (CHECK_CLIB(new_dir, tmp_dl)) {
                return_ = true;
                dlclose(tmp_dl);
            }
            memFree(new_dir);
        }
        memFree(path);
    }

    if (return_) {
        memFree(*file);
        *file = lib_file;
    } else
        memFree(lib_file);
    return return_;
}
#undef CHECK_CLIB

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
