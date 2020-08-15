#include "virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *global_inter = NULL;
    int status = 1;
    memVirtualMathUseJmp = true;
    if (setjmp(memVirtualMath_Env) == -1)  // 遇到内存错误
        return 2;

    if (getArgs(argc, argv))
        goto args_error;

    global_inter = runBaseInter(args.file, args.log_file, &status);

    freeInter(global_inter, true, true);
    args_error: freeArgs();
    return status;
}


/**
 * TODO-szh 类super语句
 * TODO-szh const声明
 * TODO-szh 生成语法树
 * TODO-szh 取反符号 -
 * TODO-szh 字面量后缀
 */
