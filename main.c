#include "__virtualmath.h"

jmp_buf ctrl_c;
bool set_ctrl_c = false;
void sighandler(int signum);

int main(int argc, char *argv[]) {
    Inter *inter = NULL;
    if (getArgs(argc, argv))
        goto args_error;

    inter = makeInter(args.out_file, args.error_file, NULL);

    if (setjmp(ctrl_c) == -1 || setjmp(memVirtualMath_Env) == -1) {
        freeArgs();
        freeInter(inter, true);
        return 1;
    }
    memVirtualMathUseJmp = true;
    set_ctrl_c = true;
    signal(SIGINT, sighandler);

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


void sighandler(int signum) {
    if (set_ctrl_c)
        longjmp(ctrl_c, -1);
    else
        exit(1);
}

/** TODO-szh List
 * 官方函数
 * 官方类
 */