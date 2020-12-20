#include "__virtualmath.h"
volatile SignalTag signal_tag = {.signum=0, .status=signal_reset};

void signalStopInter(int signum) {
    signal_tag.status = signal_appear;
    signal_tag.signum = signum;
    signal(signum, signalStopInter);  // signalStopInter 触发后，会和信号解除绑定，因此必须再次绑定
}