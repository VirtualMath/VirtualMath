#include "virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *global_inter = NULL;
    int status = 1;

    if (getArgs(argc, argv))
        goto args_error_;

    global_inter = runBaseInter(args.file, args.log_file, &status);

    freeInter(global_inter, true);
    args_error_: freeArgs();
    return status;
}


/**
 * TODO-szh 面向对象
 * TODO-szh import和include语句
 * TODO-szh 错误回溯
 * TODO-szh 检查文件是否为目录和目录是否为目录
 * TODO-szh 生成语法树
 */
