#include "__virtualmath.h"
volatile SignalType is_KeyInterrupt;

void resetHandler() {
    is_KeyInterrupt = signal_reset;
}
