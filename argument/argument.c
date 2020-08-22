#include "__virtualmath.h"

static const struct option long_option[]={
        {"log-err",required_argument,NULL,'o'},
        {"log-out",required_argument,NULL,'e'},
        {NULL,0,NULL,0}
};

static const char *short_option = "o:e:";

/**
 * 参数设置, args是全局结构体, 保存全局的参数设置
 * @param argc
 * @param argv
 * @return
 */
int getArgs(const int argc, char **argv)
{
    args.out_file = NULL;
    args.error_file = false;
    opterr = false;
    int opt;
    while((opt=getopt_long(argc, argv, short_option ,long_option,NULL))!=-1)
    {
        switch(opt)
        {
            case 0:
                break;
            case 'o':
                args.out_file = memStrcpy(optarg);
                break;
            case 'e':
                args.error_file = memStrcpy(optarg);
                break;
            case '?':
                fprintf(stderr, "[Error]: get not success args : -%c\n", (char)optopt);
                return -1;
            default:
                break;
        }
    }
    return 0;
}

/**
 * 释放args的成员而不包括其本身
 */
void freeArgs(void){
    memFree(args.out_file);
    memFree(args.error_file);
}