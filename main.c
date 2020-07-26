#include "__virtualmath.h"

int testMain(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    testMain(argc, argv);
    return 0;
}

int testMain(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Too many or little argc\n");
        goto argc_error_;
    }
    Inter *global_iter = makeInter();
    ParserMessage *pm = makeParserMessage(argv[1]);
    pasersCommandList(pm, global_iter, true, global_iter->statement);
    if (pm->status != success){
        printf("Syntax Error: %s\n", pm->status_message);
        goto return_;
    }
    globalIterStatement(global_iter);

    return_:
    freePasersMessage(pm, true);
    freeInter(global_iter, true);
    return 0;

    argc_error_:
    return 1;
}
