#ifndef VIRTUALMATH_LEXICAL_H
#define VIRTUALMATH_LEXICAL_H
#include "__macro.h"
#include "stdio.h"

typedef struct LexFile{
    FILE *file;
    struct {
        bool is_back;
        char p;
    } back;
} LexFile;

typedef struct LexMather{
    int len;
    int string_type;
    char *str;
    char *second_str;
    enum {
        LEXMATHER_START=1,
        LEXMATHER_ING,
        LEXMATHER_INGPOINT,
        LEXMATHER_INGSECOND,
        LEXMATHER_PASS,
        LEXMATHER_END,
        LEXMATHER_END_SECOND,
        LEXMATHER_MISTAKE,
    } status;
} LexMather;

typedef struct LexMathers{
    int size;
    struct LexMather **mathers;
} LexMathers;

char readChar(LexFile *file);
void backChar(LexFile *file);

LexFile *makeLexFile(char *dir);
void freeLexFile(LexFile *file, bool self);

void setupMather(LexMather *mather);
LexMather *makeMather();
void freeMather(LexMather *mather, bool self);

LexMathers *makeMathers(int size);
void freeMathers(LexMathers *mathers, bool self);
void setupMathers(LexMathers *mathers);
int checkoutMather(LexMathers *mathers, int max);
#endif //VIRTUALMATH_LEXICAL_H