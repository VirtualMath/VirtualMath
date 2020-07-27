#include "__virtualmath.h"

Statement *makeStatement(){
    Statement *tmp;
    tmp = memCalloc(1, sizeof(Statement));
    tmp->type = start;
    tmp->next = NULL;
    return tmp;
}

Statement *makeOperationStatement(int type){
    Statement *tmp;
    tmp = memCalloc(1, sizeof(Statement));
    tmp->type = operation;
    tmp->u.operation.OperationType = type;
    tmp->u.operation.left = NULL;
    tmp->u.operation.right = NULL;
    return tmp;
}

Token *setOperationFromToken(Statement *st, Token *left, Token *right, int type) {
    Token *new_token;
    st->u.operation.left = left->data.st;
    st->u.operation.right = right->data.st;
    new_token = makeToken();
    new_token->token_type = type;
    new_token->data.st = st;

    freeToken(left, true, false);
    freeToken(right, true, false);
    return new_token;
}

void connectStatement(Statement *base, Statement *new){
    while (base->next != NULL){
        base = base->next;
    }
    base->next = new;
}

void freeStatement(Statement *st){
    freeBase(st, return_);
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
    return_:
    return;
}
