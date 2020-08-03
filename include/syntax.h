#ifndef VIRTUALMATH_SYNTAX_H
#define VIRTUALMATH_SYNTAX_H

#define strMatherMacro(n, word) strMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/
#define charMatherMacro(n, word) charMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/

void numberMather(signed char p, LexMather *mather);
void varMather(signed char p, LexMather *mather);
void stringMather(signed char p, LexMather *mather);
void strMather(signed char p, LexMather *mather, const char *dest_p);
void charMather(signed char p, LexMather *mather, signed char dest_p);

#endif //VIRTUALMATH_SYNTAX_H