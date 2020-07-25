#include "__virtualmath.h"

Statement *makeStatement(){
    Statement *tmp;
    tmp = memCalloc(1, sizeof(Statement));
    tmp->type = start;
    tmp->next = NULL;
    return tmp;
}

void connectStatement(Statement *base, Statement *new){
    while (base->next != NULL){
        base = base->next;
    }
    base->next = new;
}

void freeStatement(Statement *st){
    if (st == NULL){
        return;
    }
    Statement *next_tmp;
    while (st != NULL){
        switch (st->type) {
            case operation:
                freeStatement(st->u.operation.right);
                freeStatement(st->u.operation.left);
                break;
            case base_var:
                memFree(st->u.base_var.name);
                freeStatement(st->u.base_var.times);
                break;
            default:
                break;
        }
        next_tmp = st->next;
        memFree(st);
        st = next_tmp;
    }
}
