#include "__virtualmath.h"
void signalStop(int signum);

int main(int argc, char *argv[]) {
    Inter *inter = NULL;
    signal(SIGINT, signalStop);
    if (setjmp(memVirtualMath_Env) == -1){
        fprintf(stderr, "ERROR: Fatal memory error encountered, May be caused by insufficient memory!\n");
        return 1;
    }
    memVirtualMathUseJmp = true;

    if (getArgs(argc, argv))
        goto args_error;

    inter = makeInter(args.out_file, args.error_file, args.in_file, NULL);
    runCodeFile(inter, argv + optind);
    if (args.run_commandLine)
        runCodeStdin(inter);
    freeInter(inter, true);

    args_error: freeArgs();
    return 0;
}


void signalStop(int signum) {
    if (is_KeyInterrupt == signal_reset)
        is_KeyInterrupt = signal_appear;
}

/** TODO-szh List
 * 官方函数
 * 官方类
 */