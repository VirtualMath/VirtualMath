#ifndef VIRTUALMATH_TOKEN_H
#define VIRTUALMATH_TOKEN_H

#include <__macro.h>

#define MATHER_NUMBER (0)
#define MATHER_STRING (1)
#define MATHER_VAR (2)

#define MATHER_EOF (3)
#define MATHER_ENTER (4)
#define MATHER_SPACE (5)

#define MATHER_IF (6)
#define MATHER_ELIF (7)
#define MATHER_WHILE (8)
#define MATHER_FOR (9)
#define MATHER_IN (10)
#define MATHER_TRY (11)
#define MATHER_EXCEPT (12)
#define MATHER_AS (13)
#define MATHER_WITH (14)
#define MATHER_DO (15)
#define MATHER_ELSE (16)
#define MATHER_FINALLY (17)
#define MATHER_DEFAULT (18)
#define MATHER_GLOBAL (19)
#define MATHER_NONLOCAL (20)
#define MATHER_PUBLIC (21)
#define MATHER_PROTECT (22)
#define MATHER_PRIVATE (23)
#define MATHER_TRUE (24)
#define MATHER_FALSE (25)
#define MATHER_NULL (26)
#define MATHER_DEF (27)
#define MATHER_CLASS (28)
#define MATHER_BLOCK (29)
#define MATHER_BREAK (30)
#define MATHER_CONTINUE (31)
#define MATHER_REGO (32)
#define MATHER_RESTART (33)
#define MATHER_RETURN (34)
#define MATHER_YIELD (35)
#define MATHER_IMPORT (36)
#define MATHER_INCLUDE (37)

#define MATHER_ADD (38)
#define MATHER_SUB (39)
#define MATHER_MUL (40)
#define MATHER_DIV (41)
#define MATHER_INTDIV (42)
#define MATHER_PER (43)
#define MATHER_POW (44)
#define MATHER_LESS (45)
#define MATHER_LESSEQ (46)
#define MATHER_MORE (47)
#define MATHER_MOREEQ (48)
#define MATHER_EQ (49)
#define MATHER_NOTEQ (50)

#define MATHER_BITAND (51)
#define MATHER_BITOR (52)
#define MATHER_BITXOR (53)
#define MATHER_BITNOT (54)
#define MATHER_BITLEFT (55)
#define MATHER_BITRIGHT (56)

#define MATHER_BOOLAND (57)
#define MATHER_BOOLOR (58)
#define MATHER_BOOLNOT (59)

#define MATHER_ASSIGNMENT (60)
#define MATHER_POINT (61)
#define MATHER_AT (62)
#define MATHER_SVAR (63)

#define MATHER_LP (64)
#define MATHER_RP (65)
#define MATHER_LB (66)
#define MATHER_RB (67)
#define MATHER_LC (68)
#define MATHER_RC (69)
#define MATHER_COMMA (70)
#define MATHER_COLON (71)
#define MATHER_SEMICOLON (72)
#define MATHER_LINK (73)
#define MATHER_RAISE (74)
#define MATHER_FROM (75)
#define MATHER_ASSERT (76)
#define MATHER_LAMBDA (77)
#define MATHER_NOTENTER (78)
#define MATHER_COMMENT (79)
#define MATHER_GOTO (80)
#define MATHER_LABEL (81)
#define MATHER_PASSVALUE (82)
#define MATHER_DEL (83)

#define MATHER_MAX (84)

// 从-6开始是为了避开status的特殊值，尽管这并没有什么影响
#define T_COMMAND (-6)
#define T_OPERATION (-7)
#define T_POLYNOMIAL (-8)
#define T_FACTOR (-9)
#define T_BASEVALUE (-10)
#define T_ASSIGNMENT (-11)
#define T_CODE (-12)
#define T_FUNCTION (-13)
#define T_CALLFUNC (-14)
#define T_IF_BRANCH (-15)
#define T_WHILE_BRANCH (-16)
#define T_BREAK (-17)
#define T_CONTINUE (-18)
#define T_REGO (-19)
#define T_RETURN (-20)
#define T_RESTART (-21)
#define T_TRY_BRANCH (-22)
#define T_RAISE (-23)
#define T_TUPLE (-24)
#define T_INCLUDE (-25)
#define T_YIELD (-26)
#define T_IMPORT (-27)
#define T_VARCONTROL (-28)
#define T_ASSERT (-29)
#define T_DO_BRANCH (-30)
#define T_WITH_BRANCH (-31)
#define T_GOTO (-32)
#define T_LABEL (-33)
#define T_DECORATION (-34)
#define T_FOR_BRANCH (-35)
#define T_DEL (-36)
#define T_POW (-37)
#define T_BITMOVE (-38)
#define T_COMPARE (-39)
#define T_COMPARE2 (-40)

#define T_BAND (-41)
#define T_BOR (-42)
#define T_BXOR (-43)
#define T_AND (-44)
#define T_OR (-45)

#define T_NOT (-46)
#define T_NEGATE (-47)

struct Token{
    int token_type;  // 记录token的类型，大于0的数字均为lex匹配器所匹配，小于0的为syntax解析器所匹配
    fline line;
    struct TokenData{
        wchar_t *str;
        wchar_t *second_str;  // 针对123.4j这种形式设定的，其中second_str存储j
        struct Statement *st;
    } data;
    struct Token *next;
};

struct TokenStream{
    struct Token *token_list;  // 提前存储token的列表
    unsigned size;
};

struct TokenMessage{
    struct TokenStream *ts;
    struct LexFile *file;
    struct LexMathers *mathers;
};

typedef struct LexFile LexFile;
typedef struct LexMathers LexMathers;
typedef struct Token Token;
typedef struct TokenStream TokenStream;
typedef struct TokenMessage TokenMessage;

TokenMessage *makeTokenMessageFile(char *file_dir);
TokenMessage *makeTokenMessageStr(wchar_t *str);
void freeTokenMessage(TokenMessage *tm, bool self, bool free_st);

Token *makeToken(long int line);
long freeToken(Token *tk, bool free_st);
Token *makeLexToken(int type, wchar_t *str, wchar_t *second_str, long int line);
Token *makeStatementToken(int type, struct Statement *st);

extern Token *getToken(LexFile *file, LexMathers *mathers);
void addBackToken(TokenStream *ts, Token *new_tk);
Token *popNewToken(TokenMessage *tm);

#endif //VIRTUALMATH_TOKEN_H