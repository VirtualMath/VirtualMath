#ifndef VIRTUALMATH_LEXICAL_H
#define VIRTUALMATH_LEXICAL_H
#include "__macro.h"

struct LexFile{
    FILE *file;
    struct LexFileBack{
        bool is_back;
        signed char p;
    } back;
    int count;
};

struct LexMather{
    int len;
    signed char string_type;
    char *str;
    char *second_str;
    enum LexMatherStatus{
        LEXMATHER_START=1,
        LEXMATHER_ING,
        LEXMATHER_INGPOINT,
        LEXMATHER_INGSECOND,
        LEXMATHER_INGPASS,
        LEXMATHER_END,
        LEXMATHER_END_SECOND,
        LEXMATHER_MISTAKE,
    } status;
};

struct LexMathers{
    int size;
    struct LexMather **mathers;
};

typedef struct LexFile LexFile;
typedef struct LexMather LexMather;
typedef struct LexMathers LexMathers;

signed char readChar(LexFile *file);
void backChar(LexFile *file);

LexFile *makeLexFile(char *dir);
void freeLexFile(LexFile *file, bool self);

void setupMather(LexMather *mather);
LexMather *makeMather();
void freeMather(LexMather *mather, bool self);

LexMathers *makeMathers(int size);
void freeMathers(LexMathers *mathers, bool self);
void setupMathers(LexMathers *mathers);
int checkoutMather(LexMathers *mathers, int max, FILE *debug);
#endif //VIRTUALMATH_LEXICAL_H