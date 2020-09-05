#ifndef VIRTUALMATH_FUNCTION_H
#define VIRTUALMATH_FUNCTION_H
void registeredFunction(REGISTERED_FUNCTIONSIG);
void functionPresetting(LinkValue *func, LinkValue **func_new, LinkValue **func_init, Inter *inter);
void functionPresettingLast(LinkValue *func, LinkValue *func_new, LinkValue *func_init, Inter *inter);
void makeBaseFunction(Inter *inter);
#endif //VIRTUALMATH_FUNCTION_H