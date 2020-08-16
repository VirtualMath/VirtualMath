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
void numberMather(int p, LexMather *mather){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING || mather->status == LEXMATHER_INGPOINT)
        if (isdigit(p) || '.' == p && mather->status == LEXMATHER_ING){
            mather->str = memStrCharcpy(mather->str, 1, true, true, p);
            mather->len += 1;
            if ('.' == p)
                mather->status = LEXMATHER_INGPOINT;
            else if (mather->status == LEXMATHER_START)
                mather->status = LEXMATHER_ING;
        }
        else if(mather->status == LEXMATHER_ING || mather->status == LEXMATHER_INGPOINT){
            if (isalpha(p) ||'_' == p){
                mather->second_str = memStrCharcpy(mather->second_str, 1, true, true, p);
                mather->status = LEXMATHER_INGSECOND;
            }
            else
                mather->status = LEXMATHER_END;
        }
        else
            mather->status = LEXMATHER_MISTAKE;
    else if (mather->status == LEXMATHER_INGSECOND)
        if (isalnum(p) ||'_' == p)
            mather->second_str = memStrCharcpy(mather->second_str, 1, true, true, p);
        else
            mather->status = LEXMATHER_END;
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
void varMather(int p, LexMather *mather){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING){
        if (isalpha(p) ||'_' == p || isdigit(p) && mather->status == LEXMATHER_ING){
            mather->str = memStrCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING;
        }
        else if(mather->status == LEXMATHER_ING)
            mather->status = LEXMATHER_END_SECOND;
        else if(mather->status == LEXMATHER_START)
            mather->status = LEXMATHER_MISTAKE;
    }
    else{
        mather->status = LEXMATHER_MISTAKE;
    }
}

/**
 * 匹配一个字符串字面量
 * 注意：string_type记录的是字符串结束标志(‘或者“)
 * 此处引进LEXMATHER_PASS，是为了在匹配到结束标志"或者'后，多读取一个字符，然后在统一回退
 * 匹配内容：’134‘，”123“
 * @param p
 * @param mather
 */
void stringMather(int p, LexMather *mather){
    if (mather->status == LEXMATHER_START)
        if ('\"' == p || '\'' == p){
            mather->status = LEXMATHER_ING;
            mather->string_type = p;
        }
        else
            mather->status = LEXMATHER_MISTAKE;
    else if (mather->status == LEXMATHER_ING)
        if (mather->string_type == p)
            mather->status = LEXMATHER_INGPASS;
        else if (EOF == p)
            mather->status = LEXMATHER_MISTAKE;
        else{
            mather->str = memStrCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING;
        }
    else if (mather->status == LEXMATHER_INGSECOND)
        if (isalnum(p) ||'_' == p)
            mather->second_str = memStrCharcpy(mather->second_str, 1, true, true, p);
        else
            mather->status = LEXMATHER_END;
    else if(mather->status == LEXMATHER_INGPASS)
        if (isalpha(p) ||'_' == p){
            mather->second_str = memStrCharcpy(mather->second_str, 1, true, true, p);
            mather->status = LEXMATHER_INGSECOND;
        }
        else
            mather->status = LEXMATHER_END;
    else
        mather->status = LEXMATHER_MISTAKE;
}

/**
 * 匹配关键词dest_p
 * @param p
 * @param mather
 * @param dest_p
 */
void strMather(int p, LexMather *mather, const char *dest_p){
    if (mather->status == LEXMATHER_START || mather->status == LEXMATHER_ING)
        if (p == dest_p[mather->len]){
            mather->str = memStrCharcpy(mather->str, 1, true, true, p);
            mather->len ++;
            mather->status = LEXMATHER_ING;
        }
        else if(mather->status == LEXMATHER_ING && mather->len == memStrlen((char *)dest_p))
            mather->status = LEXMATHER_END;
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
void charMather(int p, LexMather *mather, int dest_p){
    if (p == dest_p && mather->status == LEXMATHER_START){
        mather->str = memStrCharcpy(mather->str, 1, true, true, p);
        mather->len ++;
        mather->status = LEXMATHER_ING;
    }
    else if (mather->status == LEXMATHER_ING)
        mather->status = LEXMATHER_END;
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
    setupMathers(mathers);
    int status = -1;
    while (status == -1){
        int p = readChar(file);
        numberMather(p ,mathers->mathers[MATHER_NUMBER]);
        stringMather(p ,mathers->mathers[MATHER_STRING]);
        varMather(p ,mathers->mathers[MATHER_VAR]);
        charMatherMacro(MATHER_EOF, EOF);
        charMatherMacro(MATHER_ENTER, '\n');
        charMatherMacro(MATHER_SPACE, ' ');

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
        strMatherMacro(MATHER_ASSERT, "assert");
        strMatherMacro(MATHER_LAMBDA, "lambda");

        status = checkoutMather(mathers, MATHER_MAX);
    }
    backChar(file);
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
    Token *tmp;
    while (status == MATHER_SPACE)
        status = getMatherStatus(file, mathers);
    if (status == -2){
        tmp = makeLexToken(MATHER_ERROR_, NULL, NULL, file->line);
        goto return_;
    }
    tmp = makeLexToken(status, mathers->mathers[status]->str, mathers->mathers[status]->second_str, file->line);
    return_:
    return tmp;
}