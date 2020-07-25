#include "__virtualmath.h"

token *makeToken(){
    token *tmp = memCalloc(1, sizeof(token));
    tmp->token_type = 0;
    tmp->data.str = NULL;
    tmp->data.st = NULL;
    tmp->data.second_str = NULL;
    return tmp;
}

token *makeLexToken(int type, char *str, char *second_str) {
    struct token *tmp = makeToken();
    tmp->token_type = type;
    tmp->data.str = memStrcpy(str, 0, false, false);
    tmp->data.second_str = memStrcpy(second_str, 0, false, false);
    return tmp;
}

token *makeStatementToken(int type, struct Statement *st){
    struct token *tmp = makeToken();
    tmp->token_type = type;
    tmp->data.st = st;
    return tmp;
}

void freeToken(token *tk, bool self, bool error) {
    memFree(tk->data.str);
    memFree(tk->data.second_str);
    if (error){
        freeStatement(tk->data.st);
    }
    if (self){
        memFree(tk);
    }
}

tokenStream *makeTokenStream(){
    tokenStream *tmp = memCalloc(1, sizeof(tokenStream));
    tmp->size = 0;
    tmp->ahead = 0;
    tmp->token_list = NULL;
    tmp->token_ahead = NULL;
    return tmp;
}

void freeToekStream(tokenStream *ts, bool self) {
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

tokenMessage *makeTokenMessage(char *file_dir){
    tokenMessage *tm = memCalloc(1, sizeof(tokenMessage));
    tm->file = makeLexFile(file_dir);
    tm->mathers = makeMathers(MATHER_MAX);
    tm->ts = makeTokenStream();
    return tm;
}

void freeTokenMessage(tokenMessage *tm, bool self) {
    freeLexFile(tm->file, true);
    freeToekStream(tm->ts, true);
    freeMathers(tm->mathers, true);
    if (self){
        free(tm);
    }
}

void addToken(tokenStream *ts, token *new_tk){
    token **new_list = memCalloc(ts->size + 1, sizeof(token *));
    for (int i=0; i < ts->size; i++){
        new_list[i] = ts->token_list[i];
    }
    new_list[ts->size] = new_tk;
    ts->size ++;
    memFree(ts->token_list);
    ts->token_list = new_list;
}

token *popToken(tokenStream *ts){
    token **new_list = memCalloc(ts->size - 1, sizeof(token *));
    for (int i=0; i < ts->size - 1; i++){
        new_list[i] = ts->token_list[i];
    }
    token *tmp = ts->token_list[ts->size - 1];
    memFree(ts->token_list);
    ts->token_list = new_list;
    ts->size --;
    return tmp;
}

token *backToken(tokenStream *ts){
    token **new_list = memCalloc(ts->size - 1, sizeof(token *));
    token **new_ahead = memCalloc(ts->ahead + 1, sizeof(token *));
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

token *forwardToken(tokenStream *ts){
    token **new_list = memCalloc(ts->size + 1, sizeof(token *));
    token **new_ahead = memCalloc(ts->ahead - 1, sizeof(token *));
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
 * 获取token
 * @param tm
 * @return 返回获取token的token_type
 */
int safeGetToken(tokenMessage *tm){
    token *tmp;
    if (tm->ts->ahead == 0){
        tmp = getToken(tm->file, tm->mathers);
        addToken(tm->ts, tmp);
    }
    else{
        tmp = forwardToken(tm->ts);
    }
    return tmp->token_type;
}