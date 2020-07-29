#include "__virtualmath.h"

Statement *makeStatement(){
    Statement *tmp = memCalloc(1, sizeof(Statement));
    tmp->type = start;
    tmp->next = NULL;
    return tmp;
}

Statement *makeOperationStatement(int type){
    Statement *tmp = makeStatement();
    tmp->type = operation;
    tmp->u.operation.OperationType = type;
    tmp->u.operation.left = NULL;
    tmp->u.operation.right = NULL;
    return tmp;
}

Token *setOperationFromToken(Statement *st, Token *left, Token *right, int type) {
    Token *new_token = NULL;
    st->u.operation.left = left->data.st;
    st->u.operation.right = right->data.st;
    new_token = makeToken();
    new_token->token_type = type;
    new_token->data.st = st;

    freeToken(left, true, false);
    freeToken(right, true, false);
    return new_token;
}

Statement *makeFunctionStatement(Statement *name, Statement *function){
    Statement *tmp = makeStatement();
    tmp->type = set_function;
    tmp->u.set_function.name = name;
    tmp->u.set_function.function = function;
    return tmp;
}

Statement *makeCallStatement(Statement *function){
    Statement *tmp = makeStatement();
    tmp->type = call_function;
    tmp->u.call_function.function = function;
    return tmp;
}

Statement *makeIfStatement(){
    Statement *tmp = makeStatement();
    tmp->type = if_branch;
    tmp->u.if_branch.if_list = NULL;
    tmp->u.if_branch.else_list = NULL;
    tmp->u.if_branch.finally = NULL;
    return tmp;
}

Statement *makeWhileStatement(){
    Statement *tmp = makeStatement();
    tmp->type = while_branch;
    tmp->u.while_branch.type = while_;
    tmp->u.while_branch.while_list = NULL;
    tmp->u.while_branch.else_list = NULL;
    tmp->u.while_branch.finally = NULL;
    tmp->u.while_branch.first = NULL;
    tmp->u.while_branch.after = NULL;
    return tmp;
}

Statement *makeBreakStatement(Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = break_cycle;
    tmp->u.break_cycle.times = times;
    return tmp;
}

Statement *makeContinueStatement(Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = continue_cycle;
    tmp->u.continue_cycle.times = times;
    return tmp;
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
            // base-value 不需要释放
            case operation:
                freeStatement(st->u.operation.right);
                freeStatement(st->u.operation.left);
                break;
            case base_var:
                memFree(st->u.base_var.name);
                freeStatement(st->u.base_var.times);
                break;
            case set_function:
                freeStatement(st->u.set_function.name);
                freeStatement(st->u.set_function.function);
                break;
            case call_function:
                freeStatement(st->u.call_function.function);
                break;
            case if_branch:
                freeStatementList(st->u.if_branch.if_list);
                freeStatement(st->u.if_branch.finally);
                freeStatement(st->u.if_branch.else_list);
                break;
            case while_branch:
                freeStatementList(st->u.while_branch.while_list);
                freeStatement(st->u.while_branch.first);
                freeStatement(st->u.while_branch.after);
                freeStatement(st->u.while_branch.else_list);
                freeStatement(st->u.while_branch.finally);
                break;
            case for_branch:
                freeStatementList(st->u.for_branch.for_list);
                freeStatement(st->u.for_branch.var);
                freeStatement(st->u.for_branch.iter);
                freeStatement(st->u.for_branch.else_list);
                freeStatement(st->u.for_branch.finally);
                break;
            case try_branch:
                freeStatementList(st->u.try_branch.except_list);
                freeStatement(st->u.try_branch.try);
                freeStatement(st->u.try_branch.else_list);
                freeStatement(st->u.try_branch.finally);
                break;
            case with_branch:
                freeStatementList(st->u.with_branch.with_list);
                freeStatement(st->u.with_branch.else_list);
                freeStatement(st->u.with_branch.finally);
                break;
            case break_cycle:
                freeStatement(st->u.break_cycle.times);
                break;
            case continue_cycle:
                freeStatement(st->u.continue_cycle.times);
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

StatementList *makeStatementList(Statement *condition, Statement *var, Statement *code, int type) {
    StatementList *tmp = memCalloc(1, sizeof(StatementList));
    tmp->condition = condition;
    tmp->var = var;
    tmp->code = code;
    tmp->type = type;
    tmp->next = NULL;
    return tmp;
}

StatementList *connectStatementList(StatementList *base, StatementList *new){
    StatementList *tmp = base;
    if (base == NULL)
        return new;
    while (tmp->next != NULL){
        tmp = tmp->next;
    }
    tmp->next = new;
    return base;
}

void freeStatementList(StatementList *base){
    while (base != NULL){
        freeStatement(base->condition);
        freeStatement(base->code);
        freeStatement(base->var);
        StatementList *tmp = base;
        base = base->next;
        memFree(tmp);
    }
}
