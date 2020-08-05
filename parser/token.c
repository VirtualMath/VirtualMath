#include "__token.h"

Token *makeToken(long int line) {
    Token *tmp = memCalloc(1, sizeof(Token));
    tmp->token_type = 0;
    tmp->data.str = NULL;
    tmp->data.st = NULL;
    tmp->data.second_str = NULL;
    tmp->line = line;
    tmp->next = NULL;
    tmp->last = NULL;
    return tmp;
}

Token *makeLexToken(int type, char *str, char *second_str, long int line) {
    Token *tmp = makeToken(line);
    tmp->token_type = type;
    tmp->data.str = memStrcpy(str, 0, false, false);
    tmp->data.second_str = memStrcpy(second_str, 0, false, false);
    return tmp;
}

Token *makeStatementToken(int type, struct Statement *st){
    Token *tmp = makeToken(st->line);
    tmp->token_type = type;
    tmp->data.st = st;
    return tmp;
}

long freeToken(Token *tk, bool self, bool error) {
    long int line = 0;
    freeBase(tk, return_);
    line = tk->line;
    memFree(tk->data.str);
    memFree(tk->data.second_str);
    if (error){
        freeStatement(tk->data.st);
    }
    if (self){
        memFree(tk);
    }
    return_:
    return line;
}

TokenStream *makeTokenStream(){
    TokenStream *tmp = memCalloc(1, sizeof(TokenStream));
    tmp->size = 0;
    tmp->token_list = NULL;
    return tmp;
}

void freeToekStream(TokenStream *ts, bool self, bool free_st) {
    freeBase(ts, return_);
    Token *tmp = ts->token_list;
    while (tmp != NULL){
        Token *tmp_next = tmp->next;
        freeToken(tmp, true, free_st);
        tmp = tmp_next;
    }
    if (self){
        memFree(ts);
    }
    return_:
    return;
}

TokenMessage *makeTokenMessage(char *file_dir, char *debug) {
    TokenMessage *tm = memCalloc(1, sizeof(TokenMessage));
    tm->file = makeLexFile(file_dir);
    tm->mathers = makeMathers(MATHER_MAX);
    tm->ts = makeTokenStream();
#if OUT_LOG
    if (debug != NULL){
        char *debug_dir = memStrcat(debug, LEXICAL_LOG, false);
        if (access(debug_dir, F_OK) != 0 || access(debug_dir, W_OK) == 0)
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

void freeTokenMessage(TokenMessage *tm, bool self, bool free_st) {
    freeLexFile(tm->file, true);
    freeToekStream(tm->ts, true, free_st);
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
 * 添加一个token到token_ahend，token_list保持
 * @param ts
 * @param new_tk
 */
void addBackToken(TokenStream *ts, Token *new_tk, FILE *debug) {
    printTokenEnter(new_tk, debug, DEBUG, "add Token: ");
    Token *tmp = ts->token_list;

    ts->token_list = new_tk;
    new_tk->next = tmp;
    new_tk->last = NULL;
    if (tmp != NULL)
        tmp->last = new_tk;
    ts->size ++;
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
}

/**
 * 从token_ahead弹出一个token，保持token_list
 * @param ts
 * @return
 */
Token *popToken(TokenStream *ts, FILE *debug) {
    Token *tmp = ts->token_list;
    ts->token_list = tmp->next;
    tmp->next = NULL;
    tmp->last = NULL;
    if (ts->token_list != NULL)
        ts->token_list->last = NULL;
    ts->size --;
    printTokenEnter(tmp, debug, DEBUG, "pop Token: ");
    MACRO_printTokenStream(ts, debug, DEEP_DEBUG);
    return tmp;
}

Token *popNewToken(TokenMessage *tm, FILE *debug) {
    Token *tmp;
    writeLog_(debug, DEBUG, "pop new token : ", NULL);
    if (tm->ts->size == 0){
        tmp = getToken(tm->file, tm->mathers, tm->debug);
    }
    else{
        tmp = popToken(tm->ts, debug);
    }
    writeLog_(debug, DEBUG, "get token: %ld\nnew token: ", tm->file->count);
    printToken(tmp, debug, DEBUG);
    writeLog_(debug, DEBUG, "\n", NULL);
    return tmp;
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
    Token *tmp = ts->token_list;
    int i = 0;
    while (tmp != NULL){
        if (i > 0)
            writeLog_(debug, type, "-", NULL);
        printToken(tmp, debug, type);
        tmp = tmp->next;
        i++;
    }
    writeLog_(debug, type, "\n", NULL);
}