#include "__virtualmath.h"

void resetGC(GCStatus *gcs){
    gcs->continue_ = false;
    gcs->link = 0;
}

void setGC(GCStatus *gcs){
    resetGC(gcs);
    gcs->tmp_link = 0;
    gcs->statement_link = 0;
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
    if (gcs->statement_link == 0 && gcs->tmp_link == 0 && gcs->link == 0)
        return true;
    return false;
}
