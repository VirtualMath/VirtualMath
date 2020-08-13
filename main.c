#include "virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *global_inter = NULL;
    int status = 1;

    if (getArgs(argc, argv))
        goto args_error;

    global_inter = runBaseInter(args.file, args.log_file, &status);

    freeInter(global_inter, true);
    args_error: freeArgs();
    return status;
}


/**
 * TODO-szh 面向对象
 * TODO-szh import语句
 * TODO-szh 生成语法树
 * TODO-szh 取反符号 -
 * TODO-szh 字面量后缀
 */
