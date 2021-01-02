#include "virtualmath.h"

bool initVirtualMath(char const *local) {
    setlocale(LC_ALL, local);
    signal(SIGINT, vmSignalHandler);  // 注册信号处理程序
    return true;
}
