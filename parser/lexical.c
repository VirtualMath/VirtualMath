#include "lexical.h"

/**
 * 从文件中读取一个字节，并处理is_back
 * 每次从文件中读取字符时，则会保存字符到back.p中，调用backChar回退一个字符的时候则不需要传入字符了
 * @param file
 * @return 返回一个字符，若为EOF则返回-1
 */
char readChar(lexFile *file){
    if (file->back.is_back){
        file->back.is_back = false;
    }
    else
        file->back.p = (char)fgetc(file->file);
    return file->back.p;
}

/**
 * 设置字符回退
 * @param file
 */
void backChar(lexFile *file){
    file->back.is_back = true;
}

lexFile *makeLexFile(char *dir){
    lexFile *tmp = memCalloc(1, sizeof(lexFile));
    tmp->file = fopen(dir, "r");
    tmp->back.is_back = false;
    tmp->back.p = EOF;
    return tmp;
}

void freeLexFile(lexFile *file, bool self){
    fclose(file->file);
    if (self){
        memFree(file);
    }
}

/**
 * 初始化mather，代码被复用
 * @param mather
 */
void setupMather(lexMather *mather){
    mather->len = 0;
    mather->str = NULL;
    mather->second_str = NULL;
    mather->string_type = '"';
    mather->status = LEXMATHER_START;
}

lexMather *makeMather(){
    lexMather *tmp = memCalloc(1, sizeof(lexMather));
    setupMather(tmp);
    return tmp;
}

void freeMather(lexMather *mather, bool self){
    memFree(mather->str);
    memFree(mather->second_str);
    mather->len = 0;
    if (self){
        memFree(mather);
    }
}

lexMathers *makeMathers(int size){
    lexMathers *tmp = memCalloc(1, sizeof(lexMathers));
    tmp->size = size;
    tmp->mathers = (struct lexMather**)memCalloc(size, sizeof(lexMather*));
    for(int i=0;i < size; i++){
        tmp->mathers[i] = makeMather();
    }
    return tmp;
}

void freeMathers(lexMathers *mathers, bool self){
    for(int i=0;i < mathers->size; i++){
        freeMather(mathers->mathers[i], true);
    }
    memFree(mathers->mathers);
    mathers->size = 0;
    if (self){
        memFree(mathers);
    }
}

/**
 * 初始化mathers，本质是初始化mathers.mathers内所有的mather
 * @param mathers
 */
void setupMathers(lexMathers *mathers){
    for (int i=0;i < mathers->size;i++){
        if(mathers->mathers[i]->str != NULL){
            memFree(mathers->mathers[i]->str);
            memFree(mathers->mathers[i]->second_str);
        }
        setupMather(mathers->mathers[i]);
    }
}

/**
 * 检查mathers中mather的匹配情况。
 * 情况1：只出现一个匹配器处于END状态，其他均处于MISTAKE或者END_SECOND状态，则视为匹配成功，返回END状态的匹配器
 * 情况2：只出现一个匹配器处于END_SECOND状态，其他均处于MISTAKE状态无END状态，则视为匹配成功，返回END_SECOND状态的匹配器
 * 情况3：全部都在MISTAKE，返回-2，匹配失败
 * 其他情况：匹配还未完成，返回-1
 * @param mathers
 * @param max
 * @return
 */
int checkoutMather(lexMathers *mathers, int max) {
    int mistake_count = 0;
    int end_count = 0, end_index = -1;
    int end_second_count = 0, end_second_index = -1;
//    printf("CHECKOUT:\n");
//    for (int i=0;i < mathers->size;i++){
//        printf("mathers->mathers[%d]->status == %d\n", i, mathers->mathers[i]->status);
//    }
    for (int i=0;i < mathers->size;i++){
        if(mathers->mathers[i]->status == LEXMATHER_END){
            end_count ++;
            end_index = i;
        }
        else if(mathers->mathers[i]->status == LEXMATHER_END_SECOND){
            end_second_count ++;
            end_second_index = i;
        }
        else if(mathers->mathers[i]->status == LEXMATHER_MISTAKE){
            mistake_count ++;
        }
        else if(mathers->mathers[i]->status == LEXMATHER_ING || mathers->mathers[i]->status == LEXMATHER_START){
            return -1;
        }
    }
    if (mistake_count == max){
        return -2;
    }
    else if(end_count == 1){
        return end_index;
    }
    else if(end_second_count == 1){
        return end_second_index;
    }
    else{
        return -1;
    }
}
