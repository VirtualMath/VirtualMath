#include "hellovm.h"

int main(int argc, char *argv[]) {
    Inter *inter = NULL;
    char arr_cwd[200];
    getcwd(arr_cwd, 200);

#ifdef DEBUG
    setbuf(stdout, NULL);  // debug模式下关闭缓冲区以尽快输出信息便于debug
    setbuf(stderr, NULL);
    setbuf(stdin, NULL);
#endif

    atexit(freeArgs);
    signal(SIGINT, SIG_IGN);
    if (setjmp(memVirtualMath_Env) == -1){
        fprintf(stderr, "ERROR: Fatal memory error encountered, May be caused by insufficient memory!\n");
        return 1;
    }
    memVirtualMathUseJmp = true;

    if (getArgs(argc, argv) == -1)  // 命令行参数设定
        return 2;

    initVirtualMath(args.locale);
    inter = makeInter(args.out_file, args.error_file, args.in_file, arr_cwd, NULL);
    runCodeFile(inter, argv + optind);  // 从文件中运行代码
    if (args.run_commandLine)
        runCodeStdin(inter, HelloString);  // 从stdin中运行代码
    freeInter(inter, true);

    system("pause");  // 按任意键继续...
    return 0;
}
