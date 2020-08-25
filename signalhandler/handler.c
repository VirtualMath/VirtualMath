#include "__virtualmath.h"
volatile SignalType is_KeyInterrupt = signal_reset;
volatile SignalType pm_KeyInterrupt = signal_reset;

void signalStopInter(int signum) {
    if (is_KeyInterrupt == signal_reset)
        is_KeyInterrupt = signal_appear;
    signal(signum, signalStopInter);
}

void signalStopPm(int signum) {
    if (pm_KeyInterrupt == signal_reset)
        pm_KeyInterrupt = signal_appear;
    signal(signum, signalStopPm);
}