#ifndef VIRTUALMATH_HANDLER_H
#define VIRTUALMATH_HANDLER_H

enum SignalType{
    signal_reset,  // 没有信号
    signal_check,  // 信号已经被处理, 等待异步处理
    signal_appear,  // 信号未被处理
};

typedef enum SignalType SignalType;
extern volatile SignalType is_KeyInterrupt;
extern volatile SignalType pm_KeyInterrupt;

void signalStopInter(int signum);;
void signalStopPm(int signum);

#endif //VIRTUALMATH_HANDLER_H
