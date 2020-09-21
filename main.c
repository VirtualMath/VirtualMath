#include "hellovm.h"

int main(int argc, char *argv[]) {
    Inter *inter = NULL;

    atexit(freeArgs);
    signal(SIGINT, SIG_IGN);
    if (setjmp(memVirtualMath_Env) == -1){
        fprintf(stderr, "ERROR: Fatal memory error encountered, May be caused by insufficient memory!\n");
        return 1;
    }
    memVirtualMathUseJmp = true;

    if (getArgs(argc, argv))
        return 2;

    inter = makeInter(args.out_file, args.error_file, args.in_file, NULL);
    runCodeFile(inter, argv + optind);
    if (args.run_commandLine)
        runCodeStdin(inter, HelloString);
    freeInter(inter, true);
    return 0;
}

/**
class A{
	def __init__(self, n) {
		self.num = n
	}

	def printNum(self) {
		print(self.num)
	}
}

a = A(5)
b = A(10)
# a.printNum(20, b;)
a.printNum(b, 20;)  // 运行到此处会内存错误

 */