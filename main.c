#include "__virtualmath.h"

static const struct option long_option[]={
        {"input",required_argument,NULL,'i'},
        {"stdout",no_argument,NULL,'s'},

#if OUT_LOG
        {"log",required_argument,NULL,'l'},
        {"debug",no_argument,&args.level,DEBUG},
        {"ddebug",no_argument,&args.level,DEEP_DEBUG},
        {"ldebug",no_argument,&args.level,LEXICAL_DEBUG},
        {"lcdebug",no_argument,&args.level,LEXICAL_CHECKOUT_DEBUG},
        {"info",no_argument,&args.level,INFO},
#endif
        // 最后一个元素应该全为0
        {NULL,0,NULL,0}
};

#if OUT_LOG
static const char *short_option = "si:l:";
#else
static const char *short_option = "si:";
#endif
int getArgs(int argc, char *argv[]);
void freeArgs();

int main(int argc, char *argv[]) {
    printf("[start VirtualMath]\n");
    if (getArgs(argc, argv)){
        goto args_error_;
    }

    Inter *global_iter = makeInter(args.log_file);
    ParserMessage *pm = makeParserMessage(args.file, args.log_file);

    printf("[start to lexical]\n");
    parserCommandList(pm, global_iter, true, global_iter->statement);
    if (pm->status != success){
        writeLog(pm->paser_debug, ERROR, "Syntax Error: %s\n", pm->status_message);
        writeLog(stderr, ERROR, "Syntax Error: %s\n", pm->status_message);
        goto return_;
    }

    printf("[start to run]\n");
    Result tmp;
    tmp = globalIterStatement(global_iter);
    if (tmp.type == error_return){
        writeLog(global_iter->debug, ERROR, "Run Error\n", NULL);
        writeLog(stderr, ERROR, "Run Error\n", NULL);
    }

    return_:
    freeParserMessage(pm, true);
    freeInter(global_iter, true);
    freeArgs();
    printf("[exit Virtual [0]]\n");
    return 0;

    args_error_:
    freeArgs();
    printf("[exit Virtual [1]]\n");
    return 1;
}

/**
 * 参数设置, args是全局结构体, 保存全局的参数设置
 * @param argc
 * @param argv
 * @return
 */
int getArgs(int argc, char *argv[])
{
    args.file = NULL;
    args.log_file = NULL;
    args.level = LEXICAL_CHECKOUT_DEBUG;
    args.stdout_inter = false;
    opterr = false;
    int opt;
    while((opt=getopt_long(argc,argv,short_option ,long_option,NULL))!=-1)
    {
        switch(opt)
        {
            case 0:
                break;
            case 'i':
                args.file = memStrcpy(optarg, 0, false, false);
                break;
            case 'l':
                args.log_file = memStrcpy(optarg, 0, false, false);
                break;
            case 's':
                args.stdout_inter = true;
                break;
            case '?':
                printf("[Error]: get not success args : -%c\n", (char)optopt);
                return -1;
            default:
                break;
        }
    }
    if (args.file == NULL){
        if (argc > optind){
            args.file = memStrcpy(argv[optind], 0, false, false);
        }
        else{
            return -1;
        }
    }
    return 0;
}

/**
 * 释放args的成员而不包括其本身
 */
void freeArgs(){
    memFree(args.file);
    memFree(args.file);
}

/**
 *  TODO-szh 函数形式参数复制
 *  TODO-szh 运行错误的内存释放检查
 *  TODO-szh syntax错误的内存释放检查
 */