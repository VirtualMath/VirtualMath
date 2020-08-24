#include "__virtualmath.h"
volatile bool is_KeyInterrupt = false;
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
    for (ResultType status = not_return; status != error_return && argv[optind] != NULL; optind++)
        status = runCodeBlock(argv[optind], inter);
    if (args.run_commandLine)
        runCodeStdin(inter);
    freeInter(inter, true);

    args_error: freeArgs();
    return 0;
}


void signalStop(int signum) {
    is_KeyInterrupt = true;
}

/** TODO-szh List
 * 官方函数
 * 官方类
 */