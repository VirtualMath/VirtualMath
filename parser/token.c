#include "__token.h"

Token *makeToken(){
    Token *tmp = memCalloc(1, sizeof(Token));
    tmp->token_type = 0;
    tmp->data.str = NULL;
    tmp->data.st = NULL;
    tmp->data.second_str = NULL;
    return tmp;
}

Token *makeLexToken(int type, char *str, char *second_str) {
    struct Token *tmp = makeToken();
    tmp->token_type = type;
    tmp->data.str = memStrcpy(str, 0, false, false);
    tmp->data.second_str = memStrcpy(second_str, 0, false, false);
    return tmp;
}

Token *makeStatementToken(int type, struct Statement *st){
    struct Token *tmp = makeToken();
    tmp->token_type = type;
    tmp->data.st = st;
    return tmp;
}

void freeToken(Token *tk, bool self, bool error) {
    memFree(tk->data.str);
    memFree(tk->data.second_str);
    if (error){
        freeStatement(tk->data.st);
    }
    if (self){
        memFree(tk);
    }
}

TokenStream *makeTokenStream(){
    TokenStream *tmp = memCalloc(1, sizeof(TokenStream));
    tmp->size = 0;
    tmp->ahead = 0;
    tmp->token_list = NULL;
    tmp->token_ahead = NULL;
    return tmp;
}

void freeToekStream(TokenStream *ts, bool self) {
    for (int i=0; i < ts->size; i++){
        freeToken(ts->token_list[i], true, false);
    }
    for (int i=0; i < ts->ahead; i++){
        freeToken(ts->token_ahead[i], true, false);
    }
    memFree(ts->token_list);
    memFree(ts->token_ahead);
    if (self){
        memFree(ts);
    }
}

TokenMessage *makeTokenMessage(char *file_dir, char *debug) {
    TokenMessage *tm = memCalloc(1, sizeof(TokenMessage));
    tm->file = makeLexFile(file_dir);
    tm->mathers = makeMathers(MATHER_MAX);
    tm->ts = makeTokenStream();
#if OUT_LOG
    if (debug != NULL){
        char *debug_dir = memStrcat(debug, LEXICAL_LOG);
        tm->debug = fopen(debug_dir, "w");
        memFree(debug_dir);
    }
    else{
        tm->debug = NULL;
    }
#else
    tm->debug = NULL;
#endif
    return tm;
}

void freeTokenMessage(TokenMessage *tm, bool self) {
    freeLexFile(tm->file, true);
    freeToekStream(tm->ts, true);
    freeMathers(tm->mathers, true);
#if OUT_LOG
    if (tm->debug != NULL)
        fclose(tm->debug);
#endif
    if (self){
        free(tm);
    }
}

/**
 * 添加一个token到token_list，token_ahend保持
 * @param ts
 * @param new_tk
 */
void addToken(TokenStream *ts, Token *new_tk, FILE *debug) {
    printTokenEnter(new_tk, debug, DEBUG, "add Token: ");
    Token **new_list = memCalloc(ts->size + 1, sizeof(Token *));
    for (int i=0; i < ts->size; i++){
        new_list[i] = ts->token_list[i];
    }
    new_list[ts->size] = new_tk;
    ts->size ++;
    memFree(ts->token_list);
    ts->token_list = new_list;
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
}

/**
 * 从token_list弹出一个token，保持token_ahend
 * @param ts
 * @return
 */
Token *popToken(TokenStream *ts, FILE *debug) {
    Token **new_list = memCalloc(ts->size - 1, sizeof(Token *));
    for (int i=0; i < ts->size - 1; i++){
        new_list[i] = ts->token_list[i];
    }
    Token *tmp = ts->token_list[ts->size - 1];
    memFree(ts->token_list);
    ts->token_list = new_list;
    ts->size --;
    printTokenEnter(tmp, debug, DEBUG, "pop Token: ");
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
    return tmp;
}

/**
 * 把token_list的一个token退回到token_ahend
 * @param ts
 * @return
 */
Token *backToken(TokenStream *ts, FILE *debug) {
    Token **new_list = memCalloc(ts->size - 1, sizeof(Token *));
    Token **new_ahead = memCalloc(ts->ahead + 1, sizeof(Token *));
    for (int i=0; i < ts->size - 1; i++){
        new_list[i] = ts->token_list[i];
    }
    for (int i=0; i < ts->ahead; i++){
        new_ahead[i] = ts->token_ahead[i];
    }
    new_ahead[ts->ahead] = ts->token_list[ts->size - 1];
    memFree(ts->token_list);
    memFree(ts->token_ahead);
    ts->token_ahead = new_ahead;
    ts->token_list = new_list;
    ts->size --;
    ts->ahead ++;
    printTokenEnter(new_ahead[ts->ahead - 1], debug, DEBUG, "back Token: ");
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
    return new_ahead[ts->ahead - 1];
}

/**
 * backToken的逆向操作
 * @param ts
 * @return
 */
Token *forwardToken(TokenStream *ts, FILE *debug) {
    Token **new_list = memCalloc(ts->size + 1, sizeof(Token *));
    Token **new_ahead = memCalloc(ts->ahead - 1, sizeof(Token *));
    for (int i=0; i < ts->size; i++){
        new_list[i] = ts->token_list[i];
    }
    for (int i=0; i < ts->ahead - 1; i++){
        new_ahead[i] = ts->token_ahead[i];
    }
    new_list[ts->size] = ts->token_ahead[ts->ahead - 1];
    memFree(ts->token_list);
    memFree(ts->token_ahead);
    ts->token_ahead = new_ahead;
    ts->token_list = new_list;
    ts->size ++;
    ts->ahead --;
    printTokenEnter(new_list[ts->size - 1], debug, DEBUG, "forward Token: ");
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
    return new_list[ts->size - 1];
}

/**
 * 获取token, 并且放入token_list中
 * 自动处理backToken
 * @param tm
 * @return 返回获取token的token_type
 */
int safeGetToken(TokenMessage *tm, FILE *debug) {
    writeLog_(debug, DEBUG, "safe get token : ", NULL);
    Token *tmp;
    if (tm->ts->ahead == 0){
        writeLog_(debug, DEBUG, "get token: %d\n", tm->file->count);
        tmp = getToken(tm->file, tm->mathers, tm->debug);
        addToken(tm->ts, tmp, debug);
        MACRO_printTokenStream(tm->ts, debug, DEBUG);
    }
    else{
        // forwardToken 会有详细的日志输出
        tmp = forwardToken(tm->ts, debug);
    }
    return tmp->token_type;
}


void printToken(Token *tk, FILE *debug, int type) {
    if (tk->token_type >= 0) {
        char *tmp = tk->data.str, *second_tmp = tk->data.second_str;
        if (!strcmp(tmp, "\n")) {
            tmp = "\\n";
        }
        if (!strcmp(second_tmp, "\n")) {
            second_tmp = "\\n";
        }
        if (tmp[0] == EOF) {
            tmp = "(EOF)";
        }
        writeLog_(debug, type, "<token str = ('%s','%s'), type = %d>", tmp, second_tmp, tk->token_type);
    }
    else{
        writeLog_(debug, type, "<token statement, type = %d>", tk->token_type);
    }

}

void printTokenStream(TokenStream *ts, FILE *debug, int type) {
    writeLog_(debug, type, "token_list: ", NULL);
    for (int i=0; i < ts->size; i ++){
        if (i > 0)
            writeLog_(debug, type, "-", NULL);
        printToken(ts->token_list[i], debug, type);
    }
    writeLog_(debug, type, "\n", NULL);
    writeLog_(debug, type, "token_ahead: ", NULL);
    for (int i=0; i < ts->ahead; i ++){
        if (i > 0)
            writeLog_(debug, type, "-", NULL);
        printToken(ts->token_ahead[i], debug, type);
    }
    writeLog_(debug, type, "\n", NULL);
}