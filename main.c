#include "__virtualmath.h"

jmp_buf ctrlC_ENV;
bool ctrlCUseJmp = false;
void signalStop(int signum);

int main(int argc, char *argv[]) {
    Inter *inter = NULL;
    memVirtualMathUseJmp = true;
    if (setjmp(memVirtualMath_Env) == -1){
        fprintf(stderr, "ERROR: Fatal memory error encountered, May be caused by insufficient memory!\n");
        return 1;
    }

    if (getArgs(argc, argv))
        goto args_error;
    inter = makeInter(args.out_file, args.error_file, NULL);

    ctrlCUseJmp = true;
    signal(SIGINT, signalStop);
    if (setjmp(ctrlC_ENV) == -1) {
        freeArgs();
        freeInter(inter, true);
        return 2;
    }

    for (ResultType status = not_return; status != error_return && argv[optind] != NULL; optind++)
        status = runCodeBlock(argv[optind], inter);
    if (args.run_commandLine) {
        printf("%s", HelloString);
        while (1) {
            runCodeStdin(inter);
            if (ferror(stdin) || feof(stdin))
                break;
        }
    }
    freeInter(inter, true);

    args_error: freeArgs();
    return 0;
}


void signalStop(int signum) {
    if (ctrlCUseJmp)
        longjmp(ctrlC_ENV, -1);
    else
        exit(1);
}

/** TODO-szh List
 * 官方函数
 * 官方类
 */