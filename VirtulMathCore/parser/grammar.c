#include "__grammar.h"

ParserMessage *makeParserMessage(char *file_dir) {
    ParserMessage *tmp = memCalloc(1, sizeof(ParserMessage));
    tmp->file = memStrcpy(file_dir == NULL ? "stdin" : file_dir);
    tmp->tm = makeTokenMessage(file_dir);
    tmp->status = success;
    tmp->status_message = NULL;
    return tmp;
}

void freeParserMessage(ParserMessage *pm, bool self) {
    FREE_BASE(pm, return_);
    freeTokenMessage(pm->tm, true, true);
    memFree(pm->status_message);
    memFree(pm->file);
    if (self)
        memFree(pm);
    return_:
    return;
}

/**
 * 命令表匹配
 * parserCommandList :
 * | MATHER_EOF
 * | MATHER_ENTER
 * | MATHER_SEMICOLON
 * | parserCommand MATHER_ENTER
 * | parserCommand MATHER_SEMICOLON
 * | parserCommand MATHER_EOF
 */
void parserCommandList(PASERSSIGNATURE, bool global, bool is_one, Statement *st) {
    int token_type;
    int save_enter = pm->tm->file->filter_data.enter;
    char *command_message = global ? "ERROR from command list(get parserCommand)" : NULL;
    void *bak = NULL;
    fline line = 0;
    bool should_break = false;
    bool have_command = false;

    pm_KeyInterrupt = signal_reset;
    bak = signal(SIGINT, signalStopPm);
    pm->tm->file->filter_data.enter = 0;

    while (!should_break){
        token_type = readBackToken(pm);
        if (token_type == -3 || token_type == -2)
            break;
        else if (token_type == MATHER_EOF){
            delToken(pm);
            break;
        }
        else if (token_type == MATHER_ENTER || token_type == MATHER_SEMICOLON){
            delToken(pm);
            if (is_one && have_command)
                break;
        }
        else{
            Token *command_token = NULL;
            int stop;
            have_command = true;
            if (!callChildToken(CALLPASERSSIGNATURE, parserCommand, COMMAND, &command_token, command_message, command_list_error))
                break;
            line = command_token->line;
            stop = readBackToken(pm);
            if (stop == MATHER_ENTER) {
                delToken(pm);
                if (is_one)
                    should_break = true;
            }
            else if (stop == MATHER_SEMICOLON)
                delToken(pm);
            else  if(stop != MATHER_EOF){
                if (global) {
                    Token *tk = popNewToken(pm->tm);
                    freeToken(tk, true);
                    syntaxError(pm, command_list_error, command_token->line, 1, "ERROR from parserCommand list(get stop)");
                    freeToken(command_token, true);
                }
                else{
                    connectStatement(st, command_token->data.st);
                    freeToken(command_token, false);
                }
                break;
            }
            connectStatement(st, command_token->data.st);
            freeToken(command_token, false);
        }
    }
    if (is_one)
        clearLexFile(pm->tm->file);
    signal(SIGINT, bak);
    if (pm_KeyInterrupt != signal_reset) {
        pm_KeyInterrupt = signal_reset;
        syntaxError(pm, int_error, line, 1, "KeyInterrupt");
    }
    pm->tm->file->filter_data.enter = save_enter;
}

/**
 * 命令匹配
 * parserCommand：
 * | MATHER_DEF parserDef
 * | MATHER_IF parserIf
 * | MATHER_WHILE parserWhile
 * | MATHER_TRY parserTry
 * | MATHER_BREAK parserControl
 * | MATHER_CONTINUE parserControl
 * | MATHER_RESTART parserControl
 * | MATHER_REGO parserControl
 * | MATHER_RETURN parserControl
 * | MATHER_RAISE parserControl
 * | parserOperation
 */
void parserCommand(PASERSSIGNATURE){
    int token_type, status;
    Statement *st = NULL;
    token_type = readBackToken(pm);
    switch (token_type) {
        case MATHER_AT :
            status = callChildStatement(CALLPASERSSIGNATURE, parserDecoration, DECORATION, &st, NULL);
            break;
        case MATHER_NONLOCAL :
        case MATHER_GLOBAL :
        case MATHER_DEFAULT :
            status = callChildStatement(CALLPASERSSIGNATURE, parserVarControl, VARCONTROL, &st, NULL);
            break;
        case MATHER_CLASS :
        case MATHER_DEF :
            status = callChildStatement(CALLPASERSSIGNATURE, parserDef, FUNCTION, &st, NULL);
            break;
        case MATHER_GOTO :
            status = callChildStatement(CALLPASERSSIGNATURE, parserGoto, GOTO, &st, NULL);
            break;
        case MATHER_LABEL :
            status = callChildStatement(CALLPASERSSIGNATURE, parserLabel, LABEL, &st, NULL);
            break;
        case MATHER_DO :
            status = callChildStatement(CALLPASERSSIGNATURE, parserDo, DO_BRANCH, &st, NULL);
            break;
        case MATHER_WITH :
            status = callChildStatement(CALLPASERSSIGNATURE, parserWith, WITH_BRANCH, &st, NULL);
            break;
        case MATHER_IF :
            status = callChildStatement(CALLPASERSSIGNATURE, parserIf, IF_BRANCH, &st, NULL);
            break;
        case MATHER_FOR :
            status = callChildStatement(CALLPASERSSIGNATURE, parserFor, FOR_BRANCH, &st, NULL);
            break;
        case MATHER_WHILE :
            status = callChildStatement(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st, NULL);
            break;
        case MATHER_TRY :
            status = callChildStatement(CALLPASERSSIGNATURE, parserTry, TRY_BRANCH, &st, NULL);
            break;
        case MATHER_BREAK :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeBreakStatement, BREAK, &st, false, NULL);
            break;
        case MATHER_CONTINUE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeContinueStatement, CONTINUE, &st, false, NULL);
            break;
        case MATHER_RESTART :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRestartStatement, RESTART, &st, false, NULL);
            break;
        case MATHER_REGO :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRegoStatement, REGO, &st, false, NULL);
            break;
        case MATHER_RETURN :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeReturnStatement, RETURN, &st, false, NULL);
            break;
        case MATHER_YIELD :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeYieldStatement, YIELD, &st, false, NULL);
            break;
        case MATHER_RAISE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeRaiseStatement, RAISE, &st, false, NULL);
            break;
        case MATHER_ASSERT :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeAssertStatement, ASSERT, &st, true,
                                         "parserAssert: Don't get conditions after assert");
            break;
        case MATHER_INCLUDE :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeIncludeStatement, INCLUDE, &st, true,
                                         "parserInclude: Don't get file after include");
            break;
        case MATHER_DEL :
            status = commandCallControl_(CALLPASERSSIGNATURE, makeDelStatement, DEL, &st, true,
                                         "parserInclude: Don't get operation after del");
            break;
        case MATHER_FROM :
        case MATHER_IMPORT :
            status = callChildStatement(CALLPASERSSIGNATURE, parserImport, IMPORT, &st, NULL);
            break;
        case MATHER_STRING:
        case MATHER_NUMBER:
        case MATHER_VAR:
        case MATHER_SVAR:
        case MATHER_LC:
        case MATHER_LB:
        case MATHER_LP:
        case MATHER_SUB:
        case MATHER_PROTECT:
        case MATHER_PRIVATE:
        case MATHER_PUBLIC:
        case MATHER_PASSVALUE:
            status = callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &st, NULL);
            break;
        default:
            status = false;
            break;
    }
    if (!status)
        goto return_;
    addStatementToken(COMMAND, st, pm);
    return_: return;
}

void parserDecoration(PASERSSIGNATURE){
    Statement *st = NULL;
    DecorationStatement *ds = NULL;
    int tmp;
    long int line = 0;
    while ((tmp = readBackToken(pm)) == MATHER_AT || tmp == MATHER_ENTER){
        Statement *dst = NULL;
        line = delToken(pm);
        if (tmp == MATHER_ENTER)
            continue;
        if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &dst, "Don't get a decoration operation"))
            goto error_;
        ds = connectDecorationStatement(dst, ds);
    }
    if (tmp!= MATHER_CLASS && tmp != MATHER_DEF) {
        syntaxError(pm, syntax_error, line, 1, "Don't get a decoration object");
        goto error_;
    }
    if (!callChildStatement(CALLPASERSSIGNATURE, parserDef, FUNCTION, &st, "Don't get a decoration object"))
        goto error_;
    if (tmp == MATHER_CLASS)
        st->u.set_class.decoration = ds;
    else
        st->u.set_function.decoration = ds;
    addStatementToken(DECORATION, st, pm);
    return;

    error_:
    freeDecorationStatement(ds);
}

/**
 * label语句匹配
 * parserLabel:
 * | MATHER_LABEL MATHER_COLON MATHER_STRING(MATHER_VAR) stop_token [缺省所有参数]
 * | MATHER_LABEL MATHER_COLON MATHER_STRING(MATHER_VAR) MATHER_AS parserOperation stop_token [缺省command参数]
 * | MATHER_LABEL MATHER_COLON MATHER_STRING(MATHER_VAR) MATHER_COLON parserOperation stop_token
 * | MATHER_LABEL MATHER_COLON MATHER_STRING(MATHER_VAR) MATHER_AS parserOperation MATHER_COLON parserOperation [缺省var参数]
 * @param pm
 * @param inter
 */
void parserLabel(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *var = NULL;
    Statement *command = NULL;
    Token *label_tk = NULL;
    int tmp;
    char *label = NULL;
    long int line = delToken(pm);

    if (!checkToken(pm, MATHER_COLON)){
        syntaxError(pm, syntax_error, line, 1, "Don't get : after label");
        goto error_;
    }
    switch (readBackToken(pm)) {
        case MATHER_STRING:
        case MATHER_VAR:
            label_tk = popNewToken(pm->tm);
            label = memStrcpy(label_tk->data.str);
            freeToken(label_tk, false);
            break;
        default:
            syntaxError(pm, syntax_error, line, 1, "Don't get a label name");
            goto error_;
    }

    if ((tmp = readBackToken(pm)) == MATHER_ENTER || tmp == MATHER_SEMICOLON || tmp == MATHER_EOF)  // 匹配到stop_token则label后参数缺省
        goto make;
    if (tmp != MATHER_COLON) {  // 匹配到 ： 则label的times参数缺省
        if (!checkToken(pm, MATHER_AS)) {
            syntaxError(pm, syntax_error, line, 1, "Don't get as after goto label");
            goto error_;
        } else if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &var, "Don't get a goto var"))
            goto error_;
    }

    if ((tmp = readBackToken(pm)) == MATHER_ENTER || tmp == MATHER_SEMICOLON || tmp == MATHER_EOF)  // 如匹配到stop_token则label的command参数缺省
        goto make;
    else if (!checkToken(pm, MATHER_COLON)){
        syntaxError(pm, syntax_error, line, 1, "Don't get : after goto var");
        goto error_;
    }
    else if (!callChildStatement(CALLPASERSSIGNATURE, parserCommand, COMMAND, &command, "Don't get a label command"))
        goto error_;

    make:
    st = makeLabelStatement(var, command, label, line, pm->file);
    addStatementToken(LABEL, st, pm);
    memFree(label);
    return;

    error_:
    freeStatement(var);
    freeStatement(command);
    memFree(label);
}

/**
 * goto语句匹配
 * parserGoto:
 * | MATHER_GOTO MATHER_AT parserOperation stop_token [缺省所有参数]
 * | MATHER_GOTO MATHER_AT parserOperation MATHER_COLON parserOperation stop_token [缺省return_参数]
 * | MATHER_GOTO MATHER_AT parserOperation MATHER_COLON parserOperation MATHER_COLON parserOperation
 * | MATHER_GOTO MATHER_AT parserOperation MATHER_COLON MATHER_COLON parserOperation [缺省times参数]
 */
void parserGoto(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *label = NULL;
    Statement *times = NULL;
    Statement *return_ = NULL;
    int tmp;
    long int line = delToken(pm);

    if (!checkToken(pm, MATHER_AT)){
        syntaxError(pm, syntax_error, line, 1, "Don't get @ after goto");
        goto error_;
    }
    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &label, "Don't get a goto times"))
        goto error_;

    if ((tmp = readBackToken(pm)) == MATHER_ENTER || tmp == MATHER_SEMICOLON || tmp == MATHER_EOF)  // 如匹配到stop_token则goto后面参数缺省
        goto make;
    else if (!checkToken(pm, MATHER_COLON)){
        syntaxError(pm, syntax_error, line, 1, "Don't get : after goto label");
        goto error_;
    }
    else if (!checkToken(pm, MATHER_COLON) && !callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &times, "Don't get a goto times"))
        goto error_;  // 若再次匹配到 MATHER_COLON 则表示goto的times参数缺省

    if ((tmp = readBackToken(pm)) == MATHER_ENTER || tmp == MATHER_SEMICOLON || tmp == MATHER_EOF)  // 如匹配到stop_token则goto的return_参数缺省
        goto make;
    else if (times != NULL && !checkToken(pm, MATHER_COLON)){
        syntaxError(pm, syntax_error, line, 1, "Don't get : after goto times");
        goto error_;
    }
    else if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &return_, "Don't get a goto return"))
        goto error_;

    make:
    st = makeGotoStatement(return_, times, label, line, pm->file);
    addStatementToken(GOTO, st, pm);
    return;

    error_:
    freeStatement(label);
    freeStatement(times);
    freeStatement(return_);
}

/**
 * import 匹配
 * parserImport
 * | parserControl AS parserOperation
 * @param callBack statement生成函数
 * @param type 输出token的类型
 * @param must_operation 必须匹配 operation
 */
void parserImport(PASERSSIGNATURE) {
    Statement *opt = NULL;
    Statement *st = NULL;
    bool is_lock = false;
    int token_type = readBackToken(pm);
    long int line = delToken(pm);

    if (checkToken(pm, MATHER_COLON)) {
        switch (readBackToken(pm)) {
            case MATHER_PUBLIC:
                break;
            case MATHER_PRIVATE:
            case MATHER_PROTECT:
                is_lock = true;
                break;
            default:
                syntaxError(pm, syntax_error, opt->line, 1, "Don't get a aut token");
                goto return_;
        }
        delToken(pm);
    }

    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &opt, "Don't get a import file"))
        goto return_;
    if (token_type == MATHER_IMPORT) {
        Statement *as = NULL;
        if (checkToken(pm, MATHER_AS) && !callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &as, "Don't get a as after import")) {
            freeStatement(opt);
            goto return_;
        }
        st = makeImportStatement(opt, as, is_lock);
    }
    else{
        Parameter *pt = NULL;
        Parameter *as = NULL;
        if (!checkToken(pm, MATHER_IMPORT)) {
            syntaxError(pm, syntax_error, opt->line, 1, "Don't get a as after import");
            freeStatement(opt);
            goto return_;
        }
        if (checkToken(pm, MATHER_MUL))  // 导入所有
            goto mul_;
        if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, false, false, false, MATHER_COMMA, MATHER_ASSIGNMENT) || pt == NULL) {
            syntaxError(pm, syntax_error, line, 1, "Don't get any value to import");
            freeStatement(opt);
            goto return_;
        }
        if (checkToken(pm, MATHER_AS) && (!parserParameter(CALLPASERSSIGNATURE, &as, false, true, false, false, MATHER_COMMA, MATHER_ASSIGNMENT) || as == NULL)) {
            freeParameter(pt, true);
            syntaxError(pm, syntax_error, opt->line, 1, "Don't get any value after import");
            freeStatement(opt);
            goto return_;
        }
        if (as == NULL && !checkFormal(pt)){
            freeParameter(pt, true);
            syntaxError(pm, syntax_error, opt->line, 1, "Don't get success value to import");
            freeStatement(opt);
            goto return_;
        }

        mul_:
        st = makeFromImportStatement(opt, as, pt, is_lock);
    }

    addStatementToken(IMPORT, st, pm);
    return_:
    return;
}


/**
 * 控制语句匹配
 * parserControl
 * | (control token) NULL
 * | (control token) parserOperation
 * @param callBack statement生成函数
 * @param type 输出token的类型
 * @param must_operation 必须匹配 operation
 */
void parserVarControl(PASERSSIGNATURE) {
    Parameter *var = NULL;
    Statement *st = NULL;
    Token *tmp = NULL;
    int token_type = readBackToken(pm);
    long int line = delToken(pm);
    if (!parserParameter(CALLPASERSSIGNATURE, &var, false, true, true, true, MATHER_COMMA, MATHER_ASSIGNMENT) || var == NULL) {
        syntaxError(pm, syntax_error, line, 1, "Don't get any var");
        goto return_;
    }
    st = makeDefaultVarStatement(var, line, pm->file, token_type == MATHER_DEFAULT ? default_ : token_type == MATHER_GLOBAL ? global_ : nonlocal_);
    addStatementToken(VARCONTROL, st, pm);
    return_:
    return;
}

/**
 * 控制语句匹配
 * parserControl
 * | (control token) NULL
 * | (control token) parserOperation
 * @param callBack statement生成函数
 * @param type 输出token的类型
 * @param must_operation 必须匹配 operation
 */
void parserControl(PASERSSIGNATURE, MakeControlFunction callBack, int type, bool must_operation, char *message) {
    Statement *opt = NULL;
    Statement *st = NULL;
    Token *tmp = NULL;
    long int line = delToken(pm);
    parserOperation(CALLPASERSSIGNATURE);
    if (call_success(pm) && readBackToken(pm) == OPERATION){
        tmp = popNewToken(pm->tm);
        opt = tmp->data.st;
        freeToken(tmp, false);
    }
    else if (must_operation)
        goto error;

    st = callBack(opt, line, pm->file);
    addStatementToken(type, st, pm);
    return;

    error:
    syntaxError(pm, syntax_error, line, 1, message);
}

void parserDo(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *do_code = NULL;
    long int line = delToken(pm);
    if (readBackToken(pm) == MATHER_WHILE){  // do...while语句
        if (!callChildStatement(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st, "Don't get a while code"))
            goto error_;
        st->u.while_branch.type = do_while_;
    }
    else {
        if (!callParserCode(CALLPASERSSIGNATURE, &do_code, "Don't get a if...else code", line))
            goto error_;

        again:
        switch (readBackToken(pm)){
            case MATHER_IF: {
                StatementList *do_sl = NULL;
                if (!callChildStatement(CALLPASERSSIGNATURE, parserIf, IF_BRANCH, &st, "Don't get a if code after do"))
                    goto error_;
                do_sl = makeStatementList(NULL, NULL, do_code, do_b);
                do_sl->next = st->u.if_branch.if_list;
                st->u.if_branch.if_list = do_sl;
                break;
            }
            case MATHER_WHILE:
                if (!callChildStatement(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st, "Don't get a while code after do"))
                    goto error_;
                st->u.while_branch.first = do_code;
                break;
            case MATHER_FOR:
                if (!callChildStatement(CALLPASERSSIGNATURE, parserFor, FOR_BRANCH, &st, "Don't get a for code after do"))
                    goto error_;
                st->u.for_branch.first_do = do_code;
                break;
            case MATHER_DEF:
                if (!callChildStatement(CALLPASERSSIGNATURE, parserDef, FUNCTION, &st, "Don't get a function def after do"))
                    goto error_;
                st->u.set_function.first_do = do_code;
                break;
            case MATHER_DO: {
                long int tmp_line = delToken(pm);
                if (readBackToken(pm) != MATHER_WHILE){
                    syntaxError(pm, syntax_error, tmp_line, 1, "Don't get while after do");
                    goto error_;
                }
                if (!callChildStatement(CALLPASERSSIGNATURE, parserWhile, WHILE_BRANCH, &st, "Don't get a while code"))
                    goto error_;
                st->u.while_branch.type = do_while_;
                st->u.while_branch.first = do_code;
                break;
            }
            case MATHER_ENTER:
                delToken(pm);
                goto again;
            default: {
                Token *tmp = popNewToken(pm->tm);
                syntaxError(pm, syntax_error, tmp->line, 1, "Get don't support branch after do");
                backToken_(pm, tmp);
                goto error_;
            }
        }
    }

    addStatementToken(DO_BRANCH, st, pm);
    return;

    error_:
    freeStatement(do_code);
    freeStatement(st);
}

void parserFor(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    Statement *do_st = NULL;
    StatementList *sl = NULL;
    long int line = delToken(pm);
    {
        Statement *code_tmp = NULL, *var_tmp = NULL, *iter_tmp = NULL;
        if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &var_tmp, "Don't get a for var"))
            goto error_;
        if (!checkToken(pm, MATHER_IN)){
            freeStatement(var_tmp);
            syntaxError(pm, syntax_error, line, 1, "Don't get in after for");
            goto error_;
        }
        if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &iter_tmp, "Don't get a for condition")) {
            freeStatement(var_tmp);
            goto error_;
        }
        if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a for code", line)) {
            freeStatement(iter_tmp);
            freeStatement(var_tmp);
            goto error_;
        }
        sl = makeStatementList(iter_tmp, var_tmp, code_tmp, for_b);
    }

    again:
    switch (readBackToken(pm)) {
        case MATHER_DO: {
            if (do_st != NULL || else_st != NULL)
                goto default_;
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &do_st, "Don't get a for...do code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_ELSE: {
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get else after else\n");
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a for...else code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_FINALLY: {
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a for...finally code", tmp_line))
                goto error_;
            break;
        }
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

    st = makeForStatement(line, pm->file);
    st->u.for_branch.for_list = sl;
    st->u.for_branch.else_list = else_st;
    st->u.for_branch.finally = finally_st;
    st->u.for_branch.after_do = do_st;
    addStatementToken(FOR_BRANCH, st, pm);
    return;

    error_:
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatement(do_st);
    freeStatementList(sl);
}

void parserWith(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *code_tmp = NULL;
    Statement *var_tmp = NULL;
    Statement *condition_tmp = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    StatementList *sl = NULL;
    long int line = 0;
    long int tmp_line;

    line = delToken(pm);
    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, "Don't get a with operation"))
        goto error_;
    if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a with var"))
        goto error_;
    if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a with code", line))
        goto error_;
    sl = connectStatementList(sl, makeStatementList(condition_tmp, var_tmp, code_tmp, with_b));
    condition_tmp = NULL;
    var_tmp = NULL;
    code_tmp = NULL;

    for (int tk=readBackToken(pm); tk == MATHER_ENTER; tk = readBackToken(pm))
        delToken(pm);

    if (readBackToken(pm) == MATHER_ELSE) {
        tmp_line = delToken(pm);
        if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a with...else code", tmp_line))
            goto error_;
    }

    for (int tk=readBackToken(pm); tk == MATHER_ENTER; tk = readBackToken(pm))
        delToken(pm);

    if (readBackToken(pm) == MATHER_FINALLY) {
        tmp_line = delToken(pm);
        if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a wilt...finally code", tmp_line))
            goto error_;
    }

    addLexToken(pm, MATHER_ENTER);
    st = makeWithStatement(line, pm->file);
    st->u.with_branch.with_list = sl;
    st->u.with_branch.else_list = else_st;
    st->u.with_branch.finally = finally_st;
    addStatementToken(WITH_BRANCH, st, pm);
    return;

    error_:
    freeStatement(condition_tmp);
    freeStatement(var_tmp);
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatementList(sl);
}

/**
 * 条件分支匹配
 * parserIf:
 * | MATHER_IF parserOperation (MATHER_AS parserOperation) callParserCode
 * | parserIf MATHER_ELIF  parserOperation (MATHER_AS parserOperation) callParserCode
 * | parserIf MATHER_DO callParserCode [不允许两次连用]
 * | parserIf MATHER_ELSE callParserCode [不允许再出现if和elif以及do] [只出现一次] [4]
 * | parserIf MATHER_FINALLY callParserCode [结尾]
 * 注释：会自动过滤 <ENTER> , 可以使用 <SEMICOLON> 作为结束表示
 * 注释：自动添加 <ENTER> 结尾符号
 * 注释：(MATHER_AS parserOperation) 表示允许出现或者不出现, 若出现则 MATHER_AS parserOperation 必须一起出现
 * 特别注意：自进入模式[4]后，匹配顺序自上而下不可逆
 * @param pm
 * @param inter
 */
void parserIf(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    StatementList *sl = NULL;
    bool have_if = false;
    long int line = 0;
    again:
    switch (readBackToken(pm)) {
        case MATHER_IF:
            if (have_if)
                goto default_;
            else
                have_if = true;
            line = delToken(pm);
            goto not_del;
        case MATHER_ELIF: {
            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get elif after else");
                goto error_;
            }

            not_del:
            if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, "Don't get a if condition"))
                goto error_;

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a while var")) {
                freeStatement(condition_tmp);
                goto error_;
            }

            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a if code", tmp_line)) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            sl = connectStatementList(sl, makeStatementList(condition_tmp, var_tmp, code_tmp, if_b));
            goto again;
        }
        case MATHER_DO: {
            if (else_st != NULL)
                goto default_;
            Statement *code_tmp = NULL;
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a if...do code", tmp_line))
                goto error_;
            sl = connectStatementList(sl, makeStatementList(NULL, NULL, code_tmp, do_b));
            goto again;
        }
        case MATHER_ELSE: {
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get else after else");
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a if...else code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_FINALLY: {
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a if...finally code", tmp_line))
                goto error_;
            break;
        }
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

    st = makeIfStatement(line, pm->file);
    st->u.if_branch.if_list = sl;
    st->u.if_branch.else_list = else_st;
    st->u.if_branch.finally = finally_st;
    addStatementToken(IF_BRANCH, st, pm);
    return;

    error_:
    freeStatement(else_st);
    freeStatement(finally_st);
    freeStatementList(sl);
}

/**
 * 条件循环匹配
 * parserWhile:
 * | MATHER_WHILE parserOperation (MATHER_AS parserOperation) callParserCode
 * | parserWhile MATHER_DO callParserCode [只出现一次]
 * | parserWhile MATHER_ELSE callParserCode [只出现一次]
 * | parserWhile MATHER_FINALLY callParserCode [结尾]
 * 注释：同 ``parserIf``
 * 特别注意：匹配顺序自上而下不可逆
 * @param pm
 * @param inter
 */
void parserWhile(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    Statement *do_st = NULL;
    StatementList *sl = NULL;
    bool have_while = false;
    long int line = 0;
    again:
    switch (readBackToken(pm)) {
        case MATHER_WHILE: {
            if (have_while)
                goto default_;
            else
                have_while = true;

            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            line = delToken(pm);
            if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, "Don't get a while condition"))
                goto error_;

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a while var")){
                freeStatement(condition_tmp);
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a while code", line)) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            if (sl != NULL)
                freeStatementList(sl);
            sl = makeStatementList(condition_tmp, var_tmp, code_tmp, while_b);
            goto again;
        }
        case MATHER_DO: {
            if (do_st != NULL || else_st != NULL)
                goto default_;
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &do_st, "Don't get a while...do code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_ELSE: {
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get else after else\n");
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a while...else code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_FINALLY: {
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a while...finally code", tmp_line))
                goto error_;
            break;
        }
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

    st = makeWhileStatement(line, pm->file);
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
}

/**
 * 异常捕获分支匹配
 * parserTry:
 * | MATHER_TRY callParserCode
 * | parserTry MATHER_EXCEPT (MATHER_AS parserOperation) callParserCode
 * | parserTry MATHER_ELSE callParserCode [只出现一次]
 * | parserTry MATHER_FINALLY callParserCode [结尾]
 * 注释：同 ``parserIf``
 * 特别注意：匹配顺序自上而下不可逆
 * @param pm
 * @param inter
 */
void parserTry(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *try_st = NULL;
    Statement *else_st = NULL;
    Statement *finally_st = NULL;
    StatementList *sl = NULL;
    long int line = 0;
    again:
    switch (readBackToken(pm)) {
        case MATHER_TRY:{
            if (try_st != NULL)
                goto default_;
            line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &try_st, "Don't get a try code", line))
                goto error_;
            goto again;
        }
        case MATHER_EXCEPT: {
            Statement *code_tmp = NULL, *var_tmp = NULL, *condition_tmp = NULL;
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get except after else");
                goto error_;
            }
            if (readBackToken(pm) != MATHER_LC)
                callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &condition_tmp, NULL);

            if (!callParserAs(CALLPASERSSIGNATURE, &var_tmp, "Don't get a except var")){
                freeStatement(condition_tmp);
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a except code", tmp_line)) {
                freeStatement(condition_tmp);
                freeStatement(var_tmp);
                goto error_;
            }
            sl = connectStatementList(sl, makeStatementList(condition_tmp, var_tmp, code_tmp, except_b));
            goto again;
        }
        case MATHER_ELSE: {
            long int tmp_line = delToken(pm);
            if (else_st != NULL) {
                syntaxError(pm, syntax_error, tmp_line, 1, "get else after else");
                goto error_;
            }
            if (!callParserCode(CALLPASERSSIGNATURE, &else_st, "Don't get a try...else code", tmp_line))
                goto error_;
            goto again;
        }
        case MATHER_FINALLY: {
            long int tmp_line = delToken(pm);
            if (!callParserCode(CALLPASERSSIGNATURE, &finally_st, "Don't get a try...finally code", tmp_line))
                goto error_;
            break;
        }
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

    st = makeTryStatement(line, pm->file);
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
}

/**
 * 函数定义匹配
 * parserDef:
 * | parserBaseValue MATHER_LP parserParameter(is_formal) MATHER_RP callParserCode
 * 注释：自动添加 <ENTER> 结尾符号
 * @param pm
 * @param inter
 */
void parserDef(PASERSSIGNATURE){
    Statement *st = NULL;
    Statement *name_tmp = NULL;
    Statement *code_tmp = NULL;
    Parameter *pt = NULL;
    int type = readBackToken(pm);
    long int line = delToken(pm);

    if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &name_tmp,
                            "Don't get a function/class name"))
        goto error_;

    if (!checkToken(pm, MATHER_LP))
        goto get_code;
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, true, true, false, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        lexEnter(pm, false);
        syntaxError(pm, syntax_error, line, 1, "Don't get a function/class parameter");
        goto error_;
    }
    if (!checkToken(pm, MATHER_RP)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get a function/class ) after parameter");
        goto error_;
    }
    get_code:
    if (!callParserCode(CALLPASERSSIGNATURE, &code_tmp, "Don't get a function code", line)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get a function code");
        goto error_;
    }

    if (type == MATHER_DEF)
        st = makeFunctionStatement(name_tmp, code_tmp, pt);
    else
        st = makeClassStatement(name_tmp, code_tmp, pt);
    addLexToken(pm, MATHER_ENTER);
    addStatementToken(FUNCTION, st, pm);
    return;

    error_:
    freeStatement(name_tmp);
    freeStatement(code_tmp);
    freeParameter(pt, true);
}

/**
 * 函数定义匹配
 * parserCode:
 * | MATHER_LC parserCommandList MATHER_RC
 * 注释：自动忽略MATHER_LC前的空格
 * @param pm
 * @param inter
 */
void parserCode(PASERSSIGNATURE) {
    long int line = 0;
    Statement *st = makeStatement(line, pm->file);
    while (true){
        if (readBackToken(pm) != MATHER_LC)
            goto again_;
        line = delToken(pm);
        break;
        again_:
        if (!checkToken(pm, MATHER_ENTER))
            goto return_;
    }
    parserCommandList(CALLPASERSSIGNATURE, false, false, st);
    if (!call_success(pm))
        goto error_;

    if (!checkToken(pm, MATHER_RC)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get the }");  // 使用{的行号
        goto error_;
    }

    return_:
    addStatementToken(CODE, st, pm);
    return;

    error_:
    freeStatement(st);
}

/**
 * 表达式匹配
 * parserOperation：
 * | parserAssignment
 */
void parserOperation(PASERSSIGNATURE){
    Statement *operation_st = NULL;
    if (!callChildStatement(CALLPASERSSIGNATURE, parserAssignment, ASSIGNMENT, &operation_st, NULL))
        goto return_;
    addStatementToken(OPERATION, operation_st, pm);
    return_:
    return;
}

/**
 * 赋值表达式匹配
 * parserAssignment:
 * | parserTuple
 * | parserAssignment ASSIGNMENT parserTuple [2]
 * 注意：在链接statement的时候, 模式[2]相当于 parserTuple ASSIGNMENT parserAssignment
 */
bool checkAssignmentLeft(PASERSSIGNATURE, Statement *left){
    if (left->type == call_function && !checkFormal(left->u.call_function.parameter)){
        syntaxError(pm, syntax_error, left->line, 1, "Don't get success function definition from Assignment");
        return false;
    }
    return true;
}

bool switchAssignment(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {  // 此处保持使用switch分支
        case MATHER_ASSIGNMENT:
            *st = makeOperationBaseStatement(OPT_ASS, 0, pm->file);
            break;
        default:
            return false;
    }
    return true;
}
void parserAssignment(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserTuple, switchAssignment, checkAssignmentLeft, TUPLE, ASSIGNMENT,
                        "polynomial", "assignment", true);
}

/**
 * 元组匹配
 * parserTuple:
 * | parserPolynomial
 * | parserTuple COMMA parserPolynomial
 * @param pm
 * @param inter
 */
void parserTuple(PASERSSIGNATURE){
    Parameter *pt = NULL;
    Statement *st = NULL;
    Token *tmp = NULL;
    long int line = 0;
    if (readBackToken(pm) == MATHER_MUL) {
        line = pm->tm->ts->token_list->line;
        goto parserPt;
    }

    if (!callChildToken(CALLPASERSSIGNATURE, parserPolynomial, POLYNOMIAL, &tmp, NULL, syntax_error))
        goto return_;
    if (readBackToken(pm) != MATHER_COMMA){
        tmp->token_type = TUPLE;
        addToken_(pm ,tmp);
        goto return_;
    }
    line = tmp->line;
    addToken_(pm ,tmp);

    parserPt:
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, false, true, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get tuple element");
        goto return_;
    }
    st = makeTupleStatement(pt, value_tuple, pt->data.value->line, pm->file);
    addStatementToken(TUPLE, st, pm);

    return_:
    return;
}

/**
 * 多项式匹配
 * parserPolynomial:
 * | parserBaseValue
 * | parserPolynomial OPT_ADD parserFactor
 * | parserPolynomial OPT_SUB parserFactor
 */
bool switchPolynomial(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {
        case MATHER_ADD:
            *st = makeOperationBaseStatement(OPT_ADD, 0, pm->file);
            break;
        case MATHER_SUB:
            *st = makeOperationBaseStatement(OPT_SUB, 0, pm->file);
            break;
        default:
            return false;
    }
    return true;
}
void parserPolynomial(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserFactor, switchPolynomial, NULL, FACTOR, POLYNOMIAL,
                        "factor", "polynomial", false);
}

/**
 * 因式匹配
 * parserFactor:
 * | parserCallBack
 * | switchFactor OPT_ADD parserCallBack
 * | switchFactor OPT_SUB parserCallBack
 */
bool switchFactor(PASERSSIGNATURE, int symbol, Statement **st){
    switch (symbol) {
        case MATHER_MUL:
            *st = makeOperationBaseStatement(OPT_MUL, 0, pm->file);
            break;
        case MATHER_DIV:
            *st = makeOperationBaseStatement(OPT_DIV, 0, pm->file);
            break;
        default:
            return false;
    }
    return true;
}
void parserFactor(PASERSSIGNATURE){
    return twoOperation(CALLPASERSSIGNATURE, parserCallBack, switchFactor, NULL, CALLBACK, FACTOR,
                        "call back", "factor", false);
}

/**
 * 函数回调匹配
 * parserCallBack：
 * | parserBaseValue
 * | parserCallBack MATHER_LP parserParameter MATHER_RP
 */
bool tailCall(PASERSSIGNATURE, Token *left_token, Statement **st){
    Parameter *pt = NULL;
    long int line = delToken(pm);

    if (checkToken(pm, MATHER_RP))
        goto not_pt;
    if (!parserParameter(CALLPASERSSIGNATURE, &pt, true, false, false, false, MATHER_COMMA, MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get call parameter");
        return false;
    }
    if (!checkToken(pm, MATHER_RP)) {
        freeParameter(pt, true);
        syntaxError(pm, syntax_error, line, 1, "Don't get ) from call back");
        return false;
    }

    not_pt:
    *st = makeCallStatement(left_token->data.st, pt);
    return true;
}

bool tailSlice(PASERSSIGNATURE, Token *left_token, Statement **st){
    Parameter *pt = NULL;
    Token *tmp = NULL;
    enum SliceType type;  // 0-slice  1-down
    long int line = delToken(pm);

    if (!callChildToken(CALLPASERSSIGNATURE, parserPolynomial, POLYNOMIAL, &tmp, "Don't get slice/down element", syntax_error))
        return false;
    else if (readBackToken(pm) == MATHER_COLON)
        type = SliceType_slice_;
    else
        type = SliceType_down_;
    line = tmp->line;
    addToken_(pm ,tmp);

    if (!parserParameter(CALLPASERSSIGNATURE, &pt, true, true, true, true, (type == SliceType_down_ ? MATHER_COMMA : MATHER_COLON), MATHER_ASSIGNMENT)) {
        syntaxError(pm, syntax_error, line, 1, "Don't get slice element");
        return false;
    }
    if (!checkToken(pm, MATHER_RB)){
        freeParameter(pt, true);
        syntaxError(pm, syntax_error, line, 1, "Don't get ] from slice");
        return false;
    }
    *st = makeSliceStatement(left_token->data.st, pt, type);
    return true;
}

bool taliPoint(PASERSSIGNATURE, Token *left_token, Statement **st){
    Statement *right_st = NULL;
    delToken(pm);
    if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &right_st, "Don't get a BaseValue after point"))
        return false;
    *st = makeOperationStatement(OPT_POINT, left_token->data.st, right_st);
    return true;
}

bool taliLink(PASERSSIGNATURE, Token *left_token, Statement **st){
    Statement *right_st = NULL;
    delToken(pm);
    if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &right_st, "Don't get a BaseValue after link"))
        return false;
    *st = makeOperationStatement(OPT_LINK, left_token->data.st, right_st);
    return true;
}

void parserCallBack(PASERSSIGNATURE){
    while(true){
        int tk;
        Token *left_token = NULL;
        struct Statement *st = NULL;

        if (readBackToken(pm) != CALLBACK){

            if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &st, NULL))
                goto return_;
            addStatementToken(CALLBACK, st, pm);
            continue;
        }
        left_token = popNewToken(pm->tm);

        tk = readBackToken(pm);
        if (tk == MATHER_LB && !tailSlice(CALLPASERSSIGNATURE, left_token, &st) ||
            tk == MATHER_LP && !tailCall(CALLPASERSSIGNATURE, left_token, &st) ||
            tk == MATHER_POINT && !taliPoint(CALLPASERSSIGNATURE, left_token, &st) ||
            tk == MATHER_LINK && !taliLink(CALLPASERSSIGNATURE, left_token, &st)) {
            freeToken(left_token, true);
            goto return_;
        } else if (tk != MATHER_LB && tk != MATHER_LP && tk != MATHER_POINT && tk != MATHER_LINK) {
            backToken_(pm, left_token);
            goto return_;
        }
        addStatementToken(CALLBACK, st, pm);
        freeToken(left_token, false);
    }
    return_: return;
}

/**
 * 字面量匹配
 * parserBaseValue：
 * | MATHER_NUMBER
 * | MATHER_STRING
 * | MATHER_VAR
 * | MATHER_LAMBDA parserParameter MATHER_COLON parserOperation
 * | MATHER_LP parserOperation MATHER_LP
 * | MATHER_LP parserOperation MATHER_LP -> list
 * | MATHER_LP parserOperation MATHER_LP MATHER_VAR
 * | MATHER_LC parserParameter(dict) MATHER_LC
 */
int getOperation(PASERSSIGNATURE, int right_type, Statement **st, char *name){
    *st = NULL;
    if (checkToken(pm, right_type))
        goto return_;

    if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, st, NULL))
        return 0;

    if (!checkToken(pm, right_type)){
        freeStatement(*st);
        return -1;
    }

    return_:
    return 1;
}

void parserBaseValue(PASERSSIGNATURE){
    Token *value_token = popNewToken(pm->tm);
    Statement *st = NULL;
    switch (value_token->token_type) {
        case MATHER_NUMBER : {
            Statement *tmp = NULL;
            tmp = makeBaseStrValueStatement(value_token->data.str, number_str, value_token->line, pm->file);
            if (*value_token->data.second_str == NUL)
                st = tmp;
            else {
                Statement *second_var = makeBaseVarStatement(value_token->data.second_str, NULL, value_token->line,
                                                             pm->file);
                st = makeCallStatement(second_var, makeValueParameter(tmp));
            }
            break;
        }

        case MATHER_STRING:{
            Statement *tmp = NULL;
            tmp = makeBaseStrValueStatement(value_token->data.str, string_str, value_token->line, pm->file);
            if (*value_token->data.second_str == NUL)
                st = tmp;
            else {
                Statement *sencod_var = makeBaseVarStatement(value_token->data.second_str, NULL, value_token->line,
                                                             pm->file);
                st = makeCallStatement(sencod_var, makeValueParameter(tmp));
            }
            break;
        }
        case MATHER_TRUE:
            st = makeBaseValueStatement(bool_true, value_token->line, pm->file);
            break;
        case MATHER_FALSE:
            st = makeBaseValueStatement(bool_false, value_token->line, pm->file);
            break;
        case MATHER_NULL:
            st = makeBaseValueStatement(null_value, value_token->line, pm->file);
            break;
        case MATHER_PASSVALUE:
            st = makeBaseValueStatement(pass_value, value_token->line, pm->file);
            break;
        case MATHER_LAMBDA:  {
            Parameter *pt = NULL;
            Statement *lambda_st = NULL;
            if (!parserParameter(CALLPASERSSIGNATURE, &pt, false, true, false, false, MATHER_COMMA,
                                 MATHER_ASSIGNMENT)) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get a lambda parameter");
                goto return_;
            }
            if (!checkToken(pm, MATHER_COLON)) {
                lambda_st = makeStatement(value_token->line, pm->file);
                goto not_lambda_st;
            }
            if (!callChildStatement(CALLPASERSSIGNATURE, parserOperation, OPERATION, &lambda_st,
                                    "Don't get a lambda operation")) {
                freeToken(value_token, true);
                goto return_;
            }
            not_lambda_st:
            st = makeLambdaStatement(lambda_st, pt);
            break;
        }
        case MATHER_VAR:
            st = makeBaseVarStatement(value_token->data.str, NULL, value_token->line, pm->file);
            break;
        case MATHER_SVAR: {
            Statement *svar_st = NULL;
            if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &svar_st, NULL)) {
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get super var after $");
                freeToken(value_token, true);
                goto return_;
            }
            st = makeBaseSVarStatement(svar_st, NULL);
            break;
        }
        case MATHER_LB: {
            int tmp;
            Statement *tmp_st = NULL;
            lexEnter(pm, true);
            tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RB, &tmp_st, "base value");
            lexEnter(pm, false);
            if (tmp == 0) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get operation from Base Value");
                goto return_;
            } else if (tmp == -1) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get ] from list/var");
                goto return_;  // 优化goto return freeToken
            }
            if (MATHER_VAR == readBackToken(pm)) {
                Token *var_token;
                var_token = popNewToken(pm->tm);
                st = makeBaseVarStatement(var_token->data.str, tmp_st, var_token->line, pm->file);
                freeToken(var_token, false);
            } else {
                if (tmp_st == NULL)
                    st = makeTupleStatement(NULL, value_list, value_token->line, pm->file);
                else if (tmp_st->type == base_list && tmp_st->u.base_list.type == value_tuple) {
                    tmp_st->u.base_list.type = value_list;
                    st = tmp_st;
                } else
                    st = makeTupleStatement(makeValueParameter(tmp_st), value_list, value_token->token_type, pm->file);
            }
            break;
        }
        case MATHER_LP: {
            int tmp;
            lexEnter(pm, true);
            tmp = getOperation(CALLPASERSSIGNATURE, MATHER_RP, &st, "base value");
            lexEnter(pm, false);
            if (tmp == 0) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get operation from Base Value");
                goto return_;
            } else if (tmp == -1) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get ) from Base Value");
                goto return_;
            }
            break;
        }
        case MATHER_LC: {
            Parameter *pt = NULL;
            int parser_status;
            parser_status = parserParameter(CALLPASERSSIGNATURE, &pt, true, false, false, true, MATHER_COMMA,
                                            MATHER_COLON);
            if (!parser_status) {
                freeToken(value_token, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get a dict parameter");
                goto return_;
            }
            if (!checkToken(pm, MATHER_RC)) {
                freeToken(value_token, true);
                freeParameter(pt, true);
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get a } after dict");
                goto return_;
            }
            st = makeBaseDictStatement(pt, value_token->line, pm->file);
            break;
        }
        case MATHER_BLOCK: {
            Statement *block = NULL;
            if (!callParserCode(CALLPASERSSIGNATURE, &block, "Don't get a while code", value_token->line)) {
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get block command");
                freeToken(value_token, true);
                goto return_;
            }
            st = makeOperationStatement(OPT_BLOCK, block, NULL);
            break;
        }
        case MATHER_PRIVATE:
        case MATHER_PROTECT:
        case MATHER_PUBLIC: {
            if (MATHER_COLON != readBackToken(pm)) {
                syntaxError(pm, syntax_error, value_token->line, 1, "Don't get a : after aut token");
                freeToken(value_token, true);
                goto return_;
            }
            delToken(pm);
            if (!callChildStatement(CALLPASERSSIGNATURE, parserBaseValue, BASEVALUE, &st, "Don't get Base Value after aut token")) {
                freeToken(value_token, true);
                goto return_;
            }
            switch (value_token->token_type) {
                case MATHER_PROTECT:
                    st->aut = protect_aut;
                    break;
                case MATHER_PRIVATE:
                    st->aut = private_aut;
                    break;
                case MATHER_PUBLIC:
                    st->aut = public_aut;
                    break;
            }
            break;
        }
        default:{
            backToken_(pm, value_token);
            goto return_;
        }
    }
    freeToken(value_token, false);
    addStatementToken(BASEVALUE, st, pm);

    return_: return;
}
