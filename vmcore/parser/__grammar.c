#include "__grammar.h"
/**
 * 二元匹配器
 * twoOperation:
 * | callBack
 * | twoOperation getSymbol callBack
 * @param callBack 符号左、右值匹配函数
 * @param getSymbol 符号处理函数
 * @param call_type 左、右值类型
 * @param self_type 输出token的类型
 * @param call_name 左、右值名称(log)
 * @param self_name 输出值名称(log)
 * @param is_right 表达式是否从右运算到左
 */

inline void twoOperation(P_FUNC, PasersFunction callBack, GetSymbolFunction getSymbol, ChecktLeftToken checkleft,
                         int call_type, int self_type, char *call_name, char *self_name, bool is_right) {
    bool is_right_ = false;
    while(true){
        Token *left_token = NULL;
        Token *right_token = NULL;
        Statement *st = NULL;
        long int line;

        if (readBackToken(pm) != self_type) {
            if (!callChildStatement(CP_FUNC, callBack, call_type, &st, NULL))
                goto return_;
            addStatementToken(self_type, st, pm);
            continue;
        }
        left_token = popNewToken(pm->tm);
        line = left_token->line;

        if (getSymbol(CP_FUNC, readBackToken(pm), &st))
            delToken(pm);
        else{
            backToken_(pm, left_token);
            goto return_;
        }

        if (checkleft != NULL && !checkleft(CP_FUNC, left_token->data.st)) {
            freeToken(left_token, true);
            goto return_;
        }

        callBack(CP_FUNC);  // 获得右值
        if (!call_success(pm) || readBackToken(pm) != call_type){  // 若非正确数值
            syntaxError(pm, syntax_error, line, 5, "ERROR from ", self_name, "(get right ", call_name, ")");
            freeToken(left_token, true);
            freeStatement(st);
            goto return_;
        }
        right_token = popNewToken(pm->tm);
        addToken_(pm, setOperationFromToken(&st, left_token, right_token, self_type, is_right_));
        is_right_ = is_right;  // 第一次is_right不生效
    }
    return_: return;
}

/**
 * syntax错误处理器
 * @param pm
 * @param message 错误信息
 * @param status 错误类型
 */
void syntaxError(ParserMessage *pm, int status, long int line, int num, ...) {
    char *message = NULL;

    if (pm->status != success)
        return;
    if (status <= 0){
        message = memStrcpy("Not Message");
        goto not_message;
    }

    va_list message_args;
    va_start(message_args, num);
    for (int i=0; i < num; i++)
        message = memStrcat(message, va_arg(message_args, char *), true, false);
    va_end(message_args);

    char info[100];
    snprintf(info, 100, "\non line %ld\nin file ", line);
    message = memStrcat(message, info, true, false);
    message = memStrcat(message, pm->file, true, false);

    not_message:
    pm->status = status;
    pm->status_message = message;
}

int readBackToken(ParserMessage *pm){
    Token *tmp = popNewToken(pm->tm);
    int type = tmp->token_type;
    if (type == -2)
        syntaxError(pm, lexical_error, tmp->line, 1, "lexical make some error");
    else if (type == -3)
        syntaxError(pm, int_error, tmp->line, 1, "KeyInterrupt");
    addBackToken(pm->tm->ts, tmp);
    return type;
}

bool checkToken(ParserMessage *pm, int type){
    if (readBackToken(pm) != type)
        return false;
    delToken(pm);
    return true;
}

bool commandCallControl_(P_FUNC, MakeControlFunction callBack, int type, Statement **st, bool must_operation, char *error_message) {
    Token *tmp_token = NULL;
    *st = NULL;
    parserControl(CP_FUNC, callBack, type, must_operation, error_message);
    if (!call_success(pm) || readBackToken(pm) != type)
        return false;
    tmp_token = popNewToken(pm->tm);
    *st = tmp_token->data.st;
    freeToken(tmp_token, false);
    return true;
}
bool callParserCode(P_FUNC, Statement **st, char *message, long int line) {
    Token *tmp;
    *st = NULL;
    parserCode(CP_FUNC);
    if (!call_success(pm) || readBackToken(pm) != T_CODE) {
        if (message != NULL)
            syntaxError(pm, syntax_error, line, 1, message);
        return false;
    }
    tmp = popNewToken(pm->tm);
    *st = tmp->data.st;
    freeToken(tmp, false);
    return true;
}

bool callParserAs(P_FUNC, Statement **st, char *message){
    *st = NULL;
    if (readBackToken(pm) == MATHER_AS) {
        delToken(pm);
        return callChildStatement(CP_FUNC, parserOperation, T_OPERATION, st, message);
    }
    return true;
}

bool callChildToken(P_FUNC, PasersFunction callBack, int type, Token **tmp, char *message, int error_type) {
    *tmp = NULL;
    callBack(CP_FUNC);
    if (!call_success(pm) || readBackToken(pm) != type) {
        if (message != NULL) {
            *tmp = popNewToken(pm->tm);
            syntaxError(pm, error_type, (*tmp)->line, 1, message);
            backToken_(pm, (*tmp));
        }
        return false;
    }
    *tmp = popNewToken(pm->tm);
    return true;
}

bool callChildStatement(P_FUNC, PasersFunction callBack, int type, Statement **st, char *message){
    Token *tmp = NULL;
    *st = NULL;
    bool status = callChildToken(CP_FUNC, callBack, type, &tmp, message, syntax_error);
    if (!status)
        return false;
    *st = tmp->data.st;
    freeToken(tmp, false);
    return true;
}

/**
 * is_dict的默认模式为 s_2 ，一般情况默认模式为 s_1
 * 若获得MUL则进入模式 s_3, 若获得POW则进入模式 s_4
 * get operation [1]
 * 若模式为 s_1
 *  - 检查是否为sep符号
 *      - 若不是sep符号则检查是否为ass符号
 *          - 若是ass符号则进入 s_2 模式
 *          - 若不是ass符号则标注该参数为最后匹配参数
 *      - 若是sep符号则保持 s_1 模式
 * 若模式为 s_2
 *  - 检查是否为ass符号
 *      - 若不是ass符号则报错
 *      - 若是ass符号则保持 s_2 模式
 * 若模式为 s_3 / s_4
 *  - 检查是否为sep符号
 *      - 若不是sep符号则标注该参数为最后匹配参数
 *      - 若是sep则保持 s_3 / s_4 模式
 * ... 合成 Parameter 并且链接 ...
 * 重复操作
 *
 * @param is_formal 是否为形式参数, 若为true，则限定*args为only_value的结尾, **kwargs为name_value结尾
 * @param is_list 若为true则关闭对name_value和**kwargs的支持
 * @param is_dict 若为true则关闭对only_value和*args的支持  (is_list和is_dict同时为true表示纯 a,b,c 匹配)
 * @param sep 设定分割符号
 * @param ass 设定赋值符号
 * @return
 */
bool parserParameter(P_FUNC, Parameter **pt, bool enter, bool is_formal, bool is_list, bool is_dict,
                     int sep, int ass, int n_sep) {
    Parameter *new_pt = NULL;
    Token *tmp;
    bool last_pt = false;
    int is_sep = 0;  // 0: 不需要处理 1: 是is_sep 2: 处理过is_sep
    enum {
        s_1,  // only_value模式
        s_2,  // name_value模式
        s_3,  // only_args模式
        s_4,  // name_args模式
    } status;

    if (enter)
        lexEnter(pm, true);
    if (is_dict && !is_list)
        status = s_2;  // is_formal关闭对only_value的支持
    else
        status = s_1;

    for (int count = 0; !last_pt; count++){  // 计算匹配到parameter的个数
        tmp = NULL;
        if (is_sep == 1 || !is_formal && count > 1)  // 限制实参的;分隔符前最多只有两个参数
            is_sep = 2;
        if (!is_dict && status != s_2 && checkToken(pm, MATHER_MUL))  // is_formal关闭对*args的支持
            status = s_3;
        else if (!is_list && checkToken(pm, MATHER_POW))  // is_formal关闭对*args的支持
            status = s_4;

        parserOr(CP_FUNC);
        if (!call_success(pm))
            goto error_;
        if (readBackToken(pm) != T_OR) {
            if (status == s_3) {
                long int line = pm->tm->ts->token_list->line;
                syntaxError(pm, syntax_error, line, 1, "Don't get a parameter after *");
                goto error_;
            }
            break;
        }
        tmp = popNewToken(pm->tm);
        int pt_type = value_par;
        if (status == s_1){
            if (!checkToken(pm, sep)){
                if (is_sep == 0 && n_sep != -1 && checkToken(pm, n_sep))
                    is_sep = 1;
                else if (is_list || !checkToken(pm, ass))  // // is_list关闭对name_value的支持
                    last_pt = true;
                else {
                    pt_type = name_par;
                    status = s_2;
                }
            }
        }
        else if (status == s_2){
            pt_type = name_par;
            if (!checkToken(pm, ass))
                goto error_;
        }
        else if (status == s_3){
            pt_type = args_par;
            if (!checkToken(pm, sep))
                last_pt = true;
        }
        else {
            pt_type = kwargs_par;
            if (!checkToken(pm, sep))
                last_pt = true;
        }

        if (pt_type == value_par)
            new_pt = connectValueParameter(tmp->data.st, new_pt, is_sep == 1);
        else if (pt_type == name_par){
            Statement *tmp_value;
            if (!callChildStatement(CP_FUNC, parserOr, T_OR, &tmp_value, "Don't get a parameter value"))
                goto error_;
            new_pt = connectNameParameter(tmp_value, tmp->data.st, new_pt);
            if (!checkToken(pm, sep))
                last_pt = true;
        }
        else if (pt_type == args_par){
            new_pt = connectArgsParameter(tmp->data.st, new_pt, is_sep == 1);
            if (is_formal)
                status = s_2;  // 是否规定*args只出现一次
            else
                status = s_1;
        }
        else {
            new_pt = connectKwargsParameter(tmp->data.st, new_pt);
            if (is_formal)
                last_pt = true; // 是否规定**kwargs只出现一次
            else
                status = s_2;
        }
        freeToken(tmp, false);
    }
    *pt = new_pt;
    if (enter)
        lexEnter(pm, false);
    return true;

    error_:
    freeToken(tmp, true);
    freeParameter(new_pt, true);
    *pt = NULL;
    if (enter)
        lexEnter(pm, false);
    return false;
}

void lexEnter(ParserMessage *pm, bool lock){
    if (lock)
        pm->tm->file->filter_data.enter ++;
    else
        pm->tm->file->filter_data.enter --;
}
