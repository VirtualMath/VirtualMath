#ifndef VIRTUALMATH_SYNTAX_H
#define VIRTUALMATH_SYNTAX_H

#define strMatherMacro(n, word) strMather(p, mathers->mathers[n], L##word) /*这个宏只能用于getMatherStatus*/
#define charMatherMacro(n, word) charMather(p, mathers->mathers[n], word) /*这个宏只能用于getMatherStatus*/

void numberMather(wint_t p, LexMather *mather);
void varMather(wint_t p, LexMather *mather);
void stringMather(wint_t p, LexMather *mather);
void strMather(wint_t p, LexMather *mather, const wchar_t *dest_p);
void charMather(wint_t p, LexMather *mather, wint_t dest_p);
void aCharMather(wint_t p, LexMather *mather, wint_t dest_p);
void spaceMather(wint_t p, LexMather *mather);
void commentMather(wint_t p, LexMather *mather);
void backslashMather(wint_t p, LexMather *mather);
#endif //VIRTUALMATH_SYNTAX_H