#ifndef VIRTUALMATH_FILE_H
#define VIRTUALMATH_FILE_H

int checkFileReadable(char *dir);
char *findPath(char **path, char *env, bool need_free);
char *splitDir(char *dir);

#endif //VIRTUALMATH_FILE_H
