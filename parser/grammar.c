#include "__virtualmath.h"

#define readBackToken(status, pm) do{ \
status = safeGetToken(pm->tm); \
backToken(pm->tm->ts); \
} while(0) /*预读token*/

#define popAheadToken(token_var, pm) do{ \
safeGetToken(pm->tm); \
token_var = popToken(pm->tm->ts); \
} while(0)

#define addStatementToken(type, st, pm) do{\
token *tmp_new_token; \
tmp_new_token = makeStatementToken(type, st); \
addToken(pm->tm->ts, tmp_new_token); \
backToken(pm->tm->ts); \
} while(0)

#define backToken_(pm, token) do{ \
addToken(pm->tm->ts, token); \
backToken(pm->tm->ts); \
}while(0)

#define call_success(pm) (pm->status == success)

void command(PASERSSIGNATURE);
void parserOperation(PASERSSIGNATURE);
void polynomial(PASERSSIGNATURE);
void baseValue(PASERSSIGNATURE);

void syntaxError(parserMessage *pm, char *message, enum parserMessageStatus status);

parserMessage *makeParserMessage(char *file_dir){
    parserMessage *tmp = memCalloc(1, sizeof(parserMessage));
    tmp->tm = makeTokenMessage(file_dir);
    tmp->status = success;
    tmp->status_message = NULL;
    return tmp;
}

void freePasersMessage(parserMessage *pm, bool self) {
    freeTokenMessage(pm->tm, true);
    memFree(pm->status_message);
    if (self){
        memFree(pm);
    }
}

// TODO-szh 代码重构, 匹配器函数加前缀pasers或者grammar
void commandList(parserMessage *pm, Inter *inter, bool global, Statement *st) {
    int token_type, command_int, stop;
    struct Statement *base_st = st;
    while (true){
        readBackToken(token_type, pm);
        if (token_type == MATHER_EOF){
            // printf("get EOF\n");
            goto return_;
        }
        else{
            token *command_token,*stop_token;
            command(CALLPASERSSIGNATURE);
            if (!call_success(pm)){
                goto return_;
            }
            readBackToken(command_int, pm);
            if (COMMAND != command_int){
                if (global){
                    syntaxError(pm, "ERROR from command list(get command)", command_list_error);
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
                syntaxError(pm, "ERROR from command list(get stop)", command_list_error);
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

void command(PASERSSIGNATURE){
    int token_type;
    Statement *st = NULL;
    readBackToken(token_type, pm);
    if (false){
        pass
    }
    else{
        int command_int;
        token *command_token;
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

void parserOperation(PASERSSIGNATURE){
    int operation_int;
    polynomial(CALLPASERSSIGNATURE);
    if (!call_success(pm)){
        goto return_;
    }
    readBackToken(operation_int, pm);
    if (operation_int != POLYNOMIAL){
        goto return_;
    }
    token *operation_token;
    popAheadToken(operation_token, pm);
    /*...do something for operation...*/
    // printf("do something for operation\n");

    addStatementToken(OPERATION, operation_token->data.st, pm);
    freeToken(operation_token, true, false);

    return_:
    return;
}

void polynomial(PASERSSIGNATURE){
    while(true){
        int left, symbol, right;
        token *left_token, *symbol_token, *right_token;
        struct Statement *st = NULL;
        readBackToken(left, pm);
        if (left != POLYNOMIAL){
            baseValue(CALLPASERSSIGNATURE);  // 获得左值
            if (!call_success(pm)){
                goto return_;
            }
            readBackToken(left, pm);
            if (left != BASEVALUE){  // 若非正确数值
                goto return_;
            }
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

        baseValue(CALLPASERSSIGNATURE);  // 获得左值
        if (!call_success(pm)){
            freeToken(left_token, true, false);
            freeStatement(st);
            goto return_;
        }
        readBackToken(right, pm);
        if (right != BASEVALUE){  // 若非正确数值
            syntaxError(pm, "ERROR from polynomial(get right)", syntax_error);
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
 * baseValue：
 * | MATHER_NUMBER
 * | MATHER_STRING
 * @param pm
 */
void baseValue(PASERSSIGNATURE){
    int token_type;
    struct Statement *st = NULL;
    readBackToken(token_type, pm);
    if(MATHER_NUMBER == token_type){
        // 匹配到正常字面量
        token *value_token;
        char *stop;
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeNumberValue(strtol(value_token->data.str, &stop, 10), inter);
        freeToken(value_token, true, false);
    }
    else if(MATHER_STRING == token_type){
        token *value_token;
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

void syntaxError(parserMessage *pm, char *message, enum parserMessageStatus status){
    pm->status = status;
    pm->status_message = memStrcpy(message, 0, false, false);
}
