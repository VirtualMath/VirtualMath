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
            delToken(pm);
            goto return_;
        }
        else if (token_type == MATHER_ENTER){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: <ENTER>\n", NULL);
            delToken(pm);
            continue;
        }
        else{
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command List: call command\n", NULL);
            Token *command_token;
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
            if (stop == MATHER_ENTER || stop == MATHER_SEMICOLON){
                delToken(pm);
            }
            else if(stop == MATHER_EOF){
                PASS;
            }
            else{
                if (global) {
                    syntaxError(pm, "ERROR from parserCommand list(get stop)", command_list_error);
                    freeToken(command_token, true, true);
                }
                else{
                    // 若非global模式, 即可以匹配大括号, token保留在ahead中
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
    if (MATHER_DEF == token_type){
        int def;
        Token *def_token = NULL;
        parserDef(CALLPASERSSIGNATURE);
        if (!call_success(pm))
            goto return_;
        readBackToken(def, pm);
        if (def != FUNCTION)
            goto return_;
        popAheadToken(def_token, pm);
        st = def_token->data.st;
        freeToken(def_token, true, false);
    }
    else{
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: call operation\n", NULL);
        int command_type;
        Token *command_token = NULL;
        parserOperation(CALLPASERSSIGNATURE);
        if (!call_success(pm))
            goto return_;
        readBackToken(command_type, pm);
        if (command_type != OPERATION)
            goto return_;
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

void parserIf(PASERSSIGNATURE){
//    int if_type, code;
//    Token *if_token = NULL, *code_token = NULL;
//    struct Statement *st = NULL, *tmp = NULL;
//    StatementList *sl = NULL;
//
//    checkToken(pm, MATHER_IF, return_);
//    callChild(pm, if_type, if_token, parserOperation, OPERATION, return_, error_);
//    if (if_type != OPERATION){
//        goto return_;
//    }
//    callChild(pm, code, code_token, parserCode, CODE, error_, error_);
//
//    sl = makeConnectStatementList(sl, if_token->data.st, NULL, code_token->data.st);
//
//    st = makeFunctionStatement(name_token->data.st, code_token->data.st);
//    addStatementToken(FUNCTION, st, pm);
//    freeToken(code_token, true, false);
//    freeToken(name_token, true, false);
//    return_: return;
//
//    error_:
//    freeToken(if_token, true, true);
//    freeToken(code_token, true, true);
//    syntaxError(pm, "Don't get a if titile", syntax_error);
//    return;
}

void parserDef(PASERSSIGNATURE){
    int name_type, code;
    Token *name_token = NULL, *code_token = NULL;
    struct Statement *st = NULL;

    checkToken(pm, MATHER_DEF, return_);
    parserBaseValue(CALLPASERSSIGNATURE);
    if (!call_success(pm))
        goto return_;
    readBackToken(name_type, pm);
    if (name_type != BASEVALUE){
        syntaxError(pm, "Don't get a function name", syntax_error);
        goto return_;
    }
    popAheadToken(name_token, pm);

    checkToken(pm, MATHER_LP, error_);
    checkToken(pm, MATHER_RP, error_);
    parserCode(CALLPASERSSIGNATURE);
    if (!call_success(pm)){
        goto error_;
    }
    readBackToken(code, pm);
    if (code != CODE){
        freeToken(name_token, true, true);
        error_:
        syntaxError(pm, "Don't get a function code", syntax_error);
        goto return_;
    }
    popAheadToken(code_token, pm);

    st = makeFunctionStatement(name_token->data.st, code_token->data.st);
    addStatementToken(FUNCTION, st, pm);
    freeToken(code_token, true, false);
    freeToken(name_token, true, false);

    return_:
    return;
}

void parserCode(PASERSSIGNATURE){
    int code_type;
    Token *code_token = NULL;
    struct Statement *st = makeStatement();
    while (true){
        checkToken(pm, MATHER_LC, again_);  // 若匹配的不是{则检查是否匹配到\n
        break;  // 若匹配到{则跳出循环
        again_: checkToken(pm, MATHER_ENTER, return_);  // 若不是\n则跳到return_
    }
    pasersCommandList(CALLPASERSSIGNATURE, false, st);
    if (!call_success(pm)){
        goto return_;
    }
    readBackToken(code_type, pm);
    if (code_type != COMMANDLIST){
        syntaxError(pm, "Not CommandList\n", syntax_error);
        goto return_;
    }
    popAheadToken(code_token, pm);
    checkToken(pm, MATHER_RC, error_);

    return_:
    addStatementToken(CODE, st, pm);
    freeToken(code_token, true, false);
    return;

    error_:
    syntaxError(pm, "Don't get the }", syntax_error);
    freeToken(code_token, true, true);
    return;
}

/**
 * 表达式匹配
 * parserOperation：
 * | parserAssignment
 */
void parserOperation(PASERSSIGNATURE){
    int operation_type;
    Token *operation_token = NULL;
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: call assignment\n", NULL);
    parserAssignment(CALLPASERSSIGNATURE);
    if (!call_success(pm))
        goto return_;
    readBackToken(operation_type, pm);
    if (operation_type != ASSIGNMENT){
        goto return_;
    }
    popAheadToken(operation_token, pm);
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
    return twoOperation(CALLPASERSSIGNATURE, parserCallBack, switchFactor, CALLBACK, FACTOR,
                        "call back", "factor");
}

int tailCall(PASERSSIGNATURE, Token *left_token, Statement **st){
    int symbol;
    readBackToken(symbol, pm);
    if (symbol != MATHER_LP){
        return -1;
    }
    delToken(pm);
    checkToken(pm, MATHER_RP, error_);
    *st = makeCallStatement(left_token->data.st);
    return 1;

    error_:
    syntaxError(pm, "Don't get ) from call back", syntax_error);
    return 0;
}
void parserCallBack(PASERSSIGNATURE){
    return tailOperation(CALLPASERSSIGNATURE, parserBaseValue, tailCall, BASEVALUE, CALLBACK,
            "Base Value", "Call Back");
}

int getOperation(PASERSSIGNATURE, int right_type, Statement **st, char *name){
    int operation_type, rp;
    Token *operation_token;

    parserOperation(CALLPASERSSIGNATURE);
    if (!call_success(pm)){
        return 0;
    }
    readBackToken(operation_type, pm);
    if (operation_type != OPERATION){
        return 0;
    }
    popAheadToken(operation_token, pm);

    readBackToken(rp, pm);
    if (right_type != rp){
        return -1;
    }
    delToken(pm);

    *st = operation_token->data.st;
    freeToken(operation_token, true, false);
    return 1;
}

/**
 * 字面量匹配
 * parserBaseValue：
 * | MATHER_NUMBER
 * | MATHER_STRING
 */
void parserBaseValue(PASERSSIGNATURE){
    int token_type;
    Token *value_token = NULL;
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
    else if(MATHER_LB == token_type){
        int var, tmp;
        Statement *tmp_st = NULL;
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation\n", NULL);
        popAheadToken(value_token, pm);

        tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RB, &tmp_st, "base value");
        if (tmp == 0){
            freeToken(value_token, true, true);
            syntaxError(pm, "Don't get operation from list/var", syntax_error);
            goto return_;
        }
        else if(tmp == -1){
            freeToken(value_token, true, true);
            syntaxError(pm, "Don't get ] from list/var", syntax_error);
            goto return_;
        }
        readBackToken(var, pm);
        if (MATHER_VAR == var){
            Token *var_token;
            popAheadToken(var_token, pm);
            st = makeStatement();
            st->type = base_var;
            st->u.base_var.name = memStrcpy(var_token->data.str, 0, false, false);
            st->u.base_var.times = tmp_st;
            freeToken(var_token, true, false);
        }
        // TODO-szh list处理
    }
    else if(MATHER_LP == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation\n", NULL);
        popAheadToken(value_token, pm);
        int tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RP, &st, "base value");
        if (tmp == 0){
            freeToken(value_token, true, true);
            syntaxError(pm, "Don't get operation from Base Value", syntax_error);
            goto return_;
        }
        else if(tmp == -1){
            freeToken(value_token, true, true);
            syntaxError(pm, "Don't get ) from Base Value", syntax_error);
            goto return_;
        }
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
        Token *left_token = NULL, *symbol_token = NULL, *right_token = NULL;
        struct Statement *st = NULL;

        readBackToken(left, pm);
        if (left != self_type){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call %s(left)\n", self_name, call_name);
            callBack(CALLPASERSSIGNATURE);
            if (!call_success(pm))
                goto return_;
            readBackToken(left, pm);
            if (left != type){
                goto return_;
            }
            popAheadToken(left_token, pm);
            addStatementToken(self_type, left_token->data.st, pm);
            freeToken(left_token, true, false);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                      "%s: get %s(left) success[push %s]\n", self_name, call_name, self_name);
            continue;
        }
        popAheadToken(left_token, pm);

        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call symbol\n", self_name);
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

inline void tailOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*tailFunction)(PASERSSIGNATURE, Token *left_token,  Statement **st), int type, int self_type, char *call_name, char *self_name){
    while(true){
        int left, symbol;
        Token *left_token = NULL, *symbol_token = NULL, *right_token = NULL;
        struct Statement *st = NULL;

        readBackToken(left, pm);
        if (left != self_type){
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "%s: call %s(left)\n", self_name, call_name);
            callBack(CALLPASERSSIGNATURE);
            if (!call_success(pm))
                goto return_;
            readBackToken(left, pm);
            if (left != type){
                goto return_;
            }
            popAheadToken(left_token, pm);
            addStatementToken(self_type, left_token->data.st, pm);
            freeToken(left_token, true, false);
            writeLog_(pm->grammar_debug, GRAMMAR_DEBUG,
                      "%s: get %s(left) success[push %s]\n", self_name, call_name, self_name);
            continue;
        }
        popAheadToken(left_token, pm);

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
void syntaxError(ParserMessage *pm, char *message, int status){
    if (pm->status != success)
        return;
    pm->status = status;
    pm->status_message = memStrcpy(message, 0, false, false);
}
