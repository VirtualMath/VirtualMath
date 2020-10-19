#include "__virtualmath.h"

/**
 * 匹配一个数字字面量
 * 匹配器规则：
 * START模式：判断比较第一个字符(是否为数字或者小数点)，若匹配成功则进入ING模式，若失败则进入MISTAKE模式
 * ING模式：继续匹配，直到遇到非数字或小数点。则检查是否为英文字母，若是则进入SECOND模式，否则进入END模式
 * SECOND模式：继续匹配，知道遇到非字母、下划线、数字的内容，进入END模式
 * END模式：进入END模式意味着匹配结束了，通过checkoutMather可以检查该匹配器是否被采用，采用后则生成token，并且读取器回退一个字符
 * MISTAKE模式：错误
 * 匹配内容：12.3jk_2，其中12.3存储在str中，jk_2存储在str_second中
 * @param p
 * @param mather
 */
void numberMather(wint_t p, LexMather *mather){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING_1 || mather->status == LEXMATHER_ING_2)
        if (iswdigit(p) || L'.' == p && mather->status == LEXMATHER_ING_1){  // LEXMATHER_ING_1: 整数匹配
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len += 1;
            if (L'.' == p)
                mather->status = LEXMATHER_ING_2;
            else if (mather->status == LEXMATHER_START)
                mather->status = LEXMATHER_ING_1;
        }
        else if(mather->status == LEXMATHER_ING_1 || mather->status == LEXMATHER_ING_2){  // LEXMATHER_ING_2: 小数匹配
            if (iswalpha(p) || L'_' == p){
                mather->second_str = memWideCharcpy(mather->second_str, 1, true, true, p);
                mather->status = LEXMATHER_ING_3;
            }
            else
                mather->status = LEXMATHER_END_1;
        }
        else
            mather->status = LEXMATHER_MISTAKE;
    else if (mather->status == LEXMATHER_ING_3)
        if (iswalnum(p) || L'_' == p)
            mather->second_str = memWideCharcpy(mather->second_str, 1, true, true, p);
        else
            mather->status = LEXMATHER_END_1;
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配一个变量
 * 匹配模式：匹配器结束模式为END_SECOND模式，也就是当checkoutMather检查的时候，END_SECOND位于END的优先级之后。
 * END_SECOND解决了冲突：关键词if可以满足varMather的匹配，但他并不是变量，if有特殊的匹配器(strMather)来匹配。
 * 匹配内容：a, a_123
 * @param p
 * @param mather
 */
void varMather(wint_t p, LexMather *mather){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING_1){
        if (iswalpha(p) || L'_' == p || iswdigit(p) && mather->status == LEXMATHER_ING_1){
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING_1;
        }
        else if(mather->status == LEXMATHER_ING_1)
            mather->status = LEXMATHER_END_2;
        else if(mather->status == LEXMATHER_START)
            mather->status = LEXMATHER_MISTAKE;
    }
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配一个字符串字面量
 * 注意：string_type记录的是字符串结束标志(‘或者“)
 * 此处引进LEXMATHER_PASS，是为了在匹配到结束标志"或者'后，多读取一个字符，然后在统一回退
 * 匹配内容：’134‘，”123“
 * @param p
 * @param mather
 */
void stringMather(wint_t p, LexMather *mather, LexFile *file){
    if (mather->status == LEXMATHER_START)
        if (L'\"' == p || L'\'' == p){
            mather->status = LEXMATHER_ING_1;
            mather->string_type = p;
        }
        else
            mather->status = LEXMATHER_MISTAKE;
    else if (mather->status == LEXMATHER_ING_1)
        if (mather->string_type == p)
            mather->status = LEXMATHER_ING_4;
        else if (L'\\' == p)
            mather->status = LEXMATHER_ING_5;
        else if (WEOF == p) {
            file->errsyntax = L"stringMather: don't get quotation marks at the end of string";
            mather->status = LEXMATHER_MISTAKE;
        } else{
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING_1;
        }
    else if (mather->status == LEXMATHER_ING_3)
        if (iswalnum(p) || L'_' == p)
            mather->second_str = memWideCharcpy(mather->second_str, 1, true, true, p);
        else
            mather->status = LEXMATHER_END_1;
    else if(mather->status == LEXMATHER_ING_4)
        if (iswalpha(p) || L'_' == p){
            mather->second_str = memWideCharcpy(mather->second_str, 1, true, true, p);
            mather->status = LEXMATHER_ING_3;
        }
        else
            mather->status = LEXMATHER_END_1;
    else if (mather->status == LEXMATHER_ING_5){
        wint_t new = WEOF;
        if (mather->string_type == '\'') {
            switch (p) {
                case L'n':
                    new = L'\n';
                    break;
                case L't':
                    new = L'\t';
                    break;
                case L'b':
                    new = L'\b';
                    break;
                case L'a':
                    new = L'\a';
                    break;
                case L'r':
                    new = L'\r';
                    break;
                case L'\'':
                    new = L'\'';
                    break;
                case L'"':
                    new = L'"';
                    break;
                case L'\\':
                    new = L'\\';
                    break;
                case L'[':
                    mather->status = LEXMATHER_ING_6;
                    break;
                default :
                case L'0':
                    file->errsyntax = L"stringMather: don't support \\0";
                    mather->status = LEXMATHER_MISTAKE;
                    break;
            }
            if (new != WEOF) {
                mather->str = memWideCharcpy(mather->str, 1, true, true, new);
                mather->status = LEXMATHER_ING_1;
                mather->len++;
            }
        } else {  // 双引号为无转义字符串 (只转义双引号)
            if (p != '"') {
                mather->str = memWideCharcpy(mather->str, 1, true, true, '\\');
                mather->len++;
            }
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len++;
            mather->status = LEXMATHER_ING_1;
        }
    }
    else if (mather->status == LEXMATHER_ING_6)
        if (p == L']')
            if (mather->ascii <= 0) {
                mather->status = LEXMATHER_MISTAKE;
                file->errsyntax = L"stringMather: error ascii <= 0";
            } else {
                mather->str = memWideCharcpy(mather->str, 1, true, true, (char)mather->ascii);
                mather->status = LEXMATHER_ING_1;
                mather->len ++;
            }
        else if (iswdigit(p)){  // 手动输入ascii码
            wchar_t num_[2] = {(wchar_t)p, 0};
            int num = (int)wcstol(num_, NULL, 10);  // ascii不大于127, 使用int即可
            mather->ascii = (mather->ascii * 10) + num;
            if (mather->ascii > 127) {
                file->errsyntax = L"stringMather: error ascii > 127";
                mather->status = LEXMATHER_MISTAKE;
            }
        } else {
            mather->status = LEXMATHER_MISTAKE;
            file->errsyntax = L"stringMather: no support from ascii";
        }
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配关键词dest_p
 * @param p
 * @param mather
 * @param dest_p
 */
void strMather(wint_t p, LexMather *mather, const wchar_t *dest_p){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING_1)
        if (p == dest_p[mather->len]){
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING_1;
        }
        else if(mather->status == LEXMATHER_ING_1 && mather->len == memWidelen(dest_p))
            mather->status = LEXMATHER_END_1;
        else
            mather->status = LEXMATHER_MISTAKE;
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配但个字符dest_p
 * @param p
 * @param mather
 * @param dest_p
 */
void charMather(wint_t p, LexMather *mather, wint_t dest_p){
    if (p == dest_p && mather->status == LEXMATHER_START){
        mather->str = memWideCharcpy(mather->str, 1, true, true, p);
        mather->len ++;
        mather->status = LEXMATHER_ING_1;
    }
    else if (mather->status == LEXMATHER_ING_1)
        mather->status = LEXMATHER_END_1;
    else
        mather->status = LEXMATHER_MISTAKE;
}

void aCharMather(wint_t p, LexMather *mather, wint_t dest_p) {
    if (p == dest_p && mather->status == LEXMATHER_START){
        mather->str = memWideCharcpy(mather->str, 1, true, true, p);
        mather->len ++;
        mather->status = LEXMATHER_END_1;
    }
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配空白符号
 * @param p
 * @param mather
 */
void spaceMather(wint_t p, LexMather *mather){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING_1)
        if (iswspace(p) && L'\n' != p){
            mather->str = memWideCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING_1;
        }
        else if (mather->status == LEXMATHER_ING_1)
            mather->status = LEXMATHER_END_1;
        else
            mather->status = LEXMATHER_MISTAKE;
    else
        mather->status = LEXMATHER_MISTAKE;
}

void backslashMather(wint_t p, LexMather *mather){
    if (mather->status == LEXMATHER_START)
        if (p == L'\\')
            mather->status = LEXMATHER_ING_1;
        else
            mather->status = LEXMATHER_MISTAKE;
    else if (mather->status == LEXMATHER_ING_1) {
        if (p == WEOF)
            mather->status = LEXMATHER_END_1;
        else if (p == L'\n')
            mather->status = LEXMATHER_ING_2;
    }
    else if (mather->status == LEXMATHER_ING_2)
        mather->status = LEXMATHER_END_1;
    else
        mather->status = LEXMATHER_MISTAKE;
}

void commentMather(wint_t p, LexMather *mather){
    if (mather->status == LEXMATHER_START) {
        if (p == L'#')
            mather->status = LEXMATHER_ING_1;
        else
            mather->status = LEXMATHER_MISTAKE;
    }
    else if (mather->status == LEXMATHER_ING_1) {  // 匹配到1个#的模式
        if (p == L'#')
            mather->status = LEXMATHER_ING_3;
        else if (p == L'\n' || p == WEOF)
            mather->status = LEXMATHER_END_1;
        else
            mather->status = LEXMATHER_ING_2;
    }
    else if (mather->status == LEXMATHER_ING_2){  // 单#匹配模式
        if (p == L'\n' || p == WEOF)
            mather->status = LEXMATHER_END_1;
    }
    else if (mather->status == LEXMATHER_ING_3) { // 双#匹配模式
        if (p == L'#')
            mather->status = LEXMATHER_ING_4;
        else if (p == WEOF)
            mather->status = LEXMATHER_END_1;
    }
    else if (mather->status == LEXMATHER_ING_4) {
        if (p == L'#')
            mather->status = LEXMATHER_ING_5;
        else
            mather->status = LEXMATHER_ING_3;
    }
    else if (mather->status == LEXMATHER_ING_5)
        mather->status = LEXMATHER_END_1;
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 开始匹配，返回的int即checkoutMather返回的值(匹配成功的匹配器的索引)
 * @param file
 * @param mathers
 * @return
 */
int getMatherStatus(LexFile *file, LexMathers *mathers) {
    int status = -1;
    wint_t p;
    setupMathers(mathers);
    while (status == -1){
        p = readChar(file);
        if (pm_KeyInterrupt == signal_appear) {
            pm_KeyInterrupt = signal_check;
            file->errsyntax = L"singal KeyInterrupt";
            return -3;
        }

        file->errsyntax = NULL;
        numberMather(p ,mathers->mathers[MATHER_NUMBER]);
        varMather(p ,mathers->mathers[MATHER_VAR]);
        spaceMather(p ,mathers->mathers[MATHER_SPACE]);
        stringMather(p, mathers->mathers[MATHER_STRING], file);
        backslashMather(p, mathers->mathers[MATHER_NOTENTER]);
        commentMather(p, mathers->mathers[MATHER_COMMENT]);
        aCharMather(p, mathers->mathers[MATHER_ENTER], '\n');
        aCharMather(p, mathers->mathers[MATHER_EOF], WEOF);

        strMatherMacro(MATHER_IF, "if");  // 条件判断
        strMatherMacro(MATHER_ELIF, "elif");  // 条件循环
        strMatherMacro(MATHER_WHILE, "while");  // 条件循环
        strMatherMacro(MATHER_FOR, "for");  // 遍历
        strMatherMacro(MATHER_IN, "in");  // 定义类
        strMatherMacro(MATHER_TRY, "try");  // 定义函数
        strMatherMacro(MATHER_EXCEPT, "except");  // 定义表达式(匿名函数)
        strMatherMacro(MATHER_AS, "as");  // 异常捕获
        strMatherMacro(MATHER_WITH, "with");  // 异常捕获
        strMatherMacro(MATHER_DO, "do");  // 捕获
        strMatherMacro(MATHER_ELSE, "else");  // 捕获
        strMatherMacro(MATHER_FINALLY, "finally");  // 条件分支
        strMatherMacro(MATHER_DEFAULT, "default");  // 条件-否则
        strMatherMacro(MATHER_GLOBAL, "global");  // 结束分支
        strMatherMacro(MATHER_NONLOCAL, "nonlocal");  // 结束分支

        strMatherMacro(MATHER_PUBLIC, "public");  // 结束分支
        strMatherMacro(MATHER_PROTECT, "protect");  // break跳出分支(循环、条件等)
        strMatherMacro(MATHER_PRIVATE, "private");

        strMatherMacro(MATHER_TRUE, "true");
        strMatherMacro(MATHER_FALSE, "false");
        strMatherMacro(MATHER_NULL, "null");

        strMatherMacro(MATHER_DEF, "def");
        strMatherMacro(MATHER_CLASS, "class");
        strMatherMacro(MATHER_BLOCK, "block");
        strMatherMacro(MATHER_BREAK, "break");
        strMatherMacro(MATHER_CONTINUE, "continue");
        strMatherMacro(MATHER_REGO, "rego");
        strMatherMacro(MATHER_RESTART, "restart");
        strMatherMacro(MATHER_RETURN, "return");
        strMatherMacro(MATHER_YIELD, "yield");
        strMatherMacro(MATHER_IMPORT, "import");
        strMatherMacro(MATHER_INCLUDE, "include");

        charMatherMacro(MATHER_ADD, '+');
        charMatherMacro(MATHER_SUB, '-');
        charMatherMacro(MATHER_MUL, '*');
        charMatherMacro(MATHER_DIV, '/');
        strMatherMacro(MATHER_INTDIV, "//");
        charMatherMacro(MATHER_PER, '%');
        strMatherMacro(MATHER_POW, "**");

        strMatherMacro(MATHER_EQ, "==");
        strMatherMacro(MATHER_MOREEQ, ">=");
        strMatherMacro(MATHER_LESSEQ, "<=");
        charMatherMacro(MATHER_MORE, '>');
        charMatherMacro(MATHER_LESS, '<');
        strMatherMacro(MATHER_NOTEQ, "!=");

        charMatherMacro(MATHER_BITAND, '&');
        charMatherMacro(MATHER_BITOR, '|');
        charMatherMacro(MATHER_BITXOR, '^');
        charMatherMacro(MATHER_BITNOT, '~');
        strMatherMacro(MATHER_BITLEFT, "<<");
        strMatherMacro(MATHER_BITRIGHT, ">>");

        strMatherMacro(MATHER_BOOLAND, "&&");
        strMatherMacro(MATHER_BOOLOR, "||");
        charMatherMacro(MATHER_BOOLNOT, '!');

        charMatherMacro(MATHER_ASSIGNMENT, '=');
        charMatherMacro(MATHER_POINT, '.');
        charMatherMacro(MATHER_AT, '@');
        charMatherMacro(MATHER_SVAR, '$');

        charMatherMacro(MATHER_LP, '(');
        charMatherMacro(MATHER_RP, ')');
        charMatherMacro(MATHER_LB, '[');
        charMatherMacro(MATHER_RB, ']');
        charMatherMacro(MATHER_LC, '{');
        charMatherMacro(MATHER_RC, '}');

        charMatherMacro(MATHER_COMMA, ',');
        charMatherMacro(MATHER_COLON, ':');
        charMatherMacro(MATHER_SEMICOLON, ';');

        strMatherMacro(MATHER_LINK, "->");
        strMatherMacro(MATHER_RAISE, "raise");
        strMatherMacro(MATHER_FROM, "from");
        strMatherMacro(MATHER_ASSERT, "assert_");
        strMatherMacro(MATHER_LAMBDA, "lambda");
        strMatherMacro(MATHER_GOTO, "goto");
        strMatherMacro(MATHER_LABEL, "label");
        strMatherMacro(MATHER_PASSVALUE, "...");
        strMatherMacro(MATHER_DEL, "del");

        status = checkoutMather(mathers, MATHER_MAX);
    }
    if (status != MATHER_ENTER)
        backChar(file);
    return status;
}

int lexFilter(LexFile *file, int status){
    if (status == MATHER_SPACE ||
        status == MATHER_NOTENTER ||
        status == MATHER_COMMENT ||
        file->filter_data.enter != 0 && status == MATHER_ENTER)
        return -1;
    return status;
}

/**
 * getMatherStatus的高级封装，若匹配到空格则自动忽略(再次匹配)
 * @param file
 * @param mathers
 * @return
 */
Token *getToken(LexFile *file, LexMathers *mathers) {
    int status = MATHER_SPACE;
    int filter;

    do
        status = getMatherStatus(file, mathers);
    while ((filter = lexFilter(file, status)) == -1);

    if (status == -2 || status == -3)
        return makeLexToken(status, file->errsyntax, NULL, file->line);
    return makeLexToken(filter, mathers->mathers[status]->str, mathers->mathers[status]->second_str, file->line);
}