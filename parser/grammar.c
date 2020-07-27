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
        memFree(grammar_dir);
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
 * | parserAssignment
 */
void parserOperation(PASERSSIGNATURE){
    int operation_int;
    Token *operation_token;
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: call assignment\n", NULL);
    callChild(pm, operation_int, operation_token, parserAssignment, ASSIGNMENT, return_);
    /*...do something for operation...*/

    addStatementToken(OPERATION, operation_token->data.st, pm);
    freeToken(operation_token, true, false);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: get polynomial success\n", NULL);

    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: return\n", NULL);
}

/**
 * 赋值表达式匹配
 * parserAssignment:
 * | parserPolynomial
 * | parserAssignment ASSIGNMENT parserPolynomial
 */
bool switchAssignment(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {
        case MATHER_ASSIGNMENT:
            *st = makeOperationStatement(ASS);
            break;
        default:
            return false;
    }
    return true;
}
void parserAssignment(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserPolynomial, switchAssignment, POLYNOMIAL, ASSIGNMENT,
                        "polynomial", "assignment");
}

/**
 * 多项式匹配
 * parserPolynomial：
 * | parserBaseValue
 * | parserPolynomial ADD parserFactor
 * | parserPolynomial SUB parserFactor
 */
bool switchPolynomial(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {
        case MATHER_ADD:
            *st = makeOperationStatement(ADD);
            break;
        case MATHER_SUB:
            *st = makeOperationStatement(SUB);
            break;
        default:
            return false;
    }
    return true;
}
void parserPolynomial(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserFactor, switchPolynomial, FACTOR, POLYNOMIAL,
            "factor", "polynomial");
}

/**
 * 因式匹配
 * parserFactor：
 * | parserBaseValue [1]
 * | switchFactor ADD parserBaseValue
 * | switchFactor SUB parserBaseValue
 */
bool switchFactor(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {
        case MATHER_MUL:
            *st = makeOperationStatement(MUL);
            break;
        case MATHER_DIV:
            *st = makeOperationStatement(DIV);
            break;
        default:
            return false;
    }
    return true;
}
void parserFactor(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserBaseValue, switchFactor, BASEVALUE, FACTOR,
                        "base value", "factor");
}

/**
 * 字面量匹配
 * parserBaseValue：
 * | MATHER_NUMBER
 * | MATHER_STRING
 */
void parserBaseValue(PASERSSIGNATURE){
    int token_type;
    Token *value_token;
    struct Statement *st = NULL;
    readBackToken(token_type, pm);
    if(MATHER_NUMBER == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get number\n", NULL);
        popAheadToken(value_token, pm);
        char *stop;
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeNumberValue(strtol(value_token->data.str, &stop, 10), inter);
    }
    else if(MATHER_STRING == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get string\n", NULL);
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeStringValue(value_token->data.str, inter);
    }
    else if(MATHER_VAR == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get var\n", NULL);
        popAheadToken(value_token, pm);
        st = makeStatement();
        st->type = base_var;
        st->u.base_var.name = memStrcpy(value_token->data.str, 0, false, false);
        st->u.base_var.times = NULL;
    }
    else{
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: else\n", NULL);
        goto return_;
    }
    freeToken(value_token, true, false);
    addStatementToken(BASEVALUE, st, pm);

    return_:
    return;
}

inline void twoOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*getSymbol)(PASERSSIGNATURE, int symbol, Statement **st), int type, int self_type, char *call_name, char *self_name){
    while(true){
        int left, symbol, right;
        Token *left_token, *symbol_token, *right_token;
        struct Statement *st = NULL;

        readBackToken(left, pm);
        if (left != self_type){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call %s(left)\n", self_name, call_name);
            callChild(pm, left, left_token, callBack, type, return_);
            addStatementToken(self_type, left_token->data.st, pm);
            freeToken(left_token, true, false);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                      "%s: get %s(left) success[push %s]\n", self_name, call_name, self_name);
            continue;
        }
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call symbol\n", self_name);
        popAheadToken(left_token, pm);

        readBackToken(symbol, pm);
        if (getSymbol(CALLPASERSSIGNATURE, symbol, &st)){
            delToken(pm);
        }
        else{
            backToken_(pm, left_token);
            goto return_;
        }
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                  "%s: get symbol success\n%s: call %s[right]\n", self_name, self_name, call_name);

        callBack(CALLPASERSSIGNATURE);  // 获得左值
        if (!call_success(pm)){
            freeToken(left_token, true, false);
            freeStatement(st);
            goto return_;
        }
        readBackToken(right, pm);
        if (right != type){  // 若非正确数值
            char *tmp1, *tmp2;
            tmp1 = memStrcat("ERROR from ", self_name);
            tmp2 = memStrcat(tmp1, "(get right)");
            syntaxError(pm, tmp2, syntax_error);
            memFree(tmp1);
            memFree(tmp2);
            freeToken(left_token, true, true);
            freeStatement(st);
            goto return_;
        }

        popAheadToken(right_token, pm);
        addToken_(pm, setOperationFromToken(st, left_token, right_token, self_type));
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Polynomial: get base value(right) success[push polynomial]\n", NULL);
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
void syntaxError(ParserMessage *pm, char *message, int status){
    if (pm->status != success)
        return;
    pm->status = status;
    pm->status_message = memStrcpy(message, 0, false, false);
}
