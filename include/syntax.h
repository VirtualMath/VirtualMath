#ifndef VIRTUALMATH_SYNTAX_H
#define VIRTUALMATH_SYNTAX_H
#include "lexical.h"
#include "token.h"

void numberMather(char p, lexMather *mather);
void varMather(char p, lexMather *mather);
void stringMather(char p, lexMather *mather);
void strMather(char p, lexMather *mather, const char *dest_p);
void charMather(char p, lexMather *mather, char dest_p);

#define strMatherMacro(n, word) strMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/
#define charMatherMacro(n, word) charMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/
#endif //VIRTUALMATH_SYNTAX_H