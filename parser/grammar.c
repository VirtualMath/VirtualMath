#include "__virtualmath.h"

#define readBackToken(status, pm) do{ \
status = safeGetToken(pm->tm); \
backToken(pm->tm->ts); \
} while(0) /*预读token*/

#define popAheadToken(token_var, pm) do{ \
safeGetToken(pm->tm); \
token_var = popToken(pm->tm->ts); \
} while(0) /*弹出预读的token*/

#define addStatementToken(type, st, pm) do{\
Token *tmp_new_token; \
tmp_new_token = makeStatementToken(type, st); \
addToken(pm->tm->ts, tmp_new_token); \
backToken(pm->tm->ts); \
} while(0)

#define backToken_(pm, token) do{ \
addToken(pm->tm->ts, token); \
backToken(pm->tm->ts); \
}while(0)

#define call_success(pm) (pm->status == success)

void parserCommand(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void parserPolynomial(PASERSSIGNATURE);
void parserBaseValue(PASERSSIGNATURE);

void syntaxError(ParserMessage *pm, char *message, int status);

ParserMessage *makeParserMessage(char *file_dir){
    ParserMessage *tmp = memCalloc(1, sizeof(ParserMessage));
    tmp->tm = makeTokenMessage(file_dir);
    tmp->status = success;
    tmp->status_message = NULL;
    return tmp;
}

void freePasersMessage(ParserMessage *pm, bool self) {
    freeTokenMessage(pm->tm, true);
    memFree(pm->status_message);
    if (self){
        memFree(pm);
    }
}

/**
 * 命令表匹配
 * pasersCommandList :
 * | MATHER_EOF
 * | parserCommand MATHER_ENTER
 * | parserCommand MATHER_EOF
 */
void pasersCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st) {
    int token_type, command_int, stop;
    struct Statement *base_st = st;
    while (true){
        readBackToken(token_type, pm);
        if (token_type == MATHER_EOF){
            // printf("get EOF\n");
            Token *tmp;
            popAheadToken(tmp, pm);
            freeToken(tmp, true, false);
            goto return_;
        }
        else if (token_type == MATHER_ENTER){
            // 处理空语句
            Token *tmp;
            popAheadToken(tmp, pm);
            freeToken(tmp, true, false);
            continue;
        }
        else{
            Token *command_token,*stop_token;
            parserCommand(CALLPASERSSIGNATURE);
            if (!call_success(pm)){
                goto return_;
            }
            readBackToken(command_int, pm);
            if (COMMAND != command_int){
                if (global){
                    syntaxError(pm, "ERROR from command list(get parserCommand)", command_list_error);
                }
                goto return_;
            }
            popAheadToken(command_token, pm);

            readBackToken(stop, pm);
            if (stop == MATHER_ENTER){
                popAheadToken(stop_token, pm);
                freeToken(stop_token, true, false);
            }
            else if(stop == MATHER_EOF){
                popAheadToken(stop_token, pm);
                backToken_(pm, stop_token);
            }
            else{
                syntaxError(pm, "ERROR from parserCommand list(get stop)", command_list_error);
                freeToken(command_token, true, true);
                goto return_;
            }
            /*...do something for commandList...*/
            // printf("do something for commandList\n");
            connectStatement(base_st, command_token->data.st);
            freeToken(command_token, true, false);
        }
    }
    return_:
    addStatementToken(COMMANDLIST, base_st, pm);
}

/**
 * 命令匹配
 * parserCommand：
 * | parserOperation
 */
void parserCommand(PASERSSIGNATURE){
    int token_type;
    Statement *st = NULL;
    readBackToken(token_type, pm);
    if (false){
        PASS
    }
    else{
        int command_int;
        Token *command_token;
        parserOperation(CALLPASERSSIGNATURE);
        if (!call_success(pm)){
            goto return_;
        }
        readBackToken(command_int, pm);
        if (command_int != OPERATION){
            goto return_;
        }
        popAheadToken(command_token, pm);
        /*...do something for command...*/
        // printf("do something for command\n");
        st = command_token->data.st;
        freeToken(command_token, true, false);
    }
    addStatementToken(COMMAND, st, pm);

    return_:
    return;
}

/**
 * 表达式匹配
 * parserOperation：
 * | parserPolynomial
 */
void parserOperation(PASERSSIGNATURE){
    int operation_int;
    parserPolynomial(CALLPASERSSIGNATURE);
    if (!call_success(pm)){
        goto return_;
    }
    readBackToken(operation_int, pm);
    if (operation_int != POLYNOMIAL){
        goto return_;
    }
    Token *operation_token;
    popAheadToken(operation_token, pm);
    /*...do something for operation...*/
    // printf("do something for operation\n");

    addStatementToken(OPERATION, operation_token->data.st, pm);
    freeToken(operation_token, true, false);

    return_:
    return;
}

/**
 * 多项式匹配
 * parserPolynomial：
 * | parserBaseValue [1]
 * | parserPolynomial ADD parserBaseValue
 * | parserPolynomial SUB parserBaseValue
 */
void parserPolynomial(PASERSSIGNATURE){
    while(true){
        int left, symbol, right;
        Token *left_token, *symbol_token, *right_token;
        struct Statement *st = NULL;
        readBackToken(left, pm);
        if (left != POLYNOMIAL){
            // 情况[1]
            parserBaseValue(CALLPASERSSIGNATURE);  // 获得左值
            if (!call_success(pm)){
                goto return_;
            }
            readBackToken(left, pm);
            if (left != BASEVALUE){  // 若非正确数值
                goto return_;
            }
            popAheadToken(left_token, pm);
            addStatementToken(POLYNOMIAL, left_token->data.st, pm);
            freeToken(left_token, true, false);
            continue;
            // printf("polynomial: get base value\n");
        }
        popAheadToken(left_token, pm);
        readBackToken(symbol, pm);
        switch (symbol) {
            case MATHER_ADD:
                // printf("polynomial: get a add symbol\n");
                popAheadToken(symbol_token, pm);
                freeToken(symbol_token, true, false);
                symbol_token = NULL;

                st = makeStatement();
                st->type = operation;
                st->u.operation.OperationType = ADD;
                break;
            case MATHER_SUB:
                // printf("polynomial: get a sub symbol\n");
                popAheadToken(symbol_token, pm);
                freeToken(symbol_token, true, false);
                symbol_token = NULL;

                st = makeStatement();
                st->type = operation;
                st->u.operation.OperationType = SUB;
                break;
            default:
                // printf("polynomial: get another symbol\n");
                backToken_(pm, left_token);
                goto return_;
        }

        parserBaseValue(CALLPASERSSIGNATURE);  // 获得左值
        if (!call_success(pm)){
            freeToken(left_token, true, false);
            freeStatement(st);
            goto return_;
        }
        readBackToken(right, pm);
        if (right != BASEVALUE){  // 若非正确数值
            syntaxError(pm, "ERROR from parserPolynomial(get right)", syntax_error);
            freeToken(left_token, true, true);
            freeStatement(st);
            goto return_;
        }
        popAheadToken(right_token, pm);

        st->u.operation.left = left_token->data.st;
        st->u.operation.right = right_token->data.st;

        freeToken(left_token, true, false);
        freeToken(right_token, true, false);
        addStatementToken(POLYNOMIAL, st, pm);
        // printf("polynomial: push token\n");
    }
    return_:
    return;
}

/**
 * 字面量匹配
 * parserBaseValue：
 * | MATHER_NUMBER
 * | MATHER_STRING
 */
void parserBaseValue(PASERSSIGNATURE){
    int token_type;
    struct Statement *st = NULL;
    readBackToken(token_type, pm);
    if(MATHER_NUMBER == token_type){
        // 匹配到正常字面量
        Token *value_token;
        char *stop;
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeNumberValue(strtol(value_token->data.str, &stop, 10), inter);
        freeToken(value_token, true, false);
    }
    else if(MATHER_STRING == token_type){
        Token *value_token;
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeStringValue(value_token->data.str, inter);
        freeToken(value_token, true, false);
    }
    else{
        goto return_;
    }
    addStatementToken(BASEVALUE, st, pm);
    return_:
    return;
}

/**
 * syntax错误处理器
 * @param pm
 * @param message 错误信息
 * @param status 错误类型
 */
void syntaxError(ParserMessage *pm, char *message, int status){
    pm->status = status;
    pm->status_message = memStrcpy(message, 0, false, false);
}
