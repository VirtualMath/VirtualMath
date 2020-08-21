#include "__virtualmath.h"

int main(int argc, char *argv[]) {
    Inter *inter = NULL;

    memVirtualMathUseJmp = true;
    if (setjmp(memVirtualMath_Env) == -1)  // 遇到内存错误
        return 2;

    if (getArgs(argc, argv))
        goto args_error;

    inter = makeInter(args.log_file, NULL);
    for (int status=0; status == 0 && argv[optind] != NULL; optind++)
        status = runCodeBlock(argv[optind], inter);
    freeInter(inter, true);

    args_error: freeArgs();
    return 0;
}


/** TODO-szh List
 * 官方函数
 * 官方类
 */