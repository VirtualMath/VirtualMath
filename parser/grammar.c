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
    int token_type;
    int stop;
    Statement *base_st = st;
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
            else  if(stop != MATHER_EOF){
                if (global) {
                    syntaxError(pm, command_list_error, 1, "ERROR from parserCommand list(get stop)");
                    printf("stop = %d\n", stop);
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
            status = commandCallBack_(CALLPASERSSIGNATURE, parserDef, FUNCTION, &st,
                                      "Command: call def\n");
            break;
        case MATHER_IF :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserIf, IF_BRANCH, &st,
                                      "Command: call if\n");
            break;
        case MATHER_WHILE :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st,
                                      "Command: call while\n");
            break;
        case MATHER_TRY :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserTry, TRY_BRANCH, &st,
                                      "Command: call try\n");
            break;
        case MATHER_BREAK :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeBreakStatement, BREAK, &st,
                                         "Command: call break\n");
            break;
        case MATHER_CONTINUE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeContinueStatement, CONTINUE, &st,
                                         "Command: call continue\n");
            break;
        case MATHER_RESTART :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRestartStatement, RESTART, &st,
                                         "Command: call restart\n");
            break;
        case MATHER_REGO :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRegoStatement, REGO, &st,
                                         "Command: call rego\n");
            break;
        case MATHER_RETURN :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeReturnStatement, RETURN,  &st,
                                         "Command: call return\n");
            break;
        case MATHER_RAISE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRaiseStatement, RAISE,  &st,
                                         "Command: call raise\n");
            break;
        default :
            status = commandCallBack_(CALLPASERSSIGNATURE, parserOperation, OPERATION, &st,
                                      "Command: call operation\n");
            break;
    }
    if (!status)
        goto return_;
    addStatementToken(COMMAND, st, pm);
    return_:
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Command: get return\n", NULL);
}

void parserControl(PASERSSIGNATURE, Statement *(*callBack)(Statement *), int type){
    Statement *times = NULL;
    Statement *st = NULL;
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
    Statement *st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    StatementList *sl = NULL;
    bool have_if = false;
    again:
    switch (readBackToken(pm)) {
        case MATHER_IF:
            if (have_if)
                goto default_;
            else
                have_if = true;
        case MATHER_ELIF: {
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, 1, "get elif after else\n");
                goto error_;
            }
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
            if (else_st != NULL)
                goto default_;
            Statement *code_tmp = NULL;
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a if...do code"))
                goto error_;
            sl = makeConnectStatementList(sl, NULL, NULL, code_tmp, do_b);
            goto again;
        }
        case MATHER_ELSE:
            if (else_st != NULL){
                syntaxError(pm, syntax_error, 1, "get else after else\n");
                goto error_;
            }
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a if...else code"))
                goto error_;
            goto again;
        case MATHER_FINALLY:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a if...else code"))
                goto error_;
            break;
        case MATHER_ENTER:
            delToken(pm);
            goto again;
        case MATHER_SEMICOLON:
            break;
        default:{
            default_:
            addLexToken(pm, MATHER_ENTER);
            break;
        }
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
    Statement *st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    Statement *do_st = NULL;
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
            sl = makeStatementList(condition_tmp, var_tmp, code_tmp, while_b);
            goto again;
        }
        case MATHER_DO:
            if (do_st != NULL || else_st != NULL)
                goto default_;
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &do_st, "Don't get a while...do code"))
                goto error_;
            goto again;
        case MATHER_ELSE:
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, 1, "get else after else\n");
                goto error_;
            }
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a while...else code"))
                goto error_;
            goto again;
        case MATHER_FINALLY:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a while...finally code"))
                goto error_;
            break;
        case MATHER_ENTER:
            delToken(pm);
            goto again;
        case MATHER_SEMICOLON:
            break;
        default: {
            default_:
            addLexToken(pm, MATHER_ENTER);
            break;
        }
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

void parserTry(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *try_st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    StatementList *sl = NULL;

    again:
    switch (readBackToken(pm)) {
        case MATHER_TRY:{
            if (try_st != NULL)
                goto default_;
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &try_st, "Don't get a try code"))
                goto error_;
            goto again;
        }
        case MATHER_EXCEPT: {
            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            delToken(pm);
            callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, NULL);

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a except var")){
                freeStatement(condition_tmp);
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a except code")) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            sl = makeConnectStatementList(sl, condition_tmp, var_tmp, code_tmp, except_b);
            goto again;
        }
        case MATHER_ELSE:
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, 1, "get else after else\n");
                goto error_;
            }
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a try...else code"))
                goto error_;
            goto again;
        case MATHER_FINALLY:
            delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a try...finally code"))
                goto error_;
            break;
        case MATHER_ENTER:
            delToken(pm);
            goto again;
        case MATHER_SEMICOLON:
            break;
        default: {
            default_:
            addLexToken(pm, MATHER_ENTER);
            break;
        }
    }

    st = makeTryStatement();
    st->u.try_branch.try = try_st;
    st->u.try_branch.except_list = sl;
    st->u.try_branch.else_list = else_st;
    st->u.try_branch.finally = finally_st;
    addStatementToken(TRY_BRANCH, st, pm);
    return;

    error_:
    freeStatement(try_st);
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatementList(sl);
    return;
}

/**
 * @param pm
 * @param inter
 * @param pt
 * @param is_formal 是否为形式参数, 若为true，则限定*args为only_value的结尾, **kwargs为name_value结尾
 * @param is_list 若为true则关闭对name_value和**kwargs的支持
 * @param is_dict 若为true则关闭对only_value和*args的支持
 * @param sep 设定分割符号
 * @param ass 设定赋值符号
 * @return
 */
bool parserParameter(ParserMessage *pm, Inter *inter, Parameter **pt, bool is_formal, bool is_list, bool is_dict, int sep,
                int ass) {
    Parameter *new_pt = NULL;
    Token *tmp;
    bool last_pt = false;
    enum {
        s_1,  // only_value模式
        s_2,  // name_value模式
        s_3,  // only_args模式
        s_4,  // name_args模式
    } status;

    if (is_dict)
        status = s_2;  // is_formal关闭对only_value的支持
    else
        status = s_1;

    while (!last_pt){
        tmp = NULL;
        if (!is_dict && status != s_2 && checkToken_(pm, MATHER_MUL))  // is_formal关闭对*args的支持
            status = s_3;
        else if (!is_list && checkToken_(pm, MATHER_POW))  // is_formal关闭对*args的支持
            status = s_4;

        parserPolynomial(CALLPASERSSIGNATURE);
        if (!call_success(pm))
            goto error_;
        if (readBackToken(pm) != POLYNOMIAL) {
            if (status == s_3) {
                syntaxError(pm, syntax_error, 1, "Don't get a parameter after *");
                goto error_;
            }
            break;
        }
        tmp = popAheadToken(pm);

        int pt_type = value_par;
        if (status == s_1){
            if (!checkToken_(pm, sep)){
                if (is_list || !checkToken_(pm, ass))  // // is_list关闭对name_value的支持
                    last_pt = true;
                else {
                    pt_type = name_par;
                    status = s_2;
                }
            }
        }
        else if (status == s_2){
            pt_type = name_par;
            if (!checkToken_(pm, ass))
                goto error_;
        }
        else if (status == s_3){
            pt_type = args_par;
            if (!checkToken_(pm, sep))
                last_pt = true;
        }
        else if (status == s_4){
            pt_type = kwargs_par;
            if (!checkToken_(pm, sep))
                last_pt = true;
        }

        if (pt_type == value_par)
            new_pt = connectOnlyValueParameter(tmp->data.st, new_pt);
        else if (pt_type == name_par){
            Statement *tmp_value;
            if (!callChildStatement(CALLPASERSSIGNATURE, parserPolynomial, POLYNOMIAL, &tmp_value, "Don't get a parameter value"))
                goto error_;
            new_pt = connectNameValueParameter(tmp_value, tmp->data.st, new_pt);
            if (!checkToken_(pm, sep))
                last_pt = true;
        }
        else if (pt_type == args_par){
            new_pt = connectOnlyArgsParameter(tmp->data.st, new_pt);
            if (is_formal)
                status = s_2;  // 是否规定*args只出现一次
            else
                status = s_1;
        }
        else if (pt_type == kwargs_par){
            new_pt = connectNameArgsParameter(tmp->data.st, new_pt);
            if (is_formal)
                last_pt = true; // 是否规定**kwargs只出现一次
            else
                status = s_2;
        }
        freeToken(tmp, true, false);
    }
    *pt = new_pt;
    return true;

    error_:
    freeToken(tmp, true, true);
    freeParameter(new_pt, true);
    *pt = NULL;
    return false;
}

void parserDef(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *name_tmp = NULL;
    Statement *code_tmp = NULL;
    Parameter *pt = NULL;
    delToken(pm);

    if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &name_tmp,
                            "Don't get a function name"))
        goto error_;

    if (!checkToken_(pm, MATHER_LP)) {
        syntaxError(pm, syntax_error, 1, "Don't get a function ( before parameter");
        goto error_;
    }
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, true, false, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, 1, "Don't get a function parameter");
        goto error_;
    }
    if (!checkToken_(pm, MATHER_RP)) {
        syntaxError(pm, syntax_error, 1, "Don't get a function ) after parameter");
        goto error_;
    }
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserDef: get function title success\n", NULL);
    writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "parserDef: call parserCode\n", NULL);

    if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a function code")) {
        syntaxError(pm, syntax_error, 1, "Don't get a function code");
        goto error_;
    }

    st = makeFunctionStatement(name_tmp, code_tmp, pt);
    addLexToken(pm, MATHER_ENTER);
    addStatementToken(FUNCTION, st, pm);
    return;

    error_:
    freeStatement(name_tmp);
    freeStatement(code_tmp);
    return;
}

void parserCode(PASERSSIGNATURE){
    Token *code_token = NULL;
    Token *tk = NULL;
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
    if (!checkToken_(pm, MATHER_RC)) {
        syntaxError(pm, syntax_error, 1, "Don't get the }");
        goto error_;
    }

    return_:
    addStatementToken(CODE, st, pm);
    freeToken(code_token, true, false);
    return;

    error_:
    freeToken(code_token, true, true);
    return;
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
    return twoOperation(CALLPASERSSIGNATURE, parserTuple, switchAssignment, TUPLE, ASSIGNMENT,
                        "polynomial", "assignment", true);
}

/**
 * 元组匹配
 * parserTuple
 * | switchPolynomial
 * | parserTuple COMMA switchPolynomial
 * @param pm
 * @param inter
 */
void parserTuple(PASERSSIGNATURE){
    Parameter *pt = NULL;
    Statement *st = NULL;
    Token *tmp = NULL;
    if (readBackToken(pm) == MATHER_MUL)
        goto parserPt;

    if (!callChildToken(CALLPASERSSIGNATURE, parserPolynomial, POLYNOMIAL, &tmp, NULL, syntax_error))
        goto return_;
    if (readBackToken(pm) != MATHER_COMMA){
        tmp->token_type = TUPLE;
        addToken_(pm ,tmp);
        goto return_;
    }
    addToken_(pm ,tmp);

    parserPt:
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, true, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, 1, "Don't get tuple element");
        goto return_;
    }
    st = makeTupleStatement(pt, value_tuple);
    addStatementToken(TUPLE, st, pm);

    return_:
    return;
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
                        "factor", "polynomial", false);
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
                        "call back", "factor", false);
}

int tailCall(PASERSSIGNATURE, Token *left_token, Statement **st){
    Parameter *pt = NULL;
    if (readBackToken(pm) != MATHER_LP)
        return -1;
    delToken(pm);
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, false, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, 1, "Don't get call parameter");
        return 0;
    }
    if (!checkToken_(pm, MATHER_RP)){
        syntaxError(pm, syntax_error, 1, "Don't get ) from call back");
        return 0;
    }
    *st = makeCallStatement(left_token->data.st, pt);
    return 1;
}
void parserCallBack(PASERSSIGNATURE){
    return tailOperation(CALLPASERSSIGNATURE, parserBaseValue, tailCall, BASEVALUE, CALLBACK,
            "Base Value", "Call Back");
}

int getOperation(PASERSSIGNATURE, int right_type, Statement **st, char *name){
    *st = NULL;
    if (checkToken_(pm, right_type))
        goto return_;

    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, st, NULL))
        return 0;

    if (!checkToken_(pm, right_type)){
        freeStatement(*st);
        return -1;
    }

    return_:
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
    Statement *st = NULL;
    token_type = readBackToken(pm);
    value_token = popAheadToken(pm);
    if (MATHER_NUMBER == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get number\n", NULL);
        char *stop;
        st = makeBaseValueStatement(makeLinkValue(makeNumberValue(strtol(value_token->data.str, &stop, 10), inter), NULL, inter));
    }
    else if (MATHER_STRING == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get string\n", NULL);
        st = makeBaseValueStatement(makeLinkValue(makeStringValue(value_token->data.str, inter), NULL, inter));
    }
    else if (MATHER_VAR == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get var\n", NULL);
        st = makeBaseVarStatement(value_token->data.str, NULL);
    }
    else if (MATHER_SVAR == token_type){
        Statement *svar_st;
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: get super var\n", NULL);
        if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &svar_st, NULL)){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get super var after $");
            goto return_;
        }
        st = makeBaseSVarStatement(svar_st, NULL);
    }
    else if (MATHER_LB == token_type){
        int tmp;
        Statement *tmp_st = NULL;
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation\n", NULL);

        tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RB, &tmp_st, "base value");
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation success\n", NULL);
        if (tmp == 0){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get operation from Base Value");
            goto return_;
        }
        else if(tmp == -1){
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get ] from list/var");
            goto return_;
        }

        if (MATHER_VAR == readBackToken(pm)){
            Token *var_token;
            var_token = popAheadToken(pm);
            st = makeBaseVarStatement(var_token->data.str, tmp_st);
            freeToken(var_token, true, false);
        }
        else{
            if (tmp_st == NULL)
                st = makeTupleStatement(NULL, value_list);
            else if (tmp_st->type == base_list && tmp_st->u.base_list.type == value_tuple){
                tmp_st->u.base_list.type = value_list;
                st = tmp_st;
            }
            else
                st = makeTupleStatement(makeOnlyValueParameter(tmp_st), value_list);
        }
    }
    else if (MATHER_LP == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get operation\n", NULL);
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
    else if (MATHER_LC == token_type){
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "base value: get dict\n", NULL);
        Parameter *pt;
        if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, false, true, MATHER_COMMA, MATHER_COLON)) {
            freeToken(value_token, true, true);
            syntaxError(pm, syntax_error, 1, "Don't get a dict parameter");
            goto return_;
        }
        if (!checkToken_(pm, MATHER_RC)) {
            freeToken(value_token, true, true);
            freeParameter(pt, true);
            syntaxError(pm, syntax_error, 1, "Don't get a } after dict");
            goto return_;
        }
        st = makeBaseDictStatement(pt);
    }
    else{
        writeLog_(pm->grammar_debug, GRAMMAR_DEBUG, "Base Value: else\n", NULL);
        backToken_(pm, value_token);
        goto return_;
    }
    freeToken(value_token, true, false);
    addStatementToken(BASEVALUE, st, pm);

    return_:
    return;
}
