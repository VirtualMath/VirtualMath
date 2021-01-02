#ifndef VIRTUALMATH_HANDLER_H
#define VIRTUALMATH_HANDLER_H

struct SignalTag{
    volatile vsignal signum;  // 信号
    volatile enum SignalType{
        signal_reset=0,  // 没有信号
        signal_appear,  // 信号未被处理
    } status;
};

typedef struct SignalTag SignalTag;
extern volatile SignalTag signal_tag;

void vmSignalHandler(int signum);;

#endif //VIRTUALMATH_HANDLER_H
