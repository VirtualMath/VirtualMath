#ifndef VIRTUALMATH_LEXICAL_H
#define VIRTUALMATH_LEXICAL_H
#include "__macro.h"
#include "stdio.h"

typedef struct lexFile{
    FILE *file;
    struct back{
        bool is_back;
        char p;
    } back;
} lexFile;

typedef struct lexMather{
    int len;
    int string_type;
    char *str;
    char *second_str;
    enum status{
        LEXMATHER_START=1,
        LEXMATHER_ING,
        LEXMATHER_INGPOINT,
        LEXMATHER_INGSECOND,
        LEXMATHER_PASS,
        LEXMATHER_END,
        LEXMATHER_END_SECOND,
        LEXMATHER_MISTAKE,
    } status;
} lexMather;

typedef struct lexMathers{
    int size;
    struct lexMather **mathers;
} lexMathers;

char readChar(lexFile *file);
void backChar(lexFile *file);

lexFile *makeLexFile(char *dir);
void freeLexFile(lexFile *file, bool self);

void setupMather(lexMather *mather);
lexMather *makeMather();
void freeMather(lexMather *mather, bool self);

lexMathers *makeMathers(int size);
void freeMathers(lexMathers *mathers, bool self);
void setupMathers(lexMathers *mathers);
int checkoutMather(lexMathers *mathers, int max);
#endif //VIRTUALMATH_LEXICAL_H