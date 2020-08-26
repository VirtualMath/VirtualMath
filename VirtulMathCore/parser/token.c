#include "__virtualmath.h"

Token *makeToken(long int line) {
    Token *tmp = memCalloc(1, sizeof(Token));
    tmp->token_type = 0;
    tmp->data.str = NULL;
    tmp->data.st = NULL;
    tmp->data.second_str = NULL;
    tmp->line = line;
    tmp->next = NULL;
    return tmp;
}

Token *makeLexToken(int type, char *str, char *second_str, long int line) {
    Token *tmp = makeToken(line);
    tmp->token_type = type;
    tmp->data.str = memStrcpy(str);
    tmp->data.second_str = memStrcpy(second_str);
    return tmp;
}

Token *makeStatementToken(int type, struct Statement *st){
    Token *tmp = makeToken(st->line);
    tmp->token_type = type;
    tmp->data.st = st;
    return tmp;
}

long freeToken(Token *tk, bool free_st) {
    long int line = 0;
    FREE_BASE(tk, return_);
    line = tk->line;
    memFree(tk->data.str);
    memFree(tk->data.second_str);
    if (free_st)
        freeStatement(tk->data.st);
    memFree(tk);
    return_:
    return line;
}

TokenStream *makeTokenStream(){
    TokenStream *tmp = memCalloc(1, sizeof(TokenStream));
    tmp->size = 0;
    tmp->token_list = NULL;
    return tmp;
}

void freeToekStream(TokenStream *ts, bool free_st) {
    FREE_BASE(ts, return_);
    for (Token *tmp = ts->token_list, *tmp_next=NULL; tmp != NULL; tmp = tmp_next){
        tmp_next = tmp->next;
        freeToken(tmp, free_st);
    }
    memFree(ts);
    return_:
    return;
}

TokenMessage *makeTokenMessage(char *file_dir) {
    TokenMessage *tm = memCalloc(1, sizeof(TokenMessage));
    tm->file = makeLexFile(file_dir);
    tm->mathers = makeMathers(MATHER_MAX);
    tm->ts = makeTokenStream();
    return tm;
}

void freeTokenMessage(TokenMessage *tm, bool self, bool free_st) {
    freeLexFile(tm->file);
    freeToekStream(tm->ts, free_st);
    freeMathers(tm->mathers);
    if (self)
        free(tm);
}

/**
 * 添加一个token到token_ahend，token_list保持
 * @param ts
 * @param new_tk
 */
void addBackToken(TokenStream *ts, Token *new_tk) {
    Token *tmp = ts->token_list;
    ts->token_list = new_tk;
    new_tk->next = tmp;
    ts->size ++;
}

/**
 * 从token_ahead弹出一个token，保持token_list
 * @param ts
 * @return
 */
Token *popToken(TokenStream *ts) {
    Token *tmp = ts->token_list;
    ts->token_list = tmp->next;
    tmp->next = NULL;
    ts->size --;
    return tmp;
}

Token *popNewToken(TokenMessage *tm) {
    Token *tmp;
    if (tm->ts->size == 0)
        tmp = getToken(tm->file, tm->mathers);
    else
        tmp = popToken(tm->ts);
    return tmp;
}