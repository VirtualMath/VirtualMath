#ifndef VIRTUALMATH_LEXICAL_H
#define VIRTUALMATH_LEXICAL_H
#include "__macro.h"

struct LexFile{
    FILE *file;
    bool is_std;  // 是否位stdin
    struct LexFileBack{
        bool is_back;
        int p;
    } back;
    struct {
        int enter;  // 若计数为0则不忽略enter
    } filter_data;
    long int line;
};

struct LexMather{
    int len;
    int string_type;
    char *str;
    char *second_str;
    enum LexMatherStatus{
        LEXMATHER_START=1,
        LEXMATHER_ING_1,
        LEXMATHER_ING_2,
        LEXMATHER_ING_3,
        LEXMATHER_ING_4,
        LEXMATHER_ING_5,
        LEXMATHER_END_1,
        LEXMATHER_END_2,
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

int readChar(LexFile *file);
void backChar(LexFile *file);

LexFile *makeLexFile(char *dir);
void freeLexFile(LexFile *file);

void setupMather(LexMather *mather);
LexMather *makeMather();
void freeMather(LexMather *mather);

LexMathers *makeMathers(int size);
void freeMathers(LexMathers *mathers);
void setupMathers(LexMathers *mathers);
int checkoutMather(LexMathers *mathers, int max);
#endif //VIRTUALMATH_LEXICAL_H