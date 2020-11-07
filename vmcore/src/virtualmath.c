#include "virtualmath.h"

bool initVirtualMath(char const *local) {
    setlocale(LC_ALL, local);
    return true;
}
