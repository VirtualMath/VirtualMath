#ifndef VIRTUALMATH_OFUNC_H
#define VIRTUALMATH_OFUNC_H
#include "__macro.h"
#include "io.h"
#include "object.h"
#include "vobject.h"
#include "sys.h"
#include "int.h"
#include "dou.h"
#include "str.h"
#include "bool.h"
#include "pass.h"
#include "list.h"
#include "dict.h"
#include "function.h"
#include "listiter.h"
#include "dictiter.h"
#include "error_.h"
#include "file_.h"
#include "lib_.h"
#include "pointer.h"

struct Argument;
struct VarList;
struct Inherit;
struct Inter;

struct NameFunc{
    wchar_t *name;
    OfficialFunction of;
    enum FunctionPtType type;
    enum NameFuncVar {
        nfv_inline,
        nfv_notpush,
        nfv_default,
    } var;
};
typedef struct NameFunc NameFunc;

void registeredBaseFunction(struct LinkValue *father, Inter *inter);
void registeredFunctionName(Inter *inter, LinkValue *belong);

// 普通模式函数
LinkValue *intCore(LinkValue *belong, LinkValue *class, Inter *inter);
LinkValue *strCore(LinkValue *belong, LinkValue *class, Inter *inter);
LinkValue *boolCore(LinkValue *belong, LinkValue *class, Inter *inter);
LinkValue *douCore(LinkValue *belong, LinkValue *class, Inter *inter);
LinkValue *passCore(LinkValue *belong, LinkValue *class, Inter *inter);

#define SET_DECLARATION(NAME) void vobject_##NAME##_base(FUNC_VOBJ)
SET_DECLARATION(add);
SET_DECLARATION(sub);
SET_DECLARATION(mul);
SET_DECLARATION(div);
SET_DECLARATION(intdiv);
SET_DECLARATION(mod);
SET_DECLARATION(pow);
SET_DECLARATION(eq);
SET_DECLARATION(noteq);
SET_DECLARATION(moreeq);
SET_DECLARATION(lesseq);
SET_DECLARATION(more);
SET_DECLARATION(less);
SET_DECLARATION(band);
SET_DECLARATION(bor);
SET_DECLARATION(bxor);
SET_DECLARATION(bl);
SET_DECLARATION(br);
void vobject_bnot_base(FUNC_VOBJR);
void vobject_negate_base(FUNC_VOBJR);
#undef SET_DECLARATION

#endif //VIRTUALMATH_OFUNC_H
