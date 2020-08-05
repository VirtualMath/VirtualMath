#include "__virtualmath.h"

int checkFile(char *dir){
    struct stat my_stat;
    int status = stat(dir, &my_stat);
    if (status != 0)
        return 0;
    else if (S_ISREG(my_stat.st_mode))
        return 1;
    else if (S_ISDIR(my_stat.st_mode))
        return 2;
    else
        return 3;
}