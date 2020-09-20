#ifndef MD5_H
#define MD5_H
#include "__macro.h"

#define READ_DATA_SIZE	(1024)
int getFileMd5(const char *path, char *md5str);

#endif