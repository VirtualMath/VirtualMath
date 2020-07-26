#include "__virtualmath.h"

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

TokenMessage *makeTokenMessage(char *file_dir){
    TokenMessage *tm = memCalloc(1, sizeof(TokenMessage));
    tm->file = makeLexFile(file_dir);
    tm->mathers = makeMathers(MATHER_MAX);
    tm->ts = makeTokenStream();
    return tm;
}

void freeTokenMessage(TokenMessage *tm, bool self) {
    freeLexFile(tm->file, true);
    freeToekStream(tm->ts, true);
    freeMathers(tm->mathers, true);
    if (self){
        free(tm);
    }
}

/**
 * 添加一个token到token_list，token_ahend保持
 * @param ts
 * @param new_tk
 */
void addToken(TokenStream *ts, Token *new_tk){
    Token **new_list = memCalloc(ts->size + 1, sizeof(Token *));
    for (int i=0; i < ts->size; i++){
        new_list[i] = ts->token_list[i];
    }
    new_list[ts->size] = new_tk;
    ts->size ++;
    memFree(ts->token_list);
    ts->token_list = new_list;
}

/**
 * 从token_list弹出一个token，保持token_ahend
 * @param ts
 * @return
 */
Token *popToken(TokenStream *ts){
    Token **new_list = memCalloc(ts->size - 1, sizeof(Token *));
    for (int i=0; i < ts->size - 1; i++){
        new_list[i] = ts->token_list[i];
    }
    Token *tmp = ts->token_list[ts->size - 1];
    memFree(ts->token_list);
    ts->token_list = new_list;
    ts->size --;
    return tmp;
}

/**
 * 把token_list的一个token退回到token_ahend
 * @param ts
 * @return
 */
Token *backToken(TokenStream *ts){
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
    return new_ahead[ts->ahead - 1];
}

/**
 * backToken的逆向操作
 * @param ts
 * @return
 */
Token *forwardToken(TokenStream *ts){
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
    return new_list[ts->size - 1];
}

/**
 * 获取token, 并且放入token_list中
 * 自动处理backToken
 * @param tm
 * @return 返回获取token的token_type
 */
int safeGetToken(TokenMessage *tm){
    Token *tmp;
    if (tm->ts->ahead == 0){
        tmp = getToken(tm->file, tm->mathers);
        addToken(tm->ts, tmp);
    }
    else{
        tmp = forwardToken(tm->ts);
    }
    return tmp->token_type;
}