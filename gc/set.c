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

void gcAddTmp(GCStatus *gcs){
    gcs->tmp_link ++;
}

void gcAddLink(GCStatus *gcs){
    gcs->link ++;
}

void gcAddStatementLink(GCStatus *gcs){
    gcs->statement_link ++;
}

void gcFreeStatementLink(GCStatus *gcs){
    gcs->statement_link --;
}

void gcFreeTmpLink(GCStatus *gcs){
    gcs->tmp_link --;
}

bool setIterAlready(GCStatus *gcs){
    bool return_ = gcs->continue_;
    gcs->continue_ = true;
    return return_;
}

bool needFree(GCStatus *gcs){
    if (gcs->statement_link == 0 && gcs->tmp_link == 0 && gcs->link == 0)
        return true;
    return false;
}
