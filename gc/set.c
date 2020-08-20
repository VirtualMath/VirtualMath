#include "__virtualmath.h"

void resetGC(GCStatus *gcs){
    gcs->continue_ = false;
    gcs->link = 0;
}

void setGC(GCStatus *gcs){
    resetGC(gcs);
    gcs->tmp_link = 0;
    gcs->statement_link = 0;
    gcs->c_value = not_free;
}

void gc_addTmpLink(GCStatus *gcs){
    gcs->tmp_link ++;
}

void gc_addLink(GCStatus *gcs){
    gcs->link ++;
}

void gc_addStatementLink(GCStatus *gcs){
    gcs->statement_link ++;
}

void gc_freeStatementLink(GCStatus *gcs){
    gcs->statement_link --;
}

void gc_freeTmpLink(GCStatus *gcs){
    gcs->tmp_link --;
}

bool gc_IterAlready(GCStatus *gcs){
    bool return_ = gcs->continue_;
    gcs->continue_ = true;
    return return_;
}

bool gc_needFree(GCStatus *gcs){
    return (gcs->statement_link == 0 && gcs->tmp_link == 0 && gcs->link == 0);
}

void gc_resetValue(Value *value){
    value->gc_status.c_value = not_free;
}

bool gc_needFreeValue(Value *value){
    return (gc_needFree(&value->gc_status) && value->gc_status.c_value == need_free);
}
