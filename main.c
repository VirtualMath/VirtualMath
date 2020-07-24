#include "__virtualmath.h"
#include "lexical.h"
#include "token.h"

#define testMain3macro(tm, message) do{ \
printf("message: %s\n", message); \
printf("token stream: \n"); \
printTokenStream(tm->ts->token_list, tm->ts->size); \
printf("token ahead: \n"); \
printTokenStream(tm->ts->token_ahead, tm->ts->ahead); \
printf("end\n"); \
}while(0)

int testMain(int argc, char *argv[]);
int testMain2(int argc, char *argv[]);
int testMain3(int argc, char *argv[]);
void printToken(token *tk);
void printTokenStream(token **tk, int max);

int main(int argc, char *argv[]) {
    testMain3(argc, argv);
    return 0;
}

int testMain3(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Too many or little argc\n");
    }
    tokenMessage *tm = makeTokenMessage(argv[1]);
    safeGetToken(tm);
    safeGetToken(tm);
    safeGetToken(tm);
    testMain3macro(tm, "3 times safeGetToken test");
    backToken(tm->ts);
    backToken(tm->ts);
    testMain3macro(tm, "2 times backToken test");

    forwardToken(tm->ts);
    testMain3macro(tm, "1 times forwardToken test");

    token *tmp = popToken(tm->ts);
    printToken(tmp);
    testMain3macro(tm, "1 times popToken test");

    addToken(tm->ts, tmp);
    testMain3macro(tm, "1 times addToken test");

    safeGetToken(tm);
    testMain3macro(tm, "3 times safeGetToken test");
    
    freeTokenMessage(tm, true);
    return 0;
}

int testMain2(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Too many or little argc\n");
    }
    tokenMessage *tm = makeTokenMessage(argv[1]);
    int tmp;
    while (true){
        tmp = safeGetToken(tm);
        if (tmp == MATHER_EOF){
            break;
        }
    }
    printTokenStream(tm->ts->token_list, tm->ts->size);
    freeTokenMessage(tm, true);
    return 0;
}
/**
 * 用于测试的主函数程序
 * 需要一个命令行参数，指定一个vm文件用于解析
 * @param argc
 * @param argv
 * @return
 */
int testMain(int argc, char *argv[]){
    if (argc != 2){
        printf("Too many or little argc\n");
    }

    lexFile *file = makeLexFile(argv[1]);
    lexMathers *mathers = makeMathers(MATHER_MAX);
    token *tmp;
    while (true){
       tmp = getToken(file, mathers);
       if (tmp->token_type == MATHER_EOF){
           freeToken(tmp, true);
           break;
       }
       printToken(tmp);
       freeToken(tmp, true);
    }
    freeMathers(mathers, true);
    freeLexFile(file, true);
    return 0;
}

void printToken(token *tk){
    char *tmp = tk->data.str, *second_tmp = tk->data.second_str;
    if (!strcmp(tmp, "\n")){
        tmp = "\\n";
    }
    if (!strcmp(second_tmp, "\n")){
        second_tmp = "\\n";
    }
    if (tmp[0] == EOF){
        tmp = "(EOF)";
    }
    printf("<token str = ('%s','%s'), type = %d>\n", tmp, second_tmp, tk->token_type);
}

void printTokenStream(token **tk, int max){
    for (int i=0; i < max; i ++){
        printToken(tk[i]);
    }
}
