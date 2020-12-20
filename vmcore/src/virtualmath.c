#include "virtualmath.h"

bool initVirtualMath(char const *local) {
    setlocale(LC_ALL, local);
    signal(SIGINT, signalStopInter);  // 注册信号处理程序
    return true;
}
