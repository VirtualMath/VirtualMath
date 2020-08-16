#ifndef VIRTUALMATH_SYNTAX_H
#define VIRTUALMATH_SYNTAX_H

#define strMatherMacro(n, word) strMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/
#define charMatherMacro(n, word) charMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/

void numberMather(int p, LexMather *mather);
void varMather(int p, LexMather *mather);
void stringMather(int p, LexMather *mather);
void strMather(int p, LexMather *mather, const char *dest_p);
void charMather(int p, LexMather *mather, int dest_p);
void spaceMather(int p, LexMather *mather);
void commentMather(int p, LexMather *mather);
void backslashMather(int p, LexMather *mather);

#endif //VIRTUALMATH_SYNTAX_H