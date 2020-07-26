#ifndef VIRTUALMATH_TOKEN_H
#define VIRTUALMATH_TOKEN_H

#define MATHER_ERROR_ -1
#define MATHER_NUMBER 0
#define MATHER_STRING 1
#define MATHER_VAR 2

#define MATHER_EOF 3
#define MATHER_ENTER 4
#define MATHER_SPACE 5

#define MATHER_IF 6
#define MATHER_ELIF 7
#define MATHER_WHILE 8
#define MATHER_FOR 9
#define MATHER_IN 10
#define MATHER_TRY 11
#define MATHER_EXCEPT 12
#define MATHER_AS 13
#define MATHER_WITH 14
#define MATHER_DO 15
#define MATHER_ELSE 16
#define MATHER_FINALLY 17
#define MATHER_DEFAULT 18
#define MATHER_GLOBAL 19
#define MATHER_NONLOCAL 20
#define MATHER_PUBLIC 21
#define MATHER_PROTECT 22
#define MATHER_PRIVATE 23
#define MATHER_TRUE 24
#define MATHER_FALSE 25
#define MATHER_NULL 26
#define MATHER_DEF 27
#define MATHER_CLASS 28
#define MATHER_BLOCK 29
#define MATHER_BREAK 30
#define MATHER_CONTINUE 31
#define MATHER_REGO 32
#define MATHER_RESTART 33
#define MATHER_RETURN 34
#define MATHER_YIELD 35
#define MATHER_IMPORT 36
#define MATHER_INCLUDE 37

#define MATHER_ADD 38
#define MATHER_SUB 39
#define MATHER_MUL 40
#define MATHER_DIV 41
#define MATHER_INTDIV 42
#define MATHER_PER 43
#define MATHER_POW 44
#define MATHER_LESS 45
#define MATHER_LESSEQ 46
#define MATHER_MORE 47
#define MATHER_MOREEQ 48
#define MATHER_EQ 49
#define MATHER_NOTEQ 50

#define MATHER_BITAND 51
#define MATHER_BITOR 52
#define MATHER_BITXOR 53
#define MATHER_BITNOT 54
#define MATHER_BITLEFT 55
#define MATHER_BITRIGHT 56

#define MATHER_BOOLAND 57
#define MATHER_BOOLOR 58
#define MATHER_BOOLNOT 59

#define MATHER_ASSIGNMENT 60
#define MATHER_POINT 61
#define MATHER_AT 62
#define MATHER_SVAR 63

#define MATHER_LP 64
#define MATHER_RP 65
#define MATHER_LB 66
#define MATHER_RB 67
#define MATHER_LC 68
#define MATHER_RC 69
#define MATHER_COMMA 70
#define MATHER_COLON 71
#define MATHER_SEMICOLON 72
#define MATHER_Link 73

#define MATHER_MAX 74

// 从-5开始是为了避开status的特殊值，尽管这并没有什么影响
#define COMMANDLIST -5
#define COMMAND -6
#define OPERATION -7
#define POLYNOMIAL -8
#define BASEVALUE -9

typedef struct Token{
    int token_type;  // 记录token的类型，大于0的数字均为lex匹配器所匹配，小于0的为syntax解析器所匹配
    struct{
        char *str;
        char *second_str;  // 针对123.4j这种形式设定的，其中second_str存储j
        struct Statement *st;
    } data;
} Token;

typedef struct TokenStream{
    Token **token_list;  // 存储token的列表
    Token **token_ahead;  // 提前存储token的列表
    int size;
    int ahead;
} TokenStream;

typedef struct TokenMessage{
    TokenStream *ts;
    struct LexFile *file;
    struct LexMathers *mathers;
    FILE *debug;
} TokenMessage;

Token *makeToken();
Token *makeLexToken(int type, char *str, char *second_str);
Token *makeStatementToken(int type, struct Statement *st);
void freeToken(Token *tk, bool self, bool error);

extern Token *getToken(LexFile *file, LexMathers *mathers, FILE *debug);


int safeGetToken(TokenMessage *tm, FILE *debug);
Token *forwardToken(TokenStream *ts, FILE *debug);
Token *backToken(TokenStream *ts, FILE *debug);
void addToken(TokenStream *ts, Token *new_tk, FILE *debug);
Token *popToken(TokenStream *ts, FILE *debug);

TokenMessage *makeTokenMessage(char *file_dir, char *debug);
void freeTokenMessage(TokenMessage *tm, bool self);

// token 可视化函数
void printTokenStream(TokenStream *ts, FILE *debug, int type);
void printToken(Token *tk, FILE *debug, int type);

#define printTokenEnter(tk, debug, type, message) do{ \
writeLog(debug, type, message, NULL); \
printToken(tk, debug, type); \
writeLog(debug, type, "\n", NULL); \
} while(0)

#endif //VIRTUALMATH_TOKEN_H