#include "virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *global_inter = NULL;

    if (getArgs(argc, argv))
        goto args_error_;

    global_inter = runBaseInter(args.file, args.log_file);

    freeInter(global_inter, true);
    freeArgs();
    return 0;

    args_error_:
    freeArgs();
    return 1;
}


/**
 * TODO-szh 使用inter的Data成员
 * TODO-szh 面向对象
 * TODO-szh import和include语句
 * TODO-szh 检查文件是否存在
 */
