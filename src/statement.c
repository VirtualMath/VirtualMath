#include "__virtualmath.h"

Statement *makeStatement(){
    Statement *tmp = memCalloc(1, sizeof(Statement));
    tmp->type = start;
    tmp->next = NULL;
    return tmp;
}

Token *setOperationFromToken(Statement **st_ad, struct Token *left, struct Token *right, int type, bool is_right) {
    Token *new_token = NULL;
    Statement *st = *st_ad, *left_st = left->data.st;
    if (is_right && left->data.st->type == operation &&
        left_st->u.operation.OperationType == st->u.operation.OperationType){

        st->u.operation.left = left_st->u.operation.right;
        left_st->u.operation.right = st;
        st->u.operation.right = right->data.st;
        st = left_st;  // left_st是主中心
    }
    else{
        st->u.operation.left = left_st;
        st->u.operation.right = right->data.st;
    }
    new_token = makeToken();
    new_token->token_type = type;
    new_token->data.st = st;

    freeToken(left, true, false);
    freeToken(right, true, false);
    *st_ad = st;
    return new_token;
}

Statement *makeBaseValueStatement(LinkValue *value){
    Statement *tmp = makeStatement();
    tmp->type = base_value;
    tmp->u.base_value.value = value;
    return tmp;
}

Statement *makeBaseVarStatement(char *name, Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = base_var;
    tmp->u.base_var.name = memStrcpy(name, 0, false, false);
    tmp->u.base_var.times = times;
    return tmp;
}

Statement *makeBaseSVarStatement(Statement *name, Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = base_svar;
    tmp->u.base_svar.name = name;
    tmp->u.base_svar.times = times;
    return tmp;
}

Statement *makeBaseDictStatement(Parameter *pt){
    Statement *tmp = makeStatement();
    tmp->type = base_dict;
    tmp->u.base_dict.dict = pt;
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

Statement *makeTupleStatement(Parameter *pt, enum ListType type) {
    Statement *tmp = makeStatement();
    tmp->type = base_list;
    tmp->u.base_list.type = type;
    tmp->u.base_list.list = pt;
    return tmp;
}

Statement *makeFunctionStatement(Statement *name, Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement();
    tmp->type = set_function;
    tmp->u.set_function.name = name;
    tmp->u.set_function.function = function;
    tmp->u.set_function.parameter = pt;
    return tmp;
}

Statement *makeCallStatement(Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement();
    tmp->type = call_function;
    tmp->u.call_function.function = function;
    tmp->u.call_function.parameter = pt;
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

Statement *makeTryStatement(){
    Statement *tmp = makeStatement();
    tmp->type = try_branch;
    tmp->u.try_branch.except_list = NULL;
    tmp->u.try_branch.else_list = NULL;
    tmp->u.try_branch.finally = NULL;
    tmp->u.try_branch.try = NULL;
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

Statement *makeRegoStatement(Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = rego_if;
    tmp->u.rego_if.times = times;
    return tmp;
}

Statement *makeRestartStatement(Statement *times){
    Statement *tmp = makeStatement();
    tmp->type = restart;
    tmp->u.restart.times = times;
    return tmp;
}

Statement *makeReturnStatement(Statement *value){
    Statement *tmp = makeStatement();
    tmp->type = return_code;
    tmp->u.return_code.value = value;
    return tmp;
}

Statement *makeRaiseStatement(Statement *value){
    Statement *tmp = makeStatement();
    tmp->type = raise_code;
    tmp->u.raise_code.value = value;
    return tmp;
}

Statement *makeIncludeStatement(Statement *file){
    Statement *tmp = makeStatement();
    tmp->type = include_file;
    tmp->u.include_file.file = file;
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
            case operation:
                freeStatement(st->u.operation.right);
                freeStatement(st->u.operation.left);
                break;
            case base_var:
                memFree(st->u.base_var.name);
                freeStatement(st->u.base_var.times);
                break;
            case base_svar:
                freeStatement(st->u.base_svar.name);
                freeStatement(st->u.base_svar.times);
                break;
            case set_function:
                freeStatement(st->u.set_function.name);
                freeStatement(st->u.set_function.function);
                freeParameter(st->u.set_function.parameter, true);
                break;
            case call_function:
                freeStatement(st->u.call_function.function);
                freeParameter(st->u.call_function.parameter, true);
                break;
            case base_list:
                freeParameter(st->u.base_list.list, true);
                break;
            case base_dict:
                freeParameter(st->u.base_dict.dict, true);
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
            case rego_if:
                freeStatement(st->u.rego_if.times);
                break;
            case restart:
                freeStatement(st->u.restart.times);
                break;
            case return_code:
                freeStatement(st->u.return_code.value);
                break;
            case raise_code:
                freeStatement(st->u.raise_code.value);
                break;
            case include_file:
                freeStatement(st->u.include_file.file);
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

Statement *copyStatement(Statement *st){
    if (st == NULL)
        return NULL;

    Statement *tmp = copyStatementCore(st);
    Statement *base_tmp = tmp;

    while (st->next != NULL){
        tmp->next = copyStatementCore(st->next);
        tmp = tmp->next;
        st = st->next;
    }
    return base_tmp;
}

Statement *copyStatementCore(Statement *st){
    Statement *new = makeStatement();
    new->type = st->type;
    new->next = NULL;
    switch (st->type) {
        case base_value:
            new->u.base_value.value = st->u.base_value.value;
            break;
        case operation:
            new->u.operation.OperationType = st->u.operation.OperationType;
            new->u.operation.right = copyStatement(st->u.operation.right);
            new->u.operation.left = copyStatement(st->u.operation.left);
            break;
        case base_var:
            new->u.base_var.name = memStrcpy(st->u.base_var.name, 0, false, false);
            new->u.base_var.times = copyStatement(st->u.base_var.times);
            break;
        case base_svar:
            new->u.base_svar.name = copyStatement(st->u.base_svar.name);
            new->u.base_svar.times = copyStatement(st->u.base_svar.times);
            break;
        case set_function:
            new->u.set_function.name = copyStatement(st->u.set_function.name);
            new->u.set_function.function = copyStatement(st->u.set_function.function);
            new->u.set_function.parameter = copyParameter(st->u.set_function.parameter);
            break;
        case call_function:
            new->u.call_function.function = copyStatement(st->u.call_function.function);
            new->u.call_function.parameter = copyParameter(st->u.call_function.parameter);
            break;
        case base_list:
            new->u.base_list.type = st->u.base_list.type;
            new->u.base_list.list = copyParameter(st->u.base_list.list);
            break;
        case base_dict:
            new->u.base_dict.dict = copyParameter(st->u.base_dict.dict);
            break;
        case if_branch:
            new->u.if_branch.if_list = copyStatementList(st->u.if_branch.if_list);
            new->u.if_branch.finally = copyStatement(st->u.if_branch.finally);
            new->u.if_branch.else_list = copyStatement(st->u.if_branch.else_list);
            break;
        case while_branch:
            new->u.while_branch.type = st->u.while_branch.type;
            new->u.while_branch.while_list = copyStatementList(st->u.while_branch.while_list);
            new->u.while_branch.first = copyStatement(st->u.while_branch.first);
            new->u.while_branch.after = copyStatement(st->u.while_branch.after);
            new->u.while_branch.else_list = copyStatement(st->u.while_branch.else_list);
            new->u.while_branch.finally = copyStatement(st->u.while_branch.finally);
            break;
        case for_branch:
            new->u.for_branch.for_list = copyStatementList(st->u.for_branch.for_list);
            new->u.for_branch.var = copyStatement(st->u.for_branch.var);
            new->u.for_branch.iter = copyStatement(st->u.for_branch.iter);
            new->u.for_branch.else_list = copyStatement(st->u.for_branch.else_list);
            new->u.for_branch.finally = copyStatement(st->u.for_branch.finally);
            break;
        case try_branch:
            new->u.try_branch.except_list = copyStatementList(st->u.try_branch.except_list);
            new->u.try_branch.try = copyStatement(st->u.try_branch.try);
            new->u.try_branch.else_list = copyStatement(st->u.try_branch.else_list);
            new->u.try_branch.finally = copyStatement(st->u.try_branch.finally);
            break;
        case with_branch:
            new->u.with_branch.with_list = copyStatementList(st->u.with_branch.with_list);
            new->u.with_branch.else_list = copyStatement(st->u.with_branch.else_list);
            new->u.with_branch.finally = copyStatement(st->u.with_branch.finally);
            break;
        case break_cycle:
            new->u.break_cycle.times = copyStatement(st->u.break_cycle.times);
            break;
        case continue_cycle:
            new->u.continue_cycle.times = copyStatement(st->u.continue_cycle.times);
            break;
        case rego_if:
            new->u.rego_if.times = copyStatement(st->u.rego_if.times);
            break;
        case restart:
            new->u.restart.times = copyStatement(st->u.restart.times);
            break;
        case return_code:
            new->u.return_code.value = copyStatement(st->u.return_code.value);
            break;
        case raise_code:
            new->u.raise_code.value = copyStatement(st->u.raise_code.value);
            break;
        case include_file:
            new->u.include_file.file = copyStatement(st->u.include_file.file);
            break;
        default:
            break;
    }
    return new;
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

StatementList *copyStatementList(StatementList *sl){
    if (sl == NULL)
        return NULL;

    StatementList *tmp = makeStatementList(copyStatement(sl->condition), copyStatement(sl->var),
                                           copyStatement(sl->code), sl->type);
    StatementList *base_tmp = tmp;

    while (sl->next != NULL){
        tmp->next = makeStatementList(copyStatement(sl->condition), copyStatement(sl->var),
                                      copyStatement(sl->code), sl->type);
        tmp = tmp->next;
        sl = sl->next;
    }
    return base_tmp;
}
