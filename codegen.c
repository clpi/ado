#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static void gen_expr(AST *ast, FILE *out);
static void gen_stmt(AST *ast, FILE *out, int indent);

static int ado_temp_counter = 0;
static int ado_defer_counter = 0;
static int ado_fn_counter = 0;
static int ado_defer_capacity = 0;
static int ado_defer_count = 0;
static int ado_fn_has_defers = 0;
static AST **ado_defers = NULL;
static int ado_fn_id = 0;
static int ado_raise_label = 0;
static int next_temp(void) { return ado_temp_counter++; }
static int next_fn_id(void) { return ado_fn_counter++; }

static AST *const_fold(AST *ast) {
    if (!ast) return NULL;
    switch (ast->type) {
        case AST_BINOP:
            ast->binop.left = const_fold(ast->binop.left);
            ast->binop.right = const_fold(ast->binop.right);
            if (ast->binop.left && ast->binop.left->type == AST_INT &&
                ast->binop.right && ast->binop.right->type == AST_INT) {
                int l = ast->binop.left->int_val, r = ast->binop.right->int_val, result;
                switch (ast->binop.op) {
                    case TOK_PLUS: result = l + r; break;
                    case TOK_MINUS: result = l - r; break;
                    case TOK_STAR: result = l * r; break;
                    case TOK_SLASH: if (r == 0) return ast; result = l / r; break;
                    case TOK_PERCENT: if (r == 0) return ast; result = l % r; break;
                    case TOK_EQ: result = l == r; break;
                    case TOK_NE: result = l != r; break;
                    case TOK_LT: result = l < r; break;
                    case TOK_GT: result = l > r; break;
                    case TOK_LE: result = l <= r; break;
                    case TOK_GE: result = l >= r; break;
                    case TOK_AND: result = l && r; break;
                    case TOK_OR: result = l || r; break;
                    default: return ast;
                }
                ast->type = AST_INT; ast->int_val = result;
            }
            return ast;
        case AST_UNARY:
            ast->unary.operand = const_fold(ast->unary.operand);
            if (ast->unary.op == TOK_NOT && ast->unary.operand && ast->unary.operand->type == AST_INT) {
                ast->type = AST_INT; ast->int_val = !ast->unary.operand->int_val;
            }
            return ast;
        case AST_BLOCK:
            for (int i = 0; i < ast->block.count; i++)
                ast->block.stmts[i] = const_fold(ast->block.stmts[i]);
            return ast;
        case AST_FN:
            ast->fn.body = const_fold(ast->fn.body);
            return ast;
        case AST_LET:
            ast->let.val = const_fold(ast->let.val);
            return ast;
        case AST_ASSIGN:
            ast->assign.val = const_fold(ast->assign.val);
            return ast;
        case AST_IF:
            ast->if_stmt.cond = const_fold(ast->if_stmt.cond);
            ast->if_stmt.then = const_fold(ast->if_stmt.then);
            if (ast->if_stmt.els) ast->if_stmt.els = const_fold(ast->if_stmt.els);
            return ast;
        case AST_WHILE:
            ast->while_stmt.cond = const_fold(ast->while_stmt.cond);
            ast->while_stmt.body = const_fold(ast->while_stmt.body);
            return ast;
        case AST_FOR:
            ast->for_stmt.start = const_fold(ast->for_stmt.start);
            ast->for_stmt.end = const_fold(ast->for_stmt.end);
            if (ast->for_stmt.step) ast->for_stmt.step = const_fold(ast->for_stmt.step);
            ast->for_stmt.body = const_fold(ast->for_stmt.body);
            return ast;
        case AST_RETURN:
            ast->ret.val = const_fold(ast->ret.val);
            return ast;
        case AST_ARRAY:
            for (int i = 0; i < ast->array.count; i++)
                ast->array.elems[i] = const_fold(ast->array.elems[i]);
            return ast;
        case AST_LISTCOMP:
            ast->listcomp.start = const_fold(ast->listcomp.start);
            ast->listcomp.end = const_fold(ast->listcomp.end);
            ast->listcomp.body = const_fold(ast->listcomp.body);
            ast->listcomp.filter = const_fold(ast->listcomp.filter);
            return ast;
        case AST_DESTRUCT:
            ast->destruct.val = const_fold(ast->destruct.val);
            return ast;
        case AST_MATCH:
            ast->match_stmt.expr = const_fold(ast->match_stmt.expr);
            for (int i = 0; i < ast->match_stmt.arm_count; i++) {
                ast->match_stmt.arms[i]->match_arm.pattern = const_fold(ast->match_stmt.arms[i]->match_arm.pattern);
                ast->match_stmt.arms[i]->match_arm.body = const_fold(ast->match_stmt.arms[i]->match_arm.body);
                ast->match_stmt.arms[i]->match_arm.guard = const_fold(ast->match_stmt.arms[i]->match_arm.guard);
            }
            return ast;
        case AST_TRY:
            ast->try_stmt.body = const_fold(ast->try_stmt.body);
            ast->try_stmt.rescue_body = const_fold(ast->try_stmt.rescue_body);
            return ast;
        case AST_RAISE:
            ast->raise_stmt.expr = const_fold(ast->raise_stmt.expr);
            return ast;
        case AST_PRINT:
            for (int i = 0; i < ast->print.count; i++)
                ast->print.vals[i] = const_fold(ast->print.vals[i]);
            return ast;
        case AST_CALL:
            for (int i = 0; i < ast->call.argc; i++)
                ast->call.args[i] = const_fold(ast->call.args[i]);
            return ast;
        default:
            return ast;
    }
}

static int register_defer(AST *expr) {
    if (ado_defer_count >= ado_defer_capacity) {
        ado_defer_capacity = ado_defer_capacity ? ado_defer_capacity * 2 : 8;
        ado_defers = realloc(ado_defers, ado_defer_capacity * sizeof(AST*));
    }
    ado_defers[ado_defer_count++] = expr;
    return ado_defer_count - 1;
}

static void reset_defers(void) {
    ado_defer_count = 0;
}

static int count_defers_expr(AST *ast);
static int count_defers_stmt(AST *ast);

static int count_defers_expr(AST *ast) {
    if (!ast) return 0;
    switch (ast->type) {
        case AST_BINOP:
            return count_defers_expr(ast->binop.left) + count_defers_expr(ast->binop.right);
        case AST_UNARY:
            return count_defers_expr(ast->unary.operand);
        case AST_CALL: {
            int n = 0;
            for (int i = 0; i < ast->call.argc; i++) n += count_defers_expr(ast->call.args[i]);
            return n;
        }
        case AST_ARRAY: {
            int n = 0;
            for (int i = 0; i < ast->array.count; i++) n += count_defers_expr(ast->array.elems[i]);
            return n;
        }
        case AST_LISTCOMP:
            return count_defers_expr(ast->listcomp.start) + count_defers_expr(ast->listcomp.end) + count_defers_expr(ast->listcomp.body) + count_defers_expr(ast->listcomp.filter);
        case AST_DESTRUCT:
            return count_defers_expr(ast->destruct.val);
        case AST_MATCH: {
            int n = count_defers_expr(ast->match_stmt.expr);
            for (int i = 0; i < ast->match_stmt.arm_count; i++) {
                n += count_defers_expr(ast->match_stmt.arms[i]->match_arm.pattern);
                n += count_defers_expr(ast->match_stmt.arms[i]->match_arm.guard);
                n += count_defers_expr(ast->match_stmt.arms[i]->match_arm.body);
            }
            return n;
        }
        case AST_BLOCK: {
            int n = 0;
            for (int i = 0; i < ast->block.count; i++) n += count_defers_stmt(ast->block.stmts[i]);
            return n;
        }
        case AST_RAISE:
            return count_defers_expr(ast->raise_stmt.expr);
        case AST_INDEX:
            return count_defers_expr(ast->index.arr) + count_defers_expr(ast->index.idx);
        case AST_SLICE:
            return count_defers_expr(ast->slice.arr) + count_defers_expr(ast->slice.start) + count_defers_expr(ast->slice.end);
        case AST_RANGE:
            return count_defers_expr(ast->range.start) + count_defers_expr(ast->range.end);
        case AST_LEN:
            return count_defers_expr(ast->len.arr);
        case AST_SAFE_INDEX:
            return count_defers_expr(ast->safe_index.arr) + count_defers_expr(ast->safe_index.idx) + count_defers_expr(ast->safe_index.fallback);
        default:
            return 0;
    }
}

static int count_defers_stmt(AST *ast) {
    if (!ast) return 0;
    int n = (ast->type == AST_DEFER);
    switch (ast->type) {
        case AST_LET:
            return n + count_defers_expr(ast->let.val);
        case AST_IF:
            return n + count_defers_expr(ast->if_stmt.cond) + count_defers_stmt(ast->if_stmt.then) + count_defers_stmt(ast->if_stmt.els);
        case AST_WHILE:
            return n + count_defers_expr(ast->while_stmt.cond) + count_defers_stmt(ast->while_stmt.body);
        case AST_FOR:
            return n + count_defers_expr(ast->for_stmt.start) + count_defers_expr(ast->for_stmt.end) + count_defers_stmt(ast->for_stmt.body);
        case AST_FOREVER:
            return n + count_defers_stmt(ast->forever.body);
        case AST_UNTIL:
            return n + count_defers_expr(ast->until_stmt.cond) + count_defers_stmt(ast->until_stmt.body);
        case AST_RETURN:
            return n + count_defers_expr(ast->ret.val);
        case AST_BLOCK: {
            for (int i = 0; i < ast->block.count; i++) n += count_defers_stmt(ast->block.stmts[i]);
            return n;
        }
        case AST_PRINT:
            for (int i = 0; i < ast->print.count; i++) n += count_defers_expr(ast->print.vals[i]);
            return n;
        case AST_PUSH:
            return n + count_defers_expr(ast->push.arr) + count_defers_expr(ast->push.val);
        case AST_ASSERT:
            return n + count_defers_expr(ast->assert_stmt.expr);
        case AST_SWAP:
            return n + count_defers_expr(ast->swap.left) + count_defers_expr(ast->swap.right);
        case AST_ASSIGN:
            return n + count_defers_expr(ast->assign.target) + count_defers_expr(ast->assign.val);
        case AST_DESTRUCT:
            return n + count_defers_expr(ast->destruct.val);
        case AST_GUARD:
            return n + count_defers_expr(ast->guard_stmt.cond) + count_defers_stmt(ast->guard_stmt.body);
        case AST_MATCH: {
            n += count_defers_expr(ast->match_stmt.expr);
            for (int i = 0; i < ast->match_stmt.arm_count; i++) n += count_defers_stmt(ast->match_stmt.arms[i]);
            return n;
        }
        case AST_MATCH_ARM:
            return n + count_defers_expr(ast->match_arm.pattern) + count_defers_expr(ast->match_arm.guard) + count_defers_stmt(ast->match_arm.body);
        case AST_TRY:
            return n + count_defers_stmt(ast->try_stmt.body) + count_defers_stmt(ast->try_stmt.rescue_body);
        case AST_RAISE:
            return n + count_defers_expr(ast->raise_stmt.expr);
        default:
            return n;
    }
}

static int expr_uses_array(AST *body, const char *name);
static int stmt_uses_array(AST *body, const char *name);

static int expr_uses_array(AST *ast, const char *name) {
    if (!ast) return 0;
    switch (ast->type) {
        case AST_VAR:
            return 0; /* bare ref is not array-typed */
        case AST_INDEX:
            if (ast->index.arr->type == AST_VAR && strcmp(ast->index.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->index.arr, name) || expr_uses_array(ast->index.idx, name);
        case AST_BINOP:
            return expr_uses_array(ast->binop.left, name) || expr_uses_array(ast->binop.right, name);
        case AST_UNARY:
            return expr_uses_array(ast->unary.operand, name);
        case AST_CALL:
            for (int i = 0; i < ast->call.argc; i++)
                if (expr_uses_array(ast->call.args[i], name)) return 1;
            return 0;
        case AST_LEN:
            if (ast->len.arr->type == AST_VAR && strcmp(ast->len.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->len.arr, name);
        case AST_SAFE_INDEX:
            if (ast->safe_index.arr->type == AST_VAR && strcmp(ast->safe_index.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->safe_index.arr, name) || expr_uses_array(ast->safe_index.idx, name) || expr_uses_array(ast->safe_index.fallback, name);
        case AST_SLICE:
            if (ast->slice.arr->type == AST_VAR && strcmp(ast->slice.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->slice.arr, name);
        case AST_ARRAY:
            for (int i = 0; i < ast->array.count; i++)
                if (expr_uses_array(ast->array.elems[i], name)) return 1;
            return 0;
        case AST_BLOCK:
            for (int i = 0; i < ast->block.count; i++)
                if (stmt_uses_array(ast->block.stmts[i], name)) return 1;
            return 0;
        case AST_LISTCOMP:
            return expr_uses_array(ast->listcomp.start, name) ||
                   expr_uses_array(ast->listcomp.end, name) ||
                   expr_uses_array(ast->listcomp.body, name) ||
                   expr_uses_array(ast->listcomp.filter, name);
        case AST_DESTRUCT:
            return expr_uses_array(ast->destruct.val, name);
        case AST_MATCH:
            if (expr_uses_array(ast->match_stmt.expr, name)) return 1;
            for (int i = 0; i < ast->match_stmt.arm_count; i++) {
                if (expr_uses_array(ast->match_stmt.arms[i]->match_arm.pattern, name) ||
                    expr_uses_array(ast->match_stmt.arms[i]->match_arm.guard, name) ||
                    expr_uses_array(ast->match_stmt.arms[i]->match_arm.body, name)) return 1;
            }
            return 0;
        case AST_RAISE:
            return expr_uses_array(ast->raise_stmt.expr, name);
        case AST_RANGE:
            return expr_uses_array(ast->range.start, name) || expr_uses_array(ast->range.end, name);
        default:
            return 0;
    }
}

static int stmt_uses_array(AST *ast, const char *name) {
    if (!ast) return 0;
    switch (ast->type) {
        case AST_LET:
            return expr_uses_array(ast->let.val, name);
        case AST_IF:
            return expr_uses_array(ast->if_stmt.cond, name) ||
                   stmt_uses_array(ast->if_stmt.then, name) ||
                   stmt_uses_array(ast->if_stmt.els, name);
        case AST_WHILE:
            return expr_uses_array(ast->while_stmt.cond, name) ||
                   stmt_uses_array(ast->while_stmt.body, name);
        case AST_FOR:
            return expr_uses_array(ast->for_stmt.start, name) ||
                   expr_uses_array(ast->for_stmt.end, name) ||
                   stmt_uses_array(ast->for_stmt.body, name);
        case AST_FOREVER:
            return stmt_uses_array(ast->forever.body, name);
        case AST_UNTIL:
            return expr_uses_array(ast->until_stmt.cond, name) ||
                   stmt_uses_array(ast->until_stmt.body, name);
        case AST_RETURN:
            return expr_uses_array(ast->ret.val, name);
        case AST_BLOCK:
            for (int i = 0; i < ast->block.count; i++)
                if (stmt_uses_array(ast->block.stmts[i], name)) return 1;
            return 0;
        case AST_ASSIGN:
            if (ast->assign.target->type == AST_INDEX && ast->assign.target->index.arr->type == AST_VAR &&
                strcmp(ast->assign.target->index.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->assign.target, name) || expr_uses_array(ast->assign.val, name);
        case AST_DESTRUCT:
            return expr_uses_array(ast->destruct.val, name);
        case AST_PUSH:
            if (ast->push.arr->type == AST_VAR && strcmp(ast->push.arr->var_name, name) == 0) return 1;
            return expr_uses_array(ast->push.arr, name) || expr_uses_array(ast->push.val, name);
        case AST_PRINT:
            for (int i = 0; i < ast->print.count; i++)
                if (expr_uses_array(ast->print.vals[i], name)) return 1;
            return 0;
        case AST_ASSERT:
            return expr_uses_array(ast->assert_stmt.expr, name);
        case AST_RAISE:
            return expr_uses_array(ast->raise_stmt.expr, name);
        case AST_DEFER:
            return expr_uses_array(ast->defer_stmt.expr, name);
        case AST_GUARD:
            return expr_uses_array(ast->guard_stmt.cond, name) || stmt_uses_array(ast->guard_stmt.body, name);
        case AST_MATCH:
            if (expr_uses_array(ast->match_stmt.expr, name)) return 1;
            for (int i = 0; i < ast->match_stmt.arm_count; i++)
                if (stmt_uses_array(ast->match_stmt.arms[i], name)) return 1;
            return 0;
        case AST_MATCH_ARM:
            return expr_uses_array(ast->match_arm.pattern, name) || expr_uses_array(ast->match_arm.guard, name) || stmt_uses_array(ast->match_arm.body, name);
        case AST_TRY:
            return stmt_uses_array(ast->try_stmt.body, name) || stmt_uses_array(ast->try_stmt.rescue_body, name);
        case AST_SWAP:
            return expr_uses_array(ast->swap.left, name) || expr_uses_array(ast->swap.right, name);
        default:
            return 0;
    }
}

static int param_is_array(AST *body, const char *name) {
    return stmt_uses_array(body, name);
}

static void gen_indent(FILE *out, int indent);
static void gen_stmt_as_body(AST *ast, FILE *out, int indent);

static void gen_stmt_as_body(AST *ast, FILE *out, int indent) {
    if (!ast) return;
    switch (ast->type) {
        case AST_PRINT:
        case AST_ASSIGN:
        case AST_PUSH:
        case AST_ASSERT:
        case AST_SWAP:
        case AST_DESTRUCT:
        case AST_TRY:
        case AST_RAISE:
        case AST_IF:
        case AST_WHILE:
        case AST_FOR:
        case AST_UNTIL:
        case AST_FOREVER:
        case AST_BREAK:
        case AST_CONTINUE:
        case AST_RETURN:
        case AST_GUARD:
        case AST_DEFER:
        case AST_MATCH:
        case AST_BLOCK:
            gen_stmt(ast, out, indent);
            break;
        default:
            gen_indent(out, indent);
            gen_expr(ast, out);
            fprintf(out, ";\n");
            break;
    }
}

static void gen_str_escape(const char *s, FILE *out) {
    while (*s) {
        switch (*s) {
            case '\n': fprintf(out, "\\n"); break;
            case '\t': fprintf(out, "\\t"); break;
            case '"': fprintf(out, "\\\""); break;
            case '\\': fprintf(out, "\\\\"); break;
            case '%': fprintf(out, "%%"); break;
            default: fputc(*s, out);
        }
        s++;
    }
}

static void gen_expr(AST *ast, FILE *out) {
    if (!ast) {
        fprintf(out, "0/*null*/");
        return;
    }
    switch (ast->type) {
        case AST_INT:
            fprintf(out, "%d", ast->int_val);
            break;
        case AST_STR:
            fprintf(out, "\"");
            gen_str_escape(ast->str_val, out);
            fprintf(out, "\"");
            break;
        case AST_BOOL:
            fprintf(out, "%d", ast->bool_val);
            break;
        case AST_VAR:
            fprintf(out, "%s", ast->var_name);
            break;
        case AST_BINOP:
            fprintf(out, "(");
            gen_expr(ast->binop.left, out);
            switch (ast->binop.op) {
                case TOK_PLUS: fprintf(out, "+"); break;
                case TOK_MINUS: fprintf(out, "-"); break;
                case TOK_STAR: fprintf(out, "*"); break;
                case TOK_SLASH: fprintf(out, "/"); break;
                case TOK_PERCENT: fprintf(out, "%%"); break;
                case TOK_EQ: fprintf(out, "=="); break;
                case TOK_NE: fprintf(out, "!="); break;
                case TOK_LT: fprintf(out, "<"); break;
                case TOK_GT: fprintf(out, ">"); break;
                case TOK_LE: fprintf(out, "<="); break;
                case TOK_GE: fprintf(out, ">="); break;
                case TOK_AND: fprintf(out, "&&"); break;
                case TOK_OR: fprintf(out, "||"); break;
                default: break;
            }
            gen_expr(ast->binop.right, out);
            fprintf(out, ")");
            break;
        case AST_UNARY:
            fprintf(out, "(");
            if (ast->unary.op == TOK_NOT) {
                fprintf(out, "!");
            } else if (ast->unary.op == TOK_MINUS) {
                fprintf(out, "-");
            }
            gen_expr(ast->unary.operand, out);
            fprintf(out, ")");
            break;
        case AST_CALL: {
            const char *name = ast->call.name;
            if (strcmp(name, "abs") == 0) name = "ado_abs";
            else if (strcmp(name, "min") == 0) name = "ado_min";
            else if (strcmp(name, "max") == 0) name = "ado_max";
            else if (strcmp(name, "clamp") == 0) name = "ado_clamp";
            else if (strcmp(name, "pow") == 0) name = "ado_pow";
            else if (strcmp(name, "sign") == 0) name = "ado_sign";
            else if (strcmp(name, "is_even") == 0) name = "ado_is_even";
            else if (strcmp(name, "is_odd") == 0) name = "ado_is_odd";
            else if (strcmp(name, "gcd") == 0) name = "ado_gcd";
            else if (strcmp(name, "lcm") == 0) name = "ado_lcm";
            else if (strcmp(name, "factorial") == 0) name = "ado_factorial";
            fprintf(out, "%s(", name);
            for (int i = 0; i < ast->call.argc; i++) {
                gen_expr(ast->call.args[i], out);
                if (i < ast->call.argc - 1) fprintf(out, ",");
            }
            fprintf(out, ")");
            break;
        }
        case AST_ARRAY:
            fprintf(out, "ado_make_array((int[]){");
            for (int i = 0; i < ast->array.count; i++) {
                gen_expr(ast->array.elems[i], out);
                if (i < ast->array.count - 1) fprintf(out, ",");
            }
            fprintf(out, "},%d)", ast->array.count);
            break;
        case AST_INDEX:
            gen_expr(ast->index.arr, out);
            fprintf(out, ".data[");
            gen_expr(ast->index.idx, out);
            fprintf(out, "]");
            break;
        case AST_SAFE_INDEX: {
            int n = next_temp();
            fprintf(out, "({ AdoArray _ado_safe_%d=", n);
            gen_expr(ast->safe_index.arr, out);
            fprintf(out, "; int _ado_idx_%d=", n);
            gen_expr(ast->safe_index.idx, out);
            fprintf(out, "; int _ado_fallback_%d=", n);
            if (ast->safe_index.fallback) gen_expr(ast->safe_index.fallback, out);
            else fprintf(out, "0");
            fprintf(out, "; _ado_idx_%d>=0&&_ado_idx_%d<_ado_safe_%d.len?_ado_safe_%d.data[_ado_idx_%d]:_ado_fallback_%d; })", n, n, n, n, n, n);
            break;
        }
        case AST_SLICE:
            // Create a compound literal for the slice array
            // ado_slice creates a new array with elements from arr[start..end] (exclusive)
            fprintf(out, "({ int _s=");
            gen_expr(ast->slice.start, out);
            fprintf(out, ", _e=");
            gen_expr(ast->slice.end, out);
            fprintf(out, ", _len=_e>_s?_e-_s:0, _i; AdoArray _slice=ado_make_array((int[]){},_len); for(_i=0;_i<_len;_i++) _slice.data[_i]=");
            gen_expr(ast->slice.arr, out);
            fprintf(out, ".data[_s+_i]; _slice; })");
            break;
        case AST_RANGE:
            fprintf(out, "({ int _f=");
            gen_expr(ast->range.start, out);
            fprintf(out, ", _t=");
            gen_expr(ast->range.end, out);
            fprintf(out, ", _n, _i; _n=_t>=_f?_t-_f+1:0; AdoArray _r=ado_make_array((int[]){},_n); for(_i=0;_i<_n;_i++)_r.data[_i]=_f+_i; _r;})");
            break;
        case AST_LEN:
            gen_expr(ast->len.arr, out);
            fprintf(out, ".len");
            break;
        case AST_PIPE:
            fprintf(out, "%s(", ast->call.name);
            gen_expr(ast->call.args[0], out);
            fprintf(out, ")");
            break;
        case AST_BLOCK:
            fprintf(out, "({");
            for (int i = 0; i < ast->block.count - 1; i++) gen_stmt(ast->block.stmts[i], out, 1);
            if (ast->block.count > 0) {
                AST *last = ast->block.stmts[ast->block.count - 1];
                if (last->type == AST_LET) {
                    gen_stmt(last, out, 1);
                    fprintf(out, "%s", last->let.name);
                } else if (last->type == AST_ASSIGN) {
                    gen_expr(last->assign.target, out);
                    fprintf(out, "=");
                    gen_expr(last->assign.val, out);
                } else if (last->type == AST_PUSH || last->type == AST_PRINT || last->type == AST_ASSERT ||
                           last->type == AST_SWAP || last->type == AST_IF || last->type == AST_WHILE ||
                           last->type == AST_FOR || last->type == AST_UNTIL || last->type == AST_FOREVER ||
                           last->type == AST_BREAK || last->type == AST_CONTINUE || last->type == AST_RETURN ||
                           last->type == AST_GUARD || last->type == AST_DEFER || last->type == AST_MATCH ||
                           last->type == AST_TRY) {
                    gen_stmt(last, out, 1);
                    fprintf(out, "0");
                } else {
                    gen_expr(last, out);
                }
            } else {
                fprintf(out, "0");
            }
            fprintf(out, ";})");
            break;
        case AST_LISTCOMP: {
            int n = next_temp();
            fprintf(out, "({ int _ado_lc_start_%d=", n);
            gen_expr(ast->listcomp.start, out);
            fprintf(out, ", _ado_lc_end_%d=", n);
            gen_expr(ast->listcomp.end, out);
            fprintf(out, ", _ado_lc_i_%d; AdoArray _ado_lc_%d=ado_make_array((int[]){},0); for(_ado_lc_i_%d=_ado_lc_start_%d;_ado_lc_i_%d<_ado_lc_end_%d;_ado_lc_i_%d++){__auto_type %s=_ado_lc_i_%d;", n, n, n, n, n, n, n, ast->listcomp.var, n);
            if (ast->listcomp.filter) {
                fprintf(out, "if(");
                gen_expr(ast->listcomp.filter, out);
                fprintf(out, "){ado_push(&_ado_lc_%d,", n);
                gen_expr(ast->listcomp.body, out);
                fprintf(out, ");};}");
            } else {
                fprintf(out, "ado_push(&_ado_lc_%d,", n);
                gen_expr(ast->listcomp.body, out);
                fprintf(out, ");};");
            }
            fprintf(out, "_ado_lc_%d;})", n);
            break;
        }
        case AST_MATCH: {
            int n = next_temp();
            fprintf(out, "({__auto_type _ado_match_expr_%d=", n);
            gen_expr(ast->match_stmt.expr, out);
            fprintf(out, ";int _ado_match_out_%d=0;", n);
            int emitted_default = 0;
            for (int i = 0; i < ast->match_stmt.arm_count; i++) {
                AST *arm = ast->match_stmt.arms[i];
                if (arm->match_arm.is_default) {
                    if (!emitted_default) {
                        fprintf(out, "{_ado_match_out_%d=", n);
                        gen_expr(arm->match_arm.body, out);
                        fprintf(out, ";}");
                        emitted_default = 1;
                    }
                    continue;
                }
                fprintf(out, i == 0 ? "if(" : "else if(");
                gen_expr(arm->match_arm.pattern, out);
                fprintf(out, "==_ado_match_expr_%d", n);
                if (arm->match_arm.guard) {
                    fprintf(out, "&&(");
                    gen_expr(arm->match_arm.guard, out);
                    fprintf(out, ")");
                }
                fprintf(out, "){_ado_match_out_%d=", n);
                gen_expr(arm->match_arm.body, out);
                fprintf(out, ";}");
            }
            fprintf(out, "_ado_match_out_%d;})", n);
            break;
        }
        default:
            break;
    }
}

static void gen_stmt(AST *ast, FILE *out, int indent);

static void gen_block(AST *ast, FILE *out, int indent) {
    for (int i = 0; i < ast->block.count; i++) {
        if (ast->block.stmts[i])
            gen_stmt(ast->block.stmts[i], out, indent);
    }
}

static void gen_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) fprintf(out, "  ");
}

static void gen_stmt(AST *ast, FILE *out, int indent) {
    switch (ast->type) {
        case AST_LET:
            gen_indent(out, indent);
            fprintf(out, "__auto_type %s=", ast->let.name);
            gen_expr(ast->let.val, out);
            fprintf(out, ";\n");
            break;
        case AST_IF:
            gen_indent(out, indent);
            if (ast->if_stmt.invert_cond) fprintf(out, "if(!(");
            else fprintf(out, "if(");
            gen_expr(ast->if_stmt.cond, out);
            fprintf(out, ast->if_stmt.invert_cond ? ")){\n" : "){\n");
            gen_block(ast->if_stmt.then, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "}");
            if (ast->if_stmt.els) {
                fprintf(out, "else{\n");
                gen_block(ast->if_stmt.els, out, indent + 1);
                gen_indent(out, indent);
                fprintf(out, "}");
            }
            fprintf(out, "\n");
            break;
        case AST_WHILE:
            gen_indent(out, indent);
            fprintf(out, "while(");
            gen_expr(ast->while_stmt.cond, out);
            fprintf(out, "){\n");
            gen_block(ast->while_stmt.body, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        case AST_FOR:
            gen_indent(out, indent);
            fprintf(out, "for(int %s=", ast->for_stmt.var);
            gen_expr(ast->for_stmt.start, out);
            fprintf(out, ";%s<", ast->for_stmt.var);
            gen_expr(ast->for_stmt.end, out);
            if (ast->for_stmt.step) {
                fprintf(out, ";%s+=", ast->for_stmt.var);
                gen_expr(ast->for_stmt.step, out);
            } else {
                fprintf(out, ";%s++", ast->for_stmt.var);
            }
            fprintf(out, "){\n");
            gen_block(ast->for_stmt.body, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        case AST_FOREVER:
            gen_indent(out, indent);
            fprintf(out, "while(1){\n");
            gen_block(ast->forever.body, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        case AST_BREAK:
            gen_indent(out, indent);
            fprintf(out, "break;\n");
            break;
        case AST_CONTINUE:
            gen_indent(out, indent);
            fprintf(out, "continue;\n");
            break;
        case AST_RETURN:
            gen_indent(out, indent);
            if (ado_fn_has_defers) {
                fprintf(out, "_ado_return_%d=", ado_fn_id);
                gen_expr(ast->ret.val, out);
                fprintf(out, ";\n");
                gen_indent(out, indent);
                fprintf(out, "_ado_has_return_%d=1;\n", ado_fn_id);
                gen_indent(out, indent);
                fprintf(out, "goto _ado_cleanup_%d;\n", ado_fn_id);
            } else {
                fprintf(out, "return ");
                gen_expr(ast->ret.val, out);
                fprintf(out, ";\n");
            }
            break;
        case AST_PRINT:
            gen_indent(out, indent);
            // Build format string first
            fprintf(out, "printf(\"");
            for (int i = 0; i < ast->print.count; i++) {
                if (i > 0) fprintf(out, " ");
                AST *val = ast->print.vals[i];
                if (val->type == AST_STR) {
                    gen_str_escape(val->str_val, out);
                } else if (val->type == AST_BOOL) {
                    fprintf(out, "%%s");
                } else {
                    fprintf(out, "%%d");
                }
            }
            fprintf(out, "\\n\"");
            // Now output arguments
            for (int i = 0; i < ast->print.count; i++) {
                AST *val = ast->print.vals[i];
                if (val->type == AST_STR) {
                    // Strings are in format string, skip
                } else if (val->type == AST_BOOL) {
                    fprintf(out, ",");
                    fprintf(out, "(");
                    gen_expr(val, out);
                    fprintf(out, ")?\"true\":\"false\"");
                } else {
                    fprintf(out, ",");
                    gen_expr(val, out);
                }
            }
            fprintf(out, ");\n");
            break;
        case AST_DEFER:
            register_defer(ast->defer_stmt.expr);
            break;
        case AST_GUARD:
            gen_indent(out, indent);
            fprintf(out, "if(!(");
            gen_expr(ast->guard_stmt.cond, out);
            fprintf(out, ")){\n");
            if (ast->guard_stmt.body) gen_block(ast->guard_stmt.body, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "return 0;\n");
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        case AST_UNTIL:
            gen_indent(out, indent);
            fprintf(out, "while(!(");
            gen_expr(ast->until_stmt.cond, out);
            fprintf(out, ")){\n");
            gen_block(ast->until_stmt.body, out, indent + 1);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        case AST_PUSH: {
            gen_indent(out, indent);
            fprintf(out, "ado_push(&");
            gen_expr(ast->push.arr, out);
            fprintf(out, ",");
            gen_expr(ast->push.val, out);
            fprintf(out, ");\n");
            break;
        }
        case AST_ASSERT:
            gen_indent(out, indent);
            fprintf(out, "if(!(");
            gen_expr(ast->assert_stmt.expr, out);
            fprintf(out, ")){fprintf(stderr,\"Ado assertion failed\");exit(1);}\n");
            break;
        case AST_RAISE: {
            int n = ado_raise_label ? ado_raise_label : next_temp();
            gen_indent(out, indent);
            fprintf(out, "_ado_exception_pending=1;_ado_exception_value=");
            gen_expr(ast->raise_stmt.expr, out);
            fprintf(out, ";goto _ado_raise_%d;\n", n);
            break;
        }
        case AST_DESTRUCT: {
            int n = next_temp();
            gen_indent(out, indent);
            fprintf(out, "__auto_type _ado_destruct_%d=", n);
            gen_expr(ast->destruct.val, out);
            fprintf(out, ";\n");
            for (int i = 0; i < ast->destruct.count; i++) {
                gen_indent(out, indent);
                fprintf(out, "__auto_type %s=", ast->destruct.names[i]);
                fprintf(out, "_ado_destruct_%d.data[%d];\n", n, i);
            }
            if (ast->destruct.has_rest && ast->destruct.rest_name) {
                int start = ast->destruct.count;
                gen_indent(out, indent);
                fprintf(out, "__auto_type %s=ado_make_array((int[]){},_ado_destruct_%d.len>%d?_ado_destruct_%d.len-%d:0);", ast->destruct.rest_name, n, start, n, start);
                fprintf(out, "\n");
                gen_indent(out, indent);
                fprintf(out, "for(int _ado_rest_i_%d=0;_ado_rest_i_%d+_ado_destruct_%d.len>%d&&_ado_rest_i_%d+_ado_destruct_%d.len<_ado_destruct_%d.len;_ado_rest_i_%d++)_ado_destruct_%d.data[_ado_rest_i_%d]=_ado_destruct_%d.data[_ado_rest_i_%d+%d];\n", n, n, n, start, n, n, n, n, n, n, n, n, start);
            }
            break;
        }
        case AST_TRY: {
            int n = next_temp();
            gen_indent(out, indent);
            fprintf(out, "{\n");
            gen_indent(out, indent + 1);
            fprintf(out, "int _ado_try_pending_%d=_ado_exception_pending;int _ado_try_value_%d=_ado_exception_value;_ado_exception_pending=0;\n", n, n);
            int saved_raise_label = ado_raise_label;
            ado_raise_label = n;
            gen_block(ast->try_stmt.body, out, indent + 1);
            ado_raise_label = saved_raise_label;
            gen_indent(out, indent + 1);
            fprintf(out, "_ado_raise_%d:;\n", n);
            gen_indent(out, indent + 1);
            fprintf(out, "if(_ado_exception_pending){\n");
            ado_raise_label = 0;
            if (ast->try_stmt.rescue_var) {
                gen_indent(out, indent + 2);
                fprintf(out, "__auto_type %s=_ado_exception_value;\n", ast->try_stmt.rescue_var);
            }
            gen_block(ast->try_stmt.rescue_body, out, indent + 2);
            ado_raise_label = saved_raise_label;
            gen_indent(out, indent + 2);
            fprintf(out, "_ado_exception_pending=0;\n");
            gen_indent(out, indent + 1);
            fprintf(out, "}\n");
            gen_indent(out, indent + 1);
            fprintf(out, "_ado_exception_pending=_ado_try_pending_%d;_ado_exception_value=_ado_try_value_%d;\n", n, n);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        }
        case AST_SWAP: {
            int n = next_temp();
            gen_indent(out, indent);
            fprintf(out, "__auto_type _ado_swap_%d=", n);
            gen_expr(ast->swap.left, out);
            fprintf(out, ";\n");
            gen_indent(out, indent);
            gen_expr(ast->swap.left, out);
            fprintf(out, "=");
            gen_expr(ast->swap.right, out);
            fprintf(out, ";\n");
            gen_indent(out, indent);
            gen_expr(ast->swap.right, out);
            fprintf(out, "=_ado_swap_%d;\n", n);
            break;
        }
        case AST_ASSIGN:
            gen_indent(out, indent);
            gen_expr(ast->assign.target, out);
            fprintf(out, "=");
            gen_expr(ast->assign.val, out);
            fprintf(out, ";\n");
            break;
        case AST_BLOCK:
            gen_block(ast, out, indent);
            break;
        case AST_MATCH: {
            int n = next_temp();
            gen_indent(out, indent);
            fprintf(out, "__auto_type _ado_match_%d=", n);
            gen_expr(ast->match_stmt.expr, out);
            fprintf(out, ";\n");
            int emitted_default = 0;
            for (int i = 0; i < ast->match_stmt.arm_count; i++) {
                AST *arm = ast->match_stmt.arms[i];
                if (arm->match_arm.is_default) {
                    if (!emitted_default) {
                        gen_indent(out, indent);
                        fprintf(out, "else{\n");
                        gen_stmt_as_body(arm->match_arm.body, out, indent + 1);
                        gen_indent(out, indent);
                        fprintf(out, "}\n");
                        emitted_default = 1;
                    }
                    continue;
                }
                gen_indent(out, indent);
                fprintf(out, i == 0 ? "if(" : "else if(");
                fprintf(out, "_ado_match_%d==", n);
                gen_expr(arm->match_arm.pattern, out);
                if (arm->match_arm.guard) {
                    fprintf(out, "&&(");
                    gen_expr(arm->match_arm.guard, out);
                    fprintf(out, ")");
                }
                fprintf(out, "){\n");
                gen_stmt_as_body(arm->match_arm.body, out, indent + 1);
                gen_indent(out, indent);
                fprintf(out, "}\n");
            }
            break;
        }
        case AST_ENUM:
            break;
        default:
            gen_indent(out, indent);
            gen_expr(ast, out);
            fprintf(out, ";\n");
            break;
    }
}

void codegen(AST *ast, FILE *out) {
    const_fold(ast);
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n");
    // Runtime library (inline)
    fprintf(out, "typedef struct { int *data; int len; int cap; } AdoArray;\n");
    fprintf(out, "static int _ado_exception_pending=0;\n");
    fprintf(out, "static int _ado_exception_value=0;\n");
    fprintf(out, "static int ado_raise(int v){_ado_exception_pending=1;_ado_exception_value=v;return 0;}\n");
    fprintf(out, "static AdoArray ado_make_array(int *init, int c) {\n");
    fprintf(out, "  AdoArray a; a.len=c; a.cap=c>0?c:4; a.data=malloc(a.cap*sizeof(int));\n");
    fprintf(out, "  for(int i=0;i<c;i++) a.data[i]=init[i]; return a;\n");
    fprintf(out, "}\n");
    fprintf(out, "static void ado_push(AdoArray *a, int v) {\n");
    fprintf(out, "  if(a->len>=a->cap){a->cap=a->cap?a->cap*2:4;a->data=realloc(a->data,a->cap*sizeof(int));}\n");
    fprintf(out, "  a->data[a->len++]=v;\n");
    fprintf(out, "}\n");
    fprintf(out, "static int ado_abs(int x) { return x<0?-x:x; }\n");
    fprintf(out, "static int ado_min(int a, int b) { return a<b?a:b; }\n");
    fprintf(out, "static int ado_max(int a, int b) { return a>b?a:b; }\n");
    fprintf(out, "static int ado_clamp(int x, int l, int h) { return x<l?l:(x>h?h:x); }\n");
    fprintf(out, "static int ado_pow(int b, int e) { int r=1; while(e){if(e&1)r*=b;b*=b;e>>=1;} return r; }\n");
    fprintf(out, "static int ado_sign(int x) { return x>0?1:(x<0?-1:0); }\n");
    fprintf(out, "static int ado_is_even(int x) { return x%%2==0; }\n");
    fprintf(out, "static int ado_is_odd(int x) { return x%%2!=0; }\n");
    fprintf(out, "static int ado_gcd(int a, int b) { while(b){int t=b;b=a%%b;a=t;} return a; }\n");
    fprintf(out, "static int ado_lcm(int a, int b) { return a/ado_gcd(a,b)*b; }\n");
    fprintf(out, "static int ado_contains(AdoArray a, int v) { for(int i=0;i<a.len;i++){if(a.data[i]==v)return 1;} return 0; }\n");
    fprintf(out, "static int ado_pop(AdoArray *a) { return a->data[--a->len]; }\n");
    fprintf(out, "static int ado_reverse(AdoArray a) { for(int i=0;i<a.len/2;i++){int t=a.data[i];a.data[i]=a.data[a.len-1-i];a.data[a.len-1-i]=t;} return 0; }\n");
    fprintf(out, "static int ado_remove(AdoArray *a, int i) { for(int j=i;j<a->len-1;j++)a->data[j]=a->data[j+1]; a->len--; return 0; }\n");
    fprintf(out, "static int ado_insert(AdoArray *a, int i, int v) { if(a->len>=a->cap){a->cap=a->cap?a->cap*2:4;a->data=realloc(a->data,a->cap*sizeof(int));} for(int j=a->len;j>i;j--)a->data[j]=a->data[j-1]; a->data[i]=v; a->len++; return 0; }\n");
    fprintf(out, "static int ado_factorial(int n) { int r=1; for(int i=2;i<=n;i++) r*=i; return r; }\n");
    fprintf(out, "static int ado_fib(int n) { int a=0,b=1; for(int i=0;i<n;i++){int t=b;b=a+b;a=t;} return a; }\n");
    fprintf(out, "static int ado_sum(AdoArray a) { int s=0; for(int i=0;i<a.len;i++) s+=a.data[i]; return s; }\n");
    fprintf(out, "static int ado_avg(AdoArray a) { return a.len?ado_sum(a)/a.len:0; }\n");
    fprintf(out, "static AdoArray ado_take(AdoArray a, int n) { int c=n<a.len?n:a.len; AdoArray r=ado_make_array((int[]){},c); for(int i=0;i<c;i++) r.data[i]=a.data[i]; return r; }\n");
    fprintf(out, "static AdoArray ado_drop(AdoArray a, int n) { int c=n<a.len?a.len-n:0; AdoArray r=ado_make_array((int[]){},c); for(int i=0;i<c;i++) r.data[i]=a.data[i+n]; return r; }\n");
    fprintf(out, "static AdoArray ado_concat(AdoArray a, AdoArray b) { AdoArray r=ado_make_array((int[]){},a.len+b.len); for(int i=0;i<a.len;i++) r.data[i]=a.data[i]; for(int j=0;j<b.len;j++) r.data[a.len+j]=b.data[j]; return r; }\n");
    fprintf(out, "static AdoArray ado_fill(int n, int v) { AdoArray a; a.len=n; a.cap=n>0?n:4; a.data=malloc(a.cap*sizeof(int)); for(int i=0;i<n;i++) a.data[i]=v; return a; }\n");
    fprintf(out, "static int ado_count_if(AdoArray a, int v) { int c=0; for(int i=0;i<a.len;i++) if(a.data[i]==v) c++; return c; }\n");
    fprintf(out, "static AdoArray ado_filter(AdoArray a, int v) { AdoArray r=ado_make_array((int[]){},0); for(int i=0;i<a.len;i++) if(a.data[i]!=v){ado_push(&r,a.data[i]);} return r; }\n");
    fprintf(out, "static int ado_find(AdoArray a, int v) { for(int i=0;i<a.len;i++) if(a.data[i]==v) return i; return -1; }\n");
    fprintf(out, "static int ado_all(AdoArray a) { for(int i=0;i<a.len;i++) if(!a.data[i]) return 0; return 1; }\n");
    fprintf(out, "static int ado_any(AdoArray a) { for(int i=0;i<a.len;i++) if(a.data[i]) return 1; return 0; }\n");
    fprintf(out, "#include <curl/curl.h>\n");
    fprintf(out, "static size_t ado_http_write(void *contents, size_t size, size_t nmemb, void *userp) { size_t total=size*nmemb; char **buf=(char**)userp; size_t cur=*buf?strlen(*buf):0; size_t new_total=cur+total; if(new_total>=4096) total=4096-cur; if(total>0){*buf=realloc(*buf,new_total+1); memcpy(*buf+cur,contents,total); (*buf)[new_total]=0; } return size*nmemb; }\n");
    fprintf(out, "static long ado_http_perform(const char *url, const char *method, const char *post_data) { CURL *curl=curl_easy_init(); if(!curl) return -1; char *resp=NULL; curl_easy_setopt(curl,CURLOPT_URL,url); curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ado_http_write); curl_easy_setopt(curl,CURLOPT_WRITEDATA,&resp); curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L); curl_easy_setopt(curl,CURLOPT_MAXREDIRS,3L); if(method && strcmp(method,\"POST\")==0){curl_easy_setopt(curl,CURLOPT_POST,1L); if(post_data) curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_data);} else if(method && strcmp(method,\"PUT\")==0){curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,\"PUT\"); if(post_data) curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_data);} else if(method && strcmp(method,\"DELETE\")==0){curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,\"DELETE\");} else {curl_easy_setopt(curl,CURLOPT_HTTPGET,1L);} CURLcode rc=curl_easy_perform(curl); long code=-1; if(rc==CURLE_OK) curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&code); curl_easy_cleanup(curl); if(resp){fprintf(stderr,\"%%s\",resp); free(resp);} if(rc!=CURLE_OK){fprintf(stderr,\"ado_http: %%s: %%s\\n\",url,curl_easy_strerror(rc)); return -1;} return code; }\n");
    fprintf(out, "static int ado_http_get(const char *url) { long code=ado_http_perform(url,\"GET\",NULL); return (int)code; }\n");
    fprintf(out, "static int ado_http_post(const char *url, const char *body) { long code=ado_http_perform(url,\"POST\",body); return (int)code; }\n");
    fprintf(out, "static int ado_http_put(const char *url, const char *body) { long code=ado_http_perform(url,\"PUT\",body); return (int)code; }\n");
    fprintf(out, "static int ado_http_delete(const char *url) { long code=ado_http_perform(url,\"DELETE\",NULL); return (int)code; }\n");
    fprintf(out, "static int ado_http_status(const char *url) { long code=ado_http_perform(url,\"GET\",NULL); return (int)code; }\n");
    fprintf(out, "#include <time.h>\n");
    fprintf(out, "#include <unistd.h>\n");
    fprintf(out, "static int ado_getenv(const char *name) { char *val=getenv(name); if(!val) return 0; printf(\"%%s\",val); return 1; }\n");
    fprintf(out, "static int ado_exit(int code) { exit(code); return 0; }\n");
    fprintf(out, "static int ado_read_file(const char *path) { FILE *f=fopen(path,\"r\"); if(!f) return -1; char buf[4096]; size_t n; while((n=fread(buf,1,sizeof(buf)-1,f))){ buf[n]=0; printf(\"%%s\",buf);} fclose(f); return 0; }\n");
    fprintf(out, "static int ado_write_file(const char *path, int content) { FILE *f=fopen(path,\"w\"); if(!f) return -1; fprintf(f,\"%%d\",content); fclose(f); return 0; }\n");
    fprintf(out, "static int ado_file_exists(const char *path) { FILE *f=fopen(path,\"r\"); if(f){fclose(f);return 1;} return 0; }\n");
    fprintf(out, "static int ado_sleep(int ms) { usleep(ms*1000); return 0; }\n");
    fprintf(out, "static int ado_time(void) { return (int)time(NULL); }\n");
    fprintf(out, "static int ado_random(int max) { return rand()%%max; }\n");
    fprintf(out, "static int ado_capacity(AdoArray a) { return a.cap; }\n");
    fprintf(out, "static int ado_reserve(AdoArray *a, int c) { if(c>a->cap){a->cap=c;a->data=realloc(a->data,c*4);} return 0; }\n");
    fprintf(out, "static int ado_shrink_to_fit(AdoArray *a) { if(a->cap>a->len){a->cap=a->len;a->data=realloc(a->data,a->cap*4);} return 0; }\n");
    fprintf(out, "static void ado_isort(int *d, int lo, int hi) { for(int i=lo+1;i<=hi;i++){int v=d[i],j=i-1;while(j>=lo&&d[j]>v){d[j+1]=d[j];j--;}d[j+1]=v;} }\n");
    fprintf(out, "static int ado_sort(AdoArray a) { if(a.len<2) return 0; if(a.len<32){ado_isort(a.data,0,a.len-1); return 0;} int stk[128],top=0,lo=0,hi=a.len-1;stk[top++]=lo;stk[top++]=hi;while(top>0){hi=stk[--top];lo=stk[--top];int mid=lo+(hi-lo)/2,piv=a.data[mid];a.data[mid]=a.data[hi];a.data[hi]=piv;int i=lo-1;for(int j=lo;j<hi;j++){if(a.data[j]<=a.data[hi]){i++;int t=a.data[i];a.data[i]=a.data[j];a.data[j]=t;}}int t=a.data[i+1];a.data[i+1]=a.data[hi];a.data[hi]=t;int p=i+1;if(p-1>lo){stk[top++]=lo;stk[top++]=p-1;}if(p+1<hi){stk[top++]=p+1;stk[top++]=hi;}} return 0; }\n");
    fprintf(out, "static AdoArray ado_unique(AdoArray a) { if(a.len<=1){AdoArray r=ado_make_array((int[]){},a.len); for(int i=0;i<a.len;i++)r.data[i]=a.data[i]; r.len=a.len; return r;} ado_sort(a); AdoArray r=ado_make_array((int[]){},a.len); r.data[r.len++]=a.data[0]; for(int i=1;i<a.len;i++){if(a.data[i]!=a.data[i-1])r.data[r.len++]=a.data[i];} return r; }\n");
    fprintf(out, "static int ado_reflect(AdoArray a) { printf(\"Array(len=%%d,cap=%%d)\",a.len,a.cap); return 0; }\n");
    
    for (int i = 0; i < ast->block.count; i++) {
        AST *node = ast->block.stmts[i];
        if (node->type == AST_ENUM) {
            fprintf(out, "enum %s {", node->enum_def.enum_name);
            for (int j = 0; j < node->enum_def.variant_count; j++) {
                if (j > 0) fprintf(out, ",");
                fprintf(out, "%s=%d", node->enum_def.variants[j]->var_name, j);
            }
            fprintf(out, "};\n");
        }
    }
    
    int has_main = 0;
    
    for (int i = 0; i < ast->block.count; i++) {
        AST *node = ast->block.stmts[i];
        if (node->type == AST_FN) {
            if (strcmp(node->fn.name, "main") == 0) has_main = 1;
            reset_defers();
            int fn_id = next_fn_id();
            ado_fn_id = fn_id;
            int has_defers = count_defers_stmt(node->fn.body) > 0;
            ado_fn_has_defers = has_defers;
            fprintf(out, "int %s(", node->fn.name);
            for (int j = 0; j < node->fn.paramc; j++) {
                int is_array = param_is_array(node->fn.body, node->fn.params[j]);
                fprintf(out, "%s %s", is_array ? "AdoArray" : "int", node->fn.params[j]);
                if (j < node->fn.paramc - 1) fprintf(out, ",");
            }
            fprintf(out, "){\n");
            if (has_defers) {
                fprintf(out, "  int _ado_has_return_%d=0;\n", fn_id);
                fprintf(out, "  int _ado_return_%d=0;\n", fn_id);
            }
            gen_block(node->fn.body, out, 1);
            if (has_defers) {
                fprintf(out, "  goto _ado_cleanup_%d;\n", fn_id);
                fprintf(out, "_ado_cleanup_%d:\n", fn_id);
                for (int d = ado_defer_count - 1; d >= 0; d--) {
                    gen_stmt_as_body(ado_defers[d], out, 1);
                }
                fprintf(out, "  if(_ado_has_return_%d)return _ado_return_%d;\n", fn_id, fn_id);
                fprintf(out, "  return 0;\n");
            } else if (strcmp(node->fn.name, "main") != 0) {
                fprintf(out, "  return 0;\n");
            }
            fprintf(out, "}\n\n");
        }
    }
    
    // Generate wrapper main if no main function exists
    if (!has_main) {
        fprintf(out, "int main(void) {\n");
        fprintf(out, "  return 0;\n");
        fprintf(out, "}\n");
    }
}
