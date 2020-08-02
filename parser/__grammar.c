#include "__grammar.h"

inline void twoOperation(ParserMessage *pm, Inter *inter, PasersFunction callBack, GetSymbolFunction getSymbol,
                         int type, int self_type, char *call_name, char *self_name, bool is_right) {
    bool is_right_ = false;
    while(true){
        Token *left_token = NULL;
        Token *right_token = NULL;
        struct Statement *st = NULL;

        readBackToken(pm);
        if (readBackToken(pm) != self_type){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call %s(left)\n", self_name, call_name);
            if (!callChildStatement(CALLPASERSSIGNATURE, callBack, type, &st, NULL))
                goto return_;
            addStatementToken(self_type, st, pm);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                      "%s: get %s(left) success[push %s]\n", self_name, call_name, self_name);
            continue;
        }
        left_token = popAheadToken(pm);

        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call symbol\n", self_name);
        if (getSymbol(CALLPASERSSIGNATURE, readBackToken(pm), &st)){
            delToken(pm);
        }
        else{
            backToken_(pm, left_token);
            goto return_;
        }
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                  "%s: get symbol success\n%s: call %s[right]\n", self_name, self_name, call_name);

        callBack(CALLPASERSSIGNATURE);  // 获得右值
        if (!call_success(pm)){
            freeToken(left_token, true, false);
            freeStatement(st);
            goto return_;
        }
        if (readBackToken(pm) != type){  // 若非正确数值
            syntaxError(pm, syntax_error, 3, "ERROR from ", self_name, "(get right)");
            freeToken(left_token, true, true);
            freeStatement(st);
            goto return_;
        }

        right_token = popAheadToken(pm);
        addToken_(pm, setOperationFromToken(&st, left_token, right_token, self_type, is_right_));
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                  "Polynomial: get base value(right) success[push polynomial]\n", NULL);
        is_right_ = is_right;  // 第一次is_right不生效
    }
    return_:
    return;
}

inline void tailOperation(PASERSSIGNATURE, PasersFunction callBack, TailFunction tailFunction, int type, int self_type,
                          char *call_name, char *self_name){
    while(true){
        Token *left_token = NULL;
        struct Statement *st = NULL;

        if (readBackToken(pm) != self_type){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call %s(left)\n", self_name, call_name);
            if (!callChildStatement(CALLPASERSSIGNATURE, callBack, type, &st, NULL))
                goto return_;
            addStatementToken(self_type, st, pm);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                      "%s: get %s(left) success[push %s]\n", self_name, call_name, self_name);
            continue;
        }
        left_token = popAheadToken(pm);

        int tail_status = tailFunction(CALLPASERSSIGNATURE, left_token, &st);
        if (tail_status == -1){
            backToken_(pm, left_token);
            goto return_;
        }
        else if(!tail_status){
            goto error_;
        }
        addStatementToken(self_type, st, pm);
        freeToken(left_token, true, false);
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call tail success\n", self_name);
        continue;

        error_:
        freeToken(left_token, true, true);
        goto return_;
    }
    return_:
    return;
}

/**
 * syntax错误处理器
 * @param pm
 * @param message 错误信息
 * @param status 错误类型
 */
void syntaxError(ParserMessage *pm, int status, int num, ...) {
    if (pm->status != success)
        return;

    char *message = NULL;
    va_list message_args;
    if (status <= 0){
        message = "Not message";
        goto not_message;
    }
    va_start(message_args, num);
    for (int i=0; i < num; i++) {
        char *new_message;
        new_message = memStrcat(message, va_arg(message_args, char *));
        memFree(message);
        message = new_message;
    }
    va_end(message_args);

    not_message:
    pm->status = status;
    pm->status_message = message;
}

int readBackToken(ParserMessage *pm){
    writeLog(pm->grammar_debug, GRAMMAR_DEBUG, "token operation number : %d\n", pm->count);
    writeLog(pm->paser_debug, DEBUG, "\ntoken operation number : %d\n", pm->count);
    pm->count ++;
    Token *tmp = popNewToken(pm->tm, pm->paser_debug);
    if (tmp->token_type == -2){
        freeToken(tmp, true, false);
        syntaxError(pm, lexical_error, 1, "lexical make some error");
    }
    addBackToken(pm->tm->ts, tmp, pm->paser_debug);
    return tmp->token_type;
}

Token *popAheadToken(ParserMessage *pm){
    doubleLog(pm, GRAMMAR_DEBUG, DEBUG, "token operation number : %d\n", pm->count ++);
    return popNewToken(pm->tm, pm->paser_debug);
}

bool checkToken_(ParserMessage *pm, int type){
    if (readBackToken(pm) != type)
        return false;
    delToken(pm);
    return true;
}

bool commandCallControl_(PASERSSIGNATURE, MakeControlFunction callBack, int type, Statement **st, char *message){
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, message, NULL);
    Token *tmp_token = NULL;
    parserControl(CALLPASERSSIGNATURE, callBack, type);
    if (!call_success(pm) || readBackToken(pm) != type)
        return false;
    tmp_token = popAheadToken(pm);
    *st = tmp_token->data.st;
    freeToken(tmp_token, true, false);
    return true;
}

inline bool commandCallBack_(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message){
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, message, NULL);
    return callChildStatement(CALLPASERSSIGNATURE, callBack, type, st, NULL);
}

bool callParserCode(PASERSSIGNATURE, Statement **st,char *message){
    Statement *new_st = NULL;
    if(!callChildStatement(CALLPASERSSIGNATURE, parserCode, CODE, &new_st, message)){
        return false;
    }
    if (*st != NULL)
        freeStatement(*st);
    *st = new_st;
    return true;
}

bool callParserAs(PASERSSIGNATURE, Statement **st,char *message){
    if (readBackToken(pm) == MATHER_AS) {
        delToken(pm);
        return callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, st, message);
    }
    else
        *st = NULL;
    return true;
}

bool callChildToken(ParserMessage *pm, Inter *inter, PasersFunction callBack, int type, Token **tmp, char *message,
                    int error_type) {
    callBack(CALLPASERSSIGNATURE);
    if (!call_success(pm)) {
        *tmp = NULL;
        return false;
    }
    if (readBackToken(pm) != type) {
        *tmp = NULL;
        if (message != NULL)
            syntaxError(pm, error_type, 1, message);
        return false;
    }
    *tmp = popAheadToken(pm);
    return true;
}

bool callChildStatement(PASERSSIGNATURE, PasersFunction callBack, int type, Statement **st, char *message){
    Token *tmp = NULL;
    bool status = callChildToken(CALLPASERSSIGNATURE, callBack, type, &tmp, message, syntax_error);
    if (!status){
        *st = NULL;
        return false;
    }
    *st = tmp->data.st;
    freeToken(tmp, true, false);
    return true;
}
