#include "__virtualmath.h"

/**
 *
 * @param dir 文件地址
 * @return 0-错误, 1-普通文件, 2-目录
 */
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