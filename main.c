#include "__virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *inter = NULL;

    memVirtualMathUseJmp = true;
    if (setjmp(memVirtualMath_Env) == -1)  // 遇到内存错误
        return 2;

    if (getArgs(argc, argv))
        goto args_error;

    inter = makeInter(args.log_file);
    for (int status=0; status == 0 && argv[optind] != NULL; optind++)
        status = runCodeBlock(argv[optind], inter);
    freeInter(inter, true, true);

    args_error: freeArgs();
    return 0;
}


/**
 * TODO-szh 断言
 * TODO-szh 代码块
 * TODO-szh 类super语句
 * TODO-szh const声明
 * TODO-szh 生成语法树
 * TODO-szh 取反符号 -
 * TODO-szh 字面量后缀
 * TODO-szh 去掉free的self参数
 */
