#include "__virtualmath.h"

/**
 * @param dir 文件地址
 * @return 0-错误, 1-普通文件, 2-目录
 */
int checkFileReadable(char *dir){
    struct stat my_stat;
    int status;
    if (dir == NULL)
        return 3;
    status = stat(dir, &my_stat);
    if (status != 0)
        return 3;
    else if (S_ISREG(my_stat.st_mode))  // 普通文件
        return 1;
    else if (S_ISDIR(my_stat.st_mode))
        return 2;
    else
        return 3;
}

char *splitDir(char * dir){
    char *slash = NULL;
    char *point = NULL;
    char *return_char = NULL;

    if (dir[memStrlen(dir) - 1] == SEP_CH)
        dir[memStrlen(dir) - 1] = NUL;

    if ((slash = strrchr(dir, SEP_CH))  == NULL)
        slash = dir;
    else
        slash ++;

    if ((point = strchr(dir, '.'))  != NULL)
        *point = 0;

    return_char = memStrcpy(slash);
    if (point != NULL)
        *point = '.';
    if (!isalpha(*return_char) && *return_char != '_')
        return_char = memStrcat("_", return_char, false, true);
    for (char *tmp = return_char; *tmp != 0;tmp++)
        if (!isalnum(*tmp) &&'_' != *tmp)
            *tmp = '_';
    return return_char;
}


char *findPath(char **path, char *env, bool need_free) {  // env 必须以 SEP 结尾
#ifdef __linux
    if (**path == SEP_CH) {
#else
    if (!(isupper(**path) && *path[1] == ':')) {
#endif
        *path = memStrcat(env, *path, false, need_free);  // 调整为相对路径模式
    }
    return *path;
}
