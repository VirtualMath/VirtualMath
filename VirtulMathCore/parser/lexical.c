#include "__virtualmath.h"

/**
 * 从文件中读取一个字节，并处理is_back
 * 每次从文件中读取字符时，则会保存字符到back.p中，调用backChar回退一个字符的时候则不需要传入字符了
 * @param file
 * @return 返回一个字符，若为EOF则返回-1
 */
int readChar(LexFile *file){
    if (file->back.is_back)
        file->back.is_back = false;
    else
        file->back.p = fgetc(file->file);
    if (file->back.p == '\n')
        file->line++;
    return file->back.p;
}

/**
 * 设置字符回退
 * @param file
 */
void backChar(LexFile *file){
    file->back.is_back = true;
    if (file->back.p == '\n')
        file->line --;
}

LexFile *makeLexFile(char *dir){
    LexFile *tmp = memCalloc(1, sizeof(LexFile));
    tmp->is_std = (bool)(dir == NULL);
    if (tmp->is_std)
        tmp->file = stdin;
    else
        tmp->file = fopen(dir, "r");
    tmp->back.is_back = false;
    tmp->back.p = EOF;
    tmp->line = 1;
    tmp->filter_data.enter = 0;
    return tmp;
}

void freeLexFile(LexFile *file) {
    FREE_BASE(file, return_);
    if (!file->is_std)
        fclose(file->file);
    memFree(file);
    return_:
    return;
}

/**
 * 初始化mather，代码被复用
 * @param mather
 */
void setupMather(LexMather *mather){
    mather->len = 0;
    mather->str = NULL;
    mather->second_str = NULL;
    mather->string_type = '"';
    mather->status = LEXMATHER_START;
}

LexMather *makeMather(){
    LexMather *tmp = memCalloc(1, sizeof(LexMather));
    setupMather(tmp);
    return tmp;
}

void freeMather(LexMather *mather) {
    memFree(mather->str);
    memFree(mather->second_str);
    mather->len = 0;
    memFree(mather);
}

LexMathers *makeMathers(int size){
    LexMathers *tmp = memCalloc(1, sizeof(LexMathers));
    tmp->size = size;
    tmp->mathers = (struct LexMather**)memCalloc(size, sizeof(LexMather*));
    for(int i=0;i < size; i++)
        tmp->mathers[i] = makeMather();
    return tmp;
}

void freeMathers(LexMathers *mathers) {
    FREE_BASE(mathers, return_);
    for(int i=0;i < mathers->size; i++)
        freeMather(mathers->mathers[i]);
    memFree(mathers->mathers);
    mathers->size = 0;
    memFree(mathers);
    return_:
    return;
}

/**
 * 初始化mathers，本质是初始化mathers.mathers内所有的mather
 * @param mathers
 */
void setupMathers(LexMathers *mathers){
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
int checkoutMather(LexMathers *mathers, int max) {
    bool return_1 = false;
    int mistake_count = 0;
    int end_count = 0, end_index = -1;
    int end_second_count = 0, end_second_index = -1;
    for (int i=0;i < mathers->size;i++){
        if(mathers->mathers[i]->status == LEXMATHER_END_1){
            end_count ++;
            end_index = i;
        }
        else if(mathers->mathers[i]->status == LEXMATHER_END_2){
            end_second_count ++;
            end_second_index = i;
        }
        else if(mathers->mathers[i]->status == LEXMATHER_MISTAKE){
            mistake_count ++;
        }
        else{
            return_1 = true;
        }
    }
    if (return_1)
        goto return_;

    if (mistake_count == max)
        return -2;
    else if(end_count == 1)
        return end_index;
    else if(end_second_count == 1)
        return end_second_index;

    return_:
    return -1;
}
