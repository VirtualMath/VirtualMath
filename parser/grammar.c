#include "__grammar.h"

ParserMessage *makeParserMessage(char *file_dir, char *debug){
    ParserMessage *tmp = memCalloc(1, sizeof(ParserMessage));
    tmp->tm = makeTokenMessage(file_dir, debug);
    tmp->status = success;
    tmp->status_message = NULL;
    tmp->count = 0;
#if OUT_LOG
    if (debug != NULL){
        char *debug_dir = memStrcat(debug, PASERS_LOG), *grammar_dir = memStrcat(debug, GRAMMAR_LOG);
        tmp->paser_debug = fopen(debug_dir, "w");
        tmp->grammar_debug = fopen(grammar_dir, "w");
        memFree(debug_dir);
        memFree(debug_dir);
    }
    else{
        tmp->paser_debug = NULL;
        tmp->grammar_debug = NULL;
    }
#else
    tmp->paser_debug = NULL;
    tmp->grammar_debug = NULL;
#endif
    return tmp;
}

void freePasersMessage(ParserMessage *pm, bool self) {
    freeTokenMessage(pm->tm, true);
#if OUT_LOG
    if (pm->paser_debug != NULL)
        fclose(pm->paser_debug);
    if (pm->grammar_debug != NULL)
        fclose(pm->grammar_debug);
#endif
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
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: <EOF>\n", NULL);
            Token *tmp;
            popAheadToken(tmp, pm);
            freeToken(tmp, true, false);
            goto return_;
        }
        else if (token_type == MATHER_ENTER){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: <ENTER>\n", NULL);
            Token *tmp;
            popAheadToken(tmp, pm);
            freeToken(tmp, true, false);
            continue;
        }
        else{
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: call command\n", NULL);
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
                if (global) {
                    syntaxError(pm, "ERROR from parserCommand list(get stop)", command_list_error);
                    freeToken(command_token, true, true);
                }
                else{
                    // 若非global模式, 即可以匹配大括号
                    popAheadToken(stop_token, pm);
                    backToken_(pm, stop_token);
                    connectStatement(base_st, command_token->data.st);
                    freeToken(command_token, true, false);
                    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                            "Command List: get command success[at !global end]\n", NULL);
                }
                goto return_;
            }
            /*...do something for commandList...*/
            // printf("do something for commandList\n");
            connectStatement(base_st, command_token->data.st);
            freeToken(command_token, true, false);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: get command success\n", NULL);
        }
    }
    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: return\n", NULL);
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
        PASS;
    }
    else{
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: call operation\n", NULL);
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
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: get  command success\n", NULL);
    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: get return\n", NULL);
}

/**
 * 表达式匹配
 * parserOperation：
 * | parserPolynomial
 */
void parserOperation(PASERSSIGNATURE){
    int operation_int;
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: call polynomial\n", NULL);
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
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: get polynomial success\n", NULL);

    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: return\n", NULL);
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
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Polynomial: cal base value(left)\n", NULL);
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
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                    "Polynomial: get base value(left) success[push polynomial]\n", NULL);
            continue;
        }
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Polynomial: call symbol\n", NULL);
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
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                "Polynomial: get symbol success\nPolynomial: call base value[right]\n", NULL);
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
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Polynomial: get base value(right) success[push polynomial]\n", NULL);
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
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get number\n", NULL);
        Token *value_token;
        char *stop;
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeNumberValue(strtol(value_token->data.str, &stop, 10), inter);
        freeToken(value_token, true, false);
    }
    else if(MATHER_STRING == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get string\n", NULL);
        Token *value_token;
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeStringValue(value_token->data.str, inter);
        freeToken(value_token, true, false);
    }
    else{
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: else\n", NULL);
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
    if (pm->status != success)
        return;
    pm->status = status;
    pm->status_message = memStrcpy(message, 0, false, false);
}
