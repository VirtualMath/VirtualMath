#include "__virtualmath.h"

Statement *makeStatement(fline line, char *file) {
    Statement *tmp = memCalloc(1, sizeof(Statement));
    tmp->type = start;
    tmp->next = NULL;
    tmp->aut = auto_aut;
    tmp->line = line;
    tmp->code_file = memStrcpy(file);
    setRunInfo(tmp);
    return tmp;
}

void setRunInfo(Statement *st){
    st->info.have_info = false;
    st->info.node = NULL;
    st->info.var_list = NULL;
    st->info.branch.sl_node = NULL;
    st->info.branch.status = info_vl_branch;
    st->info.branch.with_.value = NULL;
    st->info.branch.with_._exit_ = NULL;
    st->info.branch.with_._enter_ = NULL;
    st->info.branch.with_.with_belong = NULL;
    st->info.branch.for_.iter = NULL;
    st->info.branch.func.push = true;
}

void freeRunInfo(Statement *st, bool deal_var) {
    if (deal_var && st->info.var_list != NULL)
        freeVarList(st->info.var_list);
    if (st->info.branch.with_.value != NULL)
        gc_freeTmpLink(&st->info.branch.with_.value->gc_status);
    if (st->info.branch.with_._exit_ != NULL)
        gc_freeTmpLink(&st->info.branch.with_._exit_->gc_status);
    if (st->info.branch.with_._enter_ != NULL)
        gc_freeTmpLink(&st->info.branch.with_._enter_->gc_status);
    if (st->info.branch.with_.with_belong != NULL)
        gc_freeTmpLink(&st->info.branch.with_.with_belong->gc_status);
    if (st->info.branch.for_.iter != NULL)
        gc_freeTmpLink(&st->info.branch.for_.iter->gc_status);
    setRunInfo(st);
}

Token *setOperationFromToken(Statement **st_ad, struct Token *left, struct Token *right, enum OperationType type, bool is_right) {
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
    new_token = makeToken(0);
    new_token->token_type = type;
    new_token->data.st = st;
    st->line = left->line;

    freeToken(left, false);
    freeToken(right, false);
    *st_ad = st;
    return new_token;
}

Statement *makeBaseLinkValueStatement(LinkValue *value, fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_value;
    tmp->u.base_value.type = link_value;
    tmp->u.base_value.value = value;
    tmp->u.base_value.str = NULL;
    gc_addStatementLink(&value->gc_status);
    return tmp;
}

Statement *makeBaseStrValueStatement(wchar_t *value, enum BaseValueType type, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_value;
    tmp->u.base_value.type = type;
    tmp->u.base_value.value = NULL;
    tmp->u.base_value.str = memWidecpy(value);
    return tmp;
}

Statement *makeBaseValueStatement(enum BaseValueType type, fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_value;
    tmp->u.base_value.type = type;
    tmp->u.base_value.value = NULL;
    tmp->u.base_value.str = NULL;
    return tmp;
}

Statement *makeBaseVarStatement(wchar_t *name, Statement *times, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_var;
    tmp->u.base_var.name = memWidecpy(name);
    tmp->u.base_var.times = times;
    tmp->u.base_var.run = true;
    return tmp;
}

Statement *makeBaseSVarStatement(Statement *name, Statement *times, bool is_var) {
    Statement *tmp = makeStatement(name->line, name->code_file);
    tmp->type = base_svar;
    tmp->u.base_svar.name = name;
    tmp->u.base_svar.times = times;
    tmp->u.base_svar.run = true;
    tmp->u.base_svar.is_var = is_var;
    return tmp;
}

Statement *makeBaseDictStatement(Parameter *pt, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_dict;
    tmp->u.base_dict.dict = pt;
    return tmp;
}

Statement *makeOperationBaseStatement(enum OperationType type, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = operation;
    tmp->u.operation.OperationType = type;
    tmp->u.operation.left = NULL;
    tmp->u.operation.right = NULL;
    return tmp;
}

Statement *makeOperationStatement(enum OperationType type, Statement *left, Statement *right){
    Statement *tmp = makeOperationBaseStatement(type, left->line, left->code_file);
    tmp->u.operation.left = left;
    tmp->u.operation.right = right;
    return tmp;
}

Statement *makeTupleStatement(Parameter *pt, enum ListType type, fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = base_list;
    tmp->u.base_list.type = type;
    tmp->u.base_list.list = pt;
    return tmp;
}

Statement *makeClassStatement(Statement *name, Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement(name->line, name->code_file);
    tmp->type = set_class;
    tmp->u.set_class.name = name;
    tmp->u.set_class.st = function;
    tmp->u.set_class.father = pt;
    tmp->u.set_class.decoration = NULL;
    return tmp;
}

Statement *makeFunctionStatement(Statement *name, Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement(name->line, name->code_file);
    tmp->type = set_function;
    tmp->u.set_function.name = name;
    tmp->u.set_function.function = function;
    tmp->u.set_function.parameter = pt;
    tmp->u.set_function.decoration = NULL;
    tmp->u.set_function.first_do = NULL;
    return tmp;
}

Statement *makeLambdaStatement(Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement(function->line, function->code_file);
    tmp->type = base_lambda;
    tmp->u.base_lambda.function = function;
    tmp->u.base_lambda.parameter = pt;
    return tmp;
}

Statement *makeCallStatement(Statement *function, Parameter *pt) {
    Statement *tmp = makeStatement(function->line, function->code_file);
    tmp->type = call_function;
    tmp->u.call_function.function = function;
    tmp->u.call_function.parameter = pt;
    return tmp;
}

Statement *makeSliceStatement(Statement *element, Parameter *index, enum SliceType type) {
    Statement *tmp = makeStatement(element->line, element->code_file);
    tmp->type = slice_;
    tmp->u.slice_.element = element;
    tmp->u.slice_.index = index;
    tmp->u.slice_.type = type;
    return tmp;
}

Statement *makeForStatement(fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = for_branch;
    tmp->u.for_branch.after_do = NULL;
    tmp->u.for_branch.first_do = NULL;
    tmp->u.for_branch.for_list = NULL;
    tmp->u.for_branch.else_list = NULL;
    tmp->u.for_branch.finally = NULL;
    return tmp;
}

Statement *makeIfStatement(fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = if_branch;
    tmp->u.if_branch.if_list = NULL;
    tmp->u.if_branch.else_list = NULL;
    tmp->u.if_branch.finally = NULL;
    return tmp;
}

Statement *makeWhileStatement(fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = while_branch;
    tmp->u.while_branch.type = while_;
    tmp->u.while_branch.while_list = NULL;
    tmp->u.while_branch.else_list = NULL;
    tmp->u.while_branch.finally = NULL;
    tmp->u.while_branch.first = NULL;
    tmp->u.while_branch.after = NULL;
    return tmp;
}

Statement *makeTryStatement(fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = try_branch;
    tmp->u.try_branch.except_list = NULL;
    tmp->u.try_branch.else_list = NULL;
    tmp->u.try_branch.finally = NULL;
    tmp->u.try_branch.try = NULL;
    return tmp;
}

Statement *makeWithStatement(fline line, char *file) {
    Statement *tmp = makeStatement(line, file);
    tmp->type = with_branch;
    tmp->u.with_branch.with_list = NULL;
    tmp->u.with_branch.else_list = NULL;
    tmp->u.with_branch.finally = NULL;
    return tmp;
}

Statement *makeBreakStatement(Statement *times, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = break_cycle;
    tmp->u.break_cycle.times = times;
    return tmp;
}

Statement *makeContinueStatement(Statement *times, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = continue_cycle;
    tmp->u.continue_cycle.times = times;
    return tmp;
}

Statement *makeRegoStatement(Statement *times, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = rego_if;
    tmp->u.rego_if.times = times;
    return tmp;
}

Statement *makeRestartStatement(Statement *times, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = restart;
    tmp->u.restart.times = times;
    return tmp;
}

Statement *makeReturnStatement(Statement *value, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = return_code;
    tmp->u.return_code.value = value;
    return tmp;
}

Statement *makeYieldStatement(Statement *value, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = yield_code;
    tmp->u.yield_code.value = value;
    return tmp;
}

Statement *makeRaiseStatement(Statement *value, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = raise_code;
    tmp->u.raise_code.value = value;
    return tmp;
}

Statement *makeAssertStatement(Statement *conditions, fline line, char *file){
    Statement *tmp = makeStatement(line, file);
    tmp->type = assert_;
    tmp->u.assert.conditions = conditions;
    return tmp;
}

Statement *makeIncludeStatement(Statement *file, fline line, char *file_dir){
    Statement *tmp = makeStatement(line, file_dir);
    tmp->type = include_file;
    tmp->u.include_file.file = file;
    return tmp;
}

Statement *makeImportStatement(Statement *file, Statement *as, bool is_lock) {
    Statement *tmp = makeStatement(file->line, file->code_file);
    tmp->type = import_file;
    tmp->u.import_file.file = file;
    tmp->u.import_file.as = as;
    tmp->u.import_file.is_lock = is_lock;
    return tmp;
}

Statement *makeFromImportStatement(Statement *file, Parameter *as, Parameter *pt, bool is_lock) {
    Statement *tmp = makeStatement(file->line, file->code_file);
    tmp->type = from_import_file;
    tmp->u.from_import_file.file = file;
    tmp->u.from_import_file.as = as;
    tmp->u.from_import_file.pt = pt;
    tmp->u.from_import_file.is_lock = is_lock;
    return tmp;
}

Statement *makeDefaultVarStatement(Parameter *var, fline line, char *file_dir, enum DefaultType type) {
    Statement *tmp = makeStatement(line, file_dir);
    tmp->type = default_var;
    tmp->u.default_var.var = var;
    tmp->u.default_var.default_type = type;
    return tmp;
}

Statement *makeLabelStatement(Statement *var, Statement *command, wchar_t *label, fline line, char *file_dir) {
    Statement *tmp = makeStatement(line, file_dir);
    tmp->type = label_;
    tmp->u.label_.as = var;
    tmp->u.label_.command = command;
    tmp->u.label_.label = memWidecpy(label);
    return tmp;
}

Statement *makeGotoStatement(Statement *return_, Statement *times, Statement *label, fline line, char *file_dir) {
    Statement *tmp = makeStatement(line, file_dir);
    tmp->type = goto_;
    tmp->u.goto_.return_ = return_;
    tmp->u.goto_.times = times;
    tmp->u.goto_.label = label;
    return tmp;
}

Statement *makeDelStatement(Statement *var, fline line, char *file_dir) {
    Statement *tmp = makeStatement(line, file_dir);
    tmp->type = del_;
    tmp->u.del_.var = var;
    return tmp;
}

void connectStatement(Statement *base, Statement *new){
    for (PASS; base->next != NULL; base = base->next)
        PASS;
    base->next = new;
}

void freeStatement(Statement *st){
    Statement *next_tmp = NULL;
    FREE_BASE(st, return_);
    for (PASS; st != NULL; st = next_tmp){
        next_tmp = st->next;
        switch (st->type) {
            case operation:
                freeStatement(st->u.operation.right);
                freeStatement(st->u.operation.left);
                break;
            case base_value:
                if (st->u.base_value.type == link_value)
                    gc_freeStatementLink(&st->u.base_value.value->gc_status);
                else
                    memFree(st->u.base_value.str);
                break;
            case base_var:
                memFree(st->u.base_var.name);
                freeStatement(st->u.base_var.times);
                break;
            case del_:
                freeStatement(st->u.del_.var);
                break;
            case base_svar:
                freeStatement(st->u.base_svar.name);
                freeStatement(st->u.base_svar.times);
                break;
            case base_lambda:
                freeStatement(st->u.base_lambda.function);
                freeParameter(st->u.base_lambda.parameter, true);
                break;
            case set_function:
                freeStatement(st->u.set_function.name);
                freeStatement(st->u.set_function.function);
                freeParameter(st->u.set_function.parameter, true);
                freeDecorationStatement(st->u.set_function.decoration);
                freeStatement(st->u.set_function.first_do);
                break;
            case set_class:
                freeStatement(st->u.set_class.name);
                freeStatement(st->u.set_class.st);
                freeParameter(st->u.set_class.father, true);
                freeDecorationStatement(st->u.set_class.decoration);
                break;
            case call_function:
                freeStatement(st->u.call_function.function);
                freeParameter(st->u.call_function.parameter, true);
                break;
            case slice_:
                freeStatement(st->u.slice_.element);
                freeParameter(st->u.slice_.index, true);
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
                freeStatement(st->u.for_branch.after_do);
                freeStatement(st->u.for_branch.first_do);
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
            case yield_code:
                freeStatement(st->u.yield_code.value);
                break;
            case raise_code:
                freeStatement(st->u.raise_code.value);
                break;
            case include_file:
                freeStatement(st->u.include_file.file);
                break;
            case import_file:
                freeStatement(st->u.import_file.file);
                freeStatement(st->u.import_file.as);
                break;
            case from_import_file:
                freeStatement(st->u.from_import_file.file);
                freeParameter(st->u.from_import_file.as, true);
                freeParameter(st->u.from_import_file.pt, true);
                break;
            case default_var:
                freeParameter(st->u.default_var.var, true);
                break;
            case assert_:
                freeStatement(st->u.assert.conditions);
                break;
            case label_:
                freeStatement(st->u.label_.command);
                freeStatement(st->u.label_.as);
                memFree(st->u.label_.label);
                break;
            case goto_:
                freeStatement(st->u.goto_.return_);
                freeStatement(st->u.goto_.times);
                freeStatement(st->u.goto_.label);
                break;
            default:
                break;
        }
        freeRunInfo(st, true);
        memFree(st->code_file);
        memFree(st);
    }
    return_:
    return;
}

Statement *copyStatement(Statement *st){
    Statement *base_tmp = NULL;
    Statement **tmp = &base_tmp;
    for (PASS; st != NULL; st = st->next, tmp = &(*tmp)->next)
        *tmp = copyStatementCore(st);
    return base_tmp;
}

Statement *copyStatementCore(Statement *st){
    Statement *new = makeStatement(st->line, st->code_file);
    // copyStatement的时候不会复制runInfo的信息
    new->type = st->type;
    new->aut = st->aut;
    new->next = NULL;
    switch (st->type) {
        case base_value:
            new->u.base_value.type = st->u.base_value.type;
            new->u.base_value.value = NULL;
            new->u.base_value.str = NULL;
            if (new->u.base_value.type == link_value) {
                new->u.base_value.value = st->u.base_value.value;
                gc_addStatementLink(&new->u.base_value.value->gc_status);
            }
            else if (new->u.base_value.type == string_str || new->u.base_value.type == number_str)
                new->u.base_value.str = memWidecpy(st->u.base_value.str);
            break;
        case operation:
            new->u.operation.OperationType = st->u.operation.OperationType;
            new->u.operation.right = copyStatement(st->u.operation.right);
            new->u.operation.left = copyStatement(st->u.operation.left);
            break;
        case base_var:
            new->u.base_var.name = memWidecpy(st->u.base_var.name);
            new->u.base_var.times = copyStatement(st->u.base_var.times);
            new->u.base_var.run = st->u.base_var.run;
            break;
        case del_:
            new->u.del_.var = copyStatement(st->u.del_.var);
            break;
        case base_svar:
            new->u.base_svar.name = copyStatement(st->u.base_svar.name);
            new->u.base_svar.times = copyStatement(st->u.base_svar.times);
            new->u.base_svar.run = st->u.base_svar.run;
            new->u.base_svar.is_var = st->u.base_svar.is_var;
            break;
        case base_lambda:
            new->u.base_lambda.function = copyStatement(st->u.base_lambda.function);
            new->u.base_lambda.parameter = copyParameter(st->u.base_lambda.parameter);
            break;
        case set_function:
            new->u.set_function.name = copyStatement(st->u.set_function.name);
            new->u.set_function.function = copyStatement(st->u.set_function.function);
            new->u.set_function.parameter = copyParameter(st->u.set_function.parameter);
            new->u.set_function.decoration = copyDecorationStatement(st->u.set_function.decoration);
            new->u.set_function.first_do = copyStatement(st->u.set_function.first_do);
            break;
        case set_class:
            new->u.set_class.name = copyStatement(st->u.set_class.name);
            new->u.set_class.st = copyStatement(st->u.set_class.st);
            new->u.set_class.father = copyParameter(st->u.set_class.father);
            new->u.set_class.decoration = copyDecorationStatement(st->u.set_class.decoration);
            break;
        case call_function:
            new->u.call_function.function = copyStatement(st->u.call_function.function);
            new->u.call_function.parameter = copyParameter(st->u.call_function.parameter);
            break;
        case slice_:
            new->u.slice_.element = copyStatement(st->u.slice_.element);
            new->u.slice_.index = copyParameter(st->u.slice_.index);
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
            new->u.for_branch.after_do = copyStatement(st->u.for_branch.after_do);
            new->u.for_branch.first_do = copyStatement(st->u.for_branch.first_do);
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
        case yield_code:
            new->u.yield_code.value = copyStatement(st->u.yield_code.value);
            break;
        case raise_code:
            new->u.raise_code.value = copyStatement(st->u.raise_code.value);
            break;
        case include_file:
            new->u.include_file.file = copyStatement(st->u.include_file.file);
            break;
        case import_file:
            new->u.import_file.file = copyStatement(st->u.import_file.file);
            new->u.import_file.as = copyStatement(st->u.import_file.as);
            new->u.import_file.is_lock = st->u.import_file.is_lock;
            break;
        case from_import_file:
            new->u.from_import_file.file = copyStatement(st->u.from_import_file.file);
            new->u.from_import_file.as = copyParameter(st->u.from_import_file.as);
            new->u.from_import_file.pt = copyParameter(st->u.from_import_file.pt);
            new->u.from_import_file.is_lock = st->u.from_import_file.is_lock;
            break;
        case default_var:
            new->u.default_var.var = copyParameter(st->u.default_var.var);
            break;
        case assert_:
            new->u.assert.conditions = copyStatement(st->u.assert.conditions);
            break;
        case label_:
            new->u.label_.command = copyStatement(st->u.label_.command);
            new->u.label_.as = copyStatement(st->u.label_.as);
            new->u.label_.label = memWidecpy(st->u.label_.label);
            break;
        case goto_:
            new->u.goto_.times = copyStatement(st->u.goto_.times);
            new->u.goto_.return_ = copyStatement(st->u.goto_.return_);
            new->u.goto_.label = copyStatement(st->u.goto_.label);
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
    StatementList **tmp = &base;
    for (PASS; *tmp != NULL; tmp = &(*tmp)->next)
        PASS;
    *tmp = new;
    return base;
}

void freeStatementList(StatementList *base){
    StatementList *next = NULL;
    for (PASS; base != NULL; base = next){
        next = base->next;
        freeStatement(base->condition);
        freeStatement(base->code);
        freeStatement(base->var);
        memFree(base);
    }
}

StatementList *copyStatementList(StatementList *sl){
    StatementList *base_tmp = NULL;
    StatementList **tmp = &base_tmp;
    for (PASS; sl != NULL; sl = sl->next, tmp = &(*tmp)->next)
        *tmp = makeStatementList(copyStatement(sl->condition), copyStatement(sl->var), copyStatement(sl->code), sl->type);
    return base_tmp;
}

DecorationStatement *makeDecorationStatement(){
    DecorationStatement *tmp;
    tmp = memCalloc(1, sizeof(DecorationStatement));
    tmp->decoration = NULL;
    tmp->next = NULL;
    return tmp;
}

DecorationStatement *connectDecorationStatement(Statement *decoration, DecorationStatement *base){
    DecorationStatement *tmp = makeDecorationStatement();
    tmp->decoration = decoration;
    tmp->next = base;
    return tmp;
}

void freeDecorationStatement(DecorationStatement *base){
    for (DecorationStatement *next; base != NULL; base = next) {
        next = base->next;
        freeStatement(base->decoration);
        memFree(base);
    }
}

DecorationStatement *copyDecorationStatement(DecorationStatement *ds){
    DecorationStatement *base = NULL;
    DecorationStatement **tmp = &base;
    for (PASS; ds != NULL; tmp = &(*tmp)->next, ds = ds->next)
        *tmp = copyDecorationStatementCore(ds);
    return base;
}

DecorationStatement *copyDecorationStatementCore(DecorationStatement *base){
    DecorationStatement *tmp = makeDecorationStatement();
    tmp->decoration = copyStatement(base->decoration);
    return tmp;
}
