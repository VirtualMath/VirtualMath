#include "hellovm.h"

char *HelloString = "Welcome To VirtualMath ("__TIME__", "__DATE__") \n"
#ifdef __linux__
                    "On Linux\n"
#else
                    "On windows"
#endif
                    "VirtualMath Command Line Mode\n";

static const struct option long_option[]={
        {"stderr",required_argument,NULL,'o'},
        {"stdout",required_argument,NULL,'e'},
        {"stdin",required_argument,NULL,'i'},
        {"not-run-cl",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
};

static const char *short_option = "o:e:i:n";

Args args = {.out_file=NULL, .error_file=NULL, .in_file=NULL, .run_commandLine=true};

/**
 * 参数设置, args是全局结构体, 保存全局的参数设置
 * @param argc
 * @param argv
 * @return
 */
int getArgs(const int argc, char **argv)
{
    int opt;
    opterr = true;
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
            case 'i':
                args.in_file = memStrcpy(optarg);
                break;
            case 'n':
                args.run_commandLine = false;
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
    memFree(args.in_file);
}