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

void freeParserMessage(ParserMessage *pm, bool self) {
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
 * parserCommandList :
 * | MATHER_EOF
 * | parserCommand MATHER_ENTER
 * | parserCommand MATHER_EOF
 */
void parserCommandList(ParserMessage *pm, Inter *inter, bool global, Statement *st) {
    int token_type, stop;
    struct Statement *base_st = st;
    while (true){
        token_type = readBackToken(pm);
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
            if (!callChildToken(CALLPASERSSIGNATURE, parserCommand, COMMAND, &command_token,
                               (global ? "ERROR from command list(get parserCommand)" : NULL),
                               command_list_error)){
                goto return_;
            }

            stop = readBackToken(pm);
            if (stop == MATHER_ENTER || stop == MATHER_SEMICOLON){
                delToken(pm);
            }
            else if(stop == MATHER_EOF){
                PASS;
            }
            else{
                if (global) {
                    syntaxError(pm, command_list_error, 1, "ERROR from parserCommand list(get stop)");
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
    int token_type, status;
    Statement *st = NULL;
    token_type = readBackToken(pm);
    switch (token_type) {
        case MATHER_DEF :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserDef, FUNCTION, &st, "Command: call def\n");
            break;
        case MATHER_IF :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserIf, IF_BRANCH, &st, "Command: call def\n");
            break;
        case MATHER_WHILE :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st, "Command: call def\n");
            break;
        case MATHER_BREAK :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeBreakStatement, BREAK, &st, "Command: call break\n");
            break;
        case MATHER_CONTINUE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeContinueStatement, CONTINUE, &st, "Command: call continue\n");
            break;
        case MATHER_REGO :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRegoStatement, REGO, &st, "Command: call rego\n");
            break;
        case MATHER_RETURN :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeReturnStatement, RETURN,  &st, "Command: call return\n");
            break;
        default :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserOperation, OPERATION, &st, "Command: call def\n");
            break;
    }
    if (!status)
        goto return_;
    addStatementToken(COMMAND, st, pm);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: get  command success\n", NULL);
    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: get return\n", NULL);
}

void parserControl(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type){
    Statement *times = NULL, *st = NULL;
    delToken(pm);
    parserOperation(CALLPASERSSIGNATURE);
    if (!call_success(pm))
        goto return_;
    if (readBackToken(pm) == OPERATION){
        Token *tmp;
        tmp= popAheadToken(pm);
        times = tmp->data.st;
        freeToken(tmp, true, false);
    }
    st = callBack(times);
    addStatementToken(type, st, pm);
    return_:
    return;
}

void parserIf(PASERSSIGNATURE){
    struct Statement *st = NULL, *else_st = NULL, *finally_st = NULL;
    StatementList *sl = NULL;
    bool have_if = false;
    // TODO-szh 设置重复警告 (添加PASS语句)
    again:
    switch (readBackToken(pm)) {
        case MATHER_IF:
            if (have_if)
                goto default_;
            else
                have_if = true;
        case MATHER_ELIF: {
            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            delToken(pm);
            if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, "Don't get a if condition"))
                goto error_;

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a while var")) {
                freeStatement(condition_tmp);
                goto error_;
            }

            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a if code")) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            sl = makeConnectStatementList(sl, condition_tmp, var_tmp, code_tmp, if_b);
            goto again;
        }
        case MATHER_DO: {
            Statement *code_tmp = NULL;
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a if...do code"))
                goto error_;
            sl = makeConnectStatementList(sl, NULL, NULL, code_tmp, do_b);
            goto again;
        }
        case MATHER_ELSE:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a if...else code"))
                goto error_;
            goto again;
        case MATHER_FINALLY:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a if...else code"))
                goto error_;
            goto again;
        case MATHER_ENTER:
            delToken(pm);
            goto again;
        default:
            default_: break;
    }

    st = makeIfStatement();
    st->u.if_branch.if_list = sl;
    st->u.if_branch.else_list = else_st;
    st->u.if_branch.finally = finally_st;
    addStatementToken(IF_BRANCH, st, pm);
    return;

    error_:
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatementList(sl);
    return;
}

void parserWhile(PASERSSIGNATURE){
    struct Statement *st = NULL, *else_st = NULL, *finally_st = NULL,*do_st = NULL;
    StatementList *sl = NULL;
    bool have_while = false;

    again:
    switch (readBackToken(pm)) {
        case MATHER_WHILE: {
            if (have_while)
                goto default_;
            else
                have_while = true;

            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            delToken(pm);
            if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, "Don't get a while condition"))
                goto error_;

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a while var")){
                freeStatement(condition_tmp);
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a while code")) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            if (sl != NULL)
                freeStatementList(sl);
            sl = makeStatementList(condition_tmp, var_tmp, code_tmp, if_b);
            goto again;
        }
        case MATHER_DO:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &do_st, "Don't get a while...do code"))
                goto error_;
            goto again;
        case MATHER_ELSE:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a while...else code"))
                goto error_;
            goto again;
        case MATHER_FINALLY:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a while...finally code"))
                goto error_;
            goto again;
        case MATHER_ENTER:
            delToken(pm);
            goto again;
        default:
            default_: break;
    }

    st = makeWhileStatement();
    st->u.while_branch.while_list = sl;
    st->u.while_branch.else_list = else_st;
    st->u.while_branch.finally = finally_st;
    st->u.while_branch.after = do_st;
    addStatementToken(WHILE_BRANCH, st, pm);
    return;

    error_:
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatement(do_st);
    freeStatementList(sl);
    return;
}

void parserDef(PASERSSIGNATURE){
    struct Statement *st = NULL, *name_tmp = NULL, *code_tmp = NULL;
    delToken(pm);

    if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &name_tmp, "Don't get a function name"))
        goto error_;

    if (!checkToken_(pm, MATHER_LP) || !checkToken_(pm, MATHER_RP)){
        goto error_;
    }
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserDef: get function title success\n", NULL);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserDef: call parserCode\n", NULL);

    if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a function code"))
        goto error_;

    st = makeFunctionStatement(name_tmp, code_tmp);
    addStatementToken(FUNCTION, st, pm);
    return;

    error_:
    freeStatement(name_tmp);
    freeStatement(code_tmp);
    syntaxError(pm, syntax_error, 1, "Don't get a function code");
    return;
}

void parserCode(PASERSSIGNATURE){
    Token *code_token = NULL, *tk = NULL;
    Statement *st = makeStatement();
    while (true){
        if (!checkToken_(pm, MATHER_LC))
            goto again_;
        break;
        again_:
        if (!checkToken_(pm, MATHER_ENTER))
            goto return_;
    }
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserCode: call parserCommandList\n", NULL);
    parserCommandList(CALLPASERSSIGNATURE, false, st);
    if (!call_success(pm)){
        goto error_;
    }
    if (readBackToken(pm) != COMMANDLIST){
        syntaxError(pm, syntax_error, 1, "Not CommandList\n");
        goto error_;
    }
    code_token = popAheadToken(pm);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserCode: call parserCommandList success\n", NULL);
    if (!checkToken_(pm, MATHER_RC))
        goto error_;

    return_:
    addStatementToken(CODE, st, pm);
    freeToken(code_token, true, false);
    return;

    error_:
    syntaxError(pm, syntax_error, 1, "Don't get the }");
    freeToken(code_token, true, true);
    freeStatement(st);
}

/**
 * 表达式匹配
 * parserOperation：
 * | parserAssignment
 */
void parserOperation(PASERSSIGNATURE){
    Statement *operation_st = NULL;
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: call assignment\n", NULL);
    if (!callChildStatement(CALLPASERSSIGNATURE, parserAssignment, ASSIGNMENT, &operation_st, NULL))
        goto return_;

    addStatementToken(OPERATION, operation_st, pm);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: get polynomial success\n", NULL);

    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Operation: return\n", NULL);
    return;
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
    if (readBackToken(pm) != MATHER_LP){
        return -1;
    }
    delToken(pm);
    if (!checkToken_(pm, MATHER_RP)){
        syntaxError(pm, syntax_error, 1, "Don't get ) from call back");
        return 0;
    }
    *st = makeCallStatement(left_token->data.st);
    return 1;
}
void parserCallBack(PASERSSIGNATURE){
    return tailOperation(CALLPASERSSIGNATURE, parserBaseValue, tailCall, BASEVALUE, CALLBACK,
            "Base Value", "Call Back");
}

int getOperation(PASERSSIGNATURE, int right_type, Statement **st, char *name){
    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, st, NULL))
        return 0;

    if (readBackToken(pm) != right_type){
        freeStatement(*st);
        return -1;
    }
    delToken(pm);
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
    token_type = readBackToken(pm);
    if(MATHER_NUMBER == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get number\n", NULL);
        value_token= popAheadToken(pm);
        char *stop;
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeNumberValue(strtol(value_token->data.str, &stop, 10), inter);
    }
    else if(MATHER_STRING == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get string\n", NULL);
        value_token= popAheadToken(pm);
        st = makeStatement();
        st->type = base_value;
        st->u.base_value.value = makeStringValue(value_token->data.str, inter);
    }
    else if(MATHER_VAR == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get var\n", NULL);
        value_token= popAheadToken(pm);
        st = makeStatement();
        st->type = base_var;
        st->u.base_var.name = memStrcpy(value_token->data.str, 0, false, false);
        st->u.base_var.times = NULL;
    }
    else if(MATHER_LB == token_type){
        int tmp;
        Statement *tmp_st = NULL;
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation\n", NULL);
        value_token= popAheadToken(pm);

        tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RB, &tmp_st, "base value");
        if (tmp == 0){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get operation from list/var");
            goto return_;
        }
        else if(tmp == -1){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get ] from list/var");
            goto return_;
        }
        if (MATHER_VAR == readBackToken(pm)){
            Token *var_token;
            var_token= popAheadToken(pm);
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
        value_token= popAheadToken(pm);
        int tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RP, &st, "base value");
        if (tmp == 0){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get operation from Base Value");
            goto return_;
        }
        else if(tmp == -1){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get ) from Base Value");
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
        Token *left_token = NULL, *right_token = NULL;
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
        left_token= popAheadToken(pm);

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

        right_token= popAheadToken(pm);
        addToken_(pm, setOperationFromToken(st, left_token, right_token, self_type));
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Polynomial: get base value(right) success[push polynomial]\n", NULL);
    }
    return_:
    return;
}

inline void tailOperation(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int (*tailFunction)(PASERSSIGNATURE, Token *left_token,  Statement **st), int type, int self_type, char *call_name, char *self_name){
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
        left_token= popAheadToken(pm);

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
    if (readBackToken(pm) != type){
        return false;
    }
    delToken(pm);
    return true;
}

bool commandCallControl_(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type, Statement **st, char *message){
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "message", NULL);
    Token *tmp_token = NULL;
    parserControl(CALLPASERSSIGNATURE, callBack, type);
    if (!call_success(pm) || readBackToken(pm) != type)
        return false;
    tmp_token = popAheadToken(pm);
    *st = tmp_token->data.st;
    freeToken(tmp_token, true, false);
    return true;
}

inline bool commandCallBack_(PASERSSIGNATURE, void (*callBack)(PASERSSIGNATURE), int type, Statement **st, char *message){
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

bool callChildToken(ParserMessage *pm, Inter *inter, void (*call)(ParserMessage *, Inter *), int type, Token **tmp,
                    char *message, int error_type) {
    call(CALLPASERSSIGNATURE);
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

bool callChildStatement(PASERSSIGNATURE, void (*call)(PASERSSIGNATURE), int type, Statement **st, char *message){
    Token *tmp = NULL;
    bool status = callChildToken(CALLPASERSSIGNATURE, call, type, &tmp, message, syntax_error);
    if (!status){
        *st = NULL;
        return false;
    }
    *st = tmp->data.st;
    freeToken(tmp, true, false);
    return true;
}
