#include "__virtualmath.h"

/**
 * @param dir 文件地址
 * @return 0-错误, 1-普通文件, 2-目录
 */
int checkFileReadble(char *dir){
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

#ifdef __unix__
    if (dir[memStrlen(dir) - 1] == '/')
#else
    if (dir[memStrlen(dir) - 1] == '\\')
#endif  // __unix__
    { dir[memStrlen(dir) - 1] = NUL; }

#ifdef __unix__
    if ((slash = strrchr(dir, '/'))  == NULL)
#else
        if ((slash = strchr(dir, '\\'))  == NULL)
#endif  // __unix__
    { slash = dir; }
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
