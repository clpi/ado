#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

typedef struct {
    FILE *out;
    int label_count;
    int temp_count;
} WasmGen;

static int wg_label(WasmGen *g) {
    return g->label_count++;
}

static int wg_temp(WasmGen *g) {
    return g->temp_count++;
}

static void wg_expr(WasmGen *g, AST *ast);

static void wg_stmt(WasmGen *g, AST *ast) {
    if (!ast) return;
    switch (ast->type) {
        case AST_LET: {
            fprintf(g->out, "  (local $%s i32)\n", ast->let.name);
            fprintf(g->out, "  ");
            wg_expr(g, ast->let.val);
            fprintf(g->out, "\n  local.set $%s\n", ast->let.name);
            break;
        }
        case AST_RETURN: {
            fprintf(g->out, "  return ");
            wg_expr(g, ast->ret.val);
            fprintf(g->out, "\n");
            break;
        }
        case AST_PRINT: {
            for (int i = 0; i < ast->print.count; i++) {
                fprintf(g->out, "  ");
                wg_expr(g, ast->print.vals[i]);
                fprintf(g->out, "\n  call $print_i32\n");
            }
            break;
        }
        case AST_ASSIGN: {
            int t = wg_temp(g);
            fprintf(g->out, "  (local $tmp%d i32)\n", t);
            fprintf(g->out, "  ");
            wg_expr(g, ast->assign.target);
            fprintf(g->out, "\n  local.set $tmp%d\n", t);
            fprintf(g->out, "  ");
            wg_expr(g, ast->assign.val);
            fprintf(g->out, "\n  local.get $tmp%d\n", t);
            fprintf(g->out, "  i32.store align=4\n");
            break;
        }
        case AST_PUSH: {
            fprintf(g->out, "  ;; push start\n");
            fprintf(g->out, "  local.get $%s\n", ast->push.arr->var_name);
            fprintf(g->out, "  local.tee $push_arr\n");
            fprintf(g->out, "  i32.load align=4\n");
            fprintf(g->out, "  local.tee $push_len\n");
            fprintf(g->out, "  local.get $push_arr\n");
            fprintf(g->out, "  i32.load offset=4 align=4\n");
            fprintf(g->out, "  local.tee $push_cap\n");
            fprintf(g->out, "  local.get $push_len\n");
            fprintf(g->out, "  i32.ge_s\n");
            int grow = wg_label(g);
            int after = wg_label(g);
            fprintf(g->out, "  br_if $grow_%d\n", grow);
            // Fast path: just store
            fprintf(g->out, "  local.get $push_arr\n");
            fprintf(g->out, "  local.get $push_len\n");
            fprintf(g->out, "  i32.const 4\n  i32.mul\n  i32.add\n");
            fprintf(g->out, "  ");
            wg_expr(g, ast->push.val);
            fprintf(g->out, "\n  i32.store align=4\n");
            fprintf(g->out, "  local.get $push_arr\n");
            fprintf(g->out, "  local.get $push_len\n  i32.const 1\n  i32.add\n");
            fprintf(g->out, "  i32.store align=4\n");
            fprintf(g->out, "  br $after_%d\n", after);
            // Grow path
            fprintf(g->out, "$grow_%d\n", grow);
            fprintf(g->out, "  local.get $push_cap\n  i32.const 2\n  i32.mul\n");
            fprintf(g->out, "  local.set $new_cap\n");
            fprintf(g->out, "  local.get $new_cap\n  i32.const 4\n  i32.mul\n  call $malloc\n");
            fprintf(g->out, "  local.tee $new_arr\n");
            fprintf(g->out, "  local.get $push_arr\n");
            fprintf(g->out, "  i32.load align=4\n");  // old len
            fprintf(g->out, "  local.get $new_arr\n");
            fprintf(g->out, "  i32.const 0\n");
            fprintf(g->out, "  local.get $push_cap\n  i32.const 4\n  i32.mul\n");
            fprintf(g->out, "  call $memcpy\n  drop\n");
            fprintf(g->out, "  local.get $new_arr\n  local.get $new_cap\n  i32.store offset=4 align=4\n");
            fprintf(g->out, "  local.get $new_arr\n");
            fprintf(g->out, "  ;; store new data pointer into old array header\n");
            fprintf(g->out, "  local.get $push_arr\n  local.get $new_arr\n  i32.store align=4\n");
            fprintf(g->out, "  local.get $new_arr\n  local.get $push_len\n  i32.const 1\n  i32.add\n");
            fprintf(g->out, "  i32.const 4\n  i32.mul\n  i32.add\n");
            fprintf(g->out, "  ");
            wg_expr(g, ast->push.val);
            fprintf(g->out, "\n  i32.store align=4\n");
            fprintf(g->out, "  local.get $new_arr\n");
            fprintf(g->out, "  local.get $push_len\n  i32.const 1\n  i32.add\n");
            fprintf(g->out, "  i32.store align=4\n");
            fprintf(g->out, "$after_%d\n", after);
            fprintf(g->out, "  ;; push end\n");
            break;
        }
        case AST_IF: {
            int else_lbl = wg_label(g);
            int end_lbl = wg_label(g);
            fprintf(g->out, "  (block $end_%d (block $else_%d\n", end_lbl, else_lbl);
            fprintf(g->out, "    ");
            wg_expr(g, ast->if_stmt.cond);
            fprintf(g->out, "\n    i32.eqz\n    br_if $else_%d\n", else_lbl);
            fprintf(g->out, "    ");
            wg_stmt(g, ast->if_stmt.then);
            fprintf(g->out, "    br $end_%d\n", end_lbl);
            fprintf(g->out, "    $else_%d\n", else_lbl);
            if (ast->if_stmt.els) {
                fprintf(g->out, "    ");
                wg_stmt(g, ast->if_stmt.els);
            }
            fprintf(g->out, "    $end_%d\n", end_lbl);
            fprintf(g->out, "  )) ;; end if\n");
            break;
        }
        case AST_WHILE: {
            int loop_lbl = wg_label(g);
            int end_lbl = wg_label(g);
            fprintf(g->out, "  (block $end_%d (loop $loop_%d\n", end_lbl, loop_lbl);
            fprintf(g->out, "    ");
            wg_expr(g, ast->while_stmt.cond);
            fprintf(g->out, "\n    i32.eqz\n    br_if $end_%d\n", end_lbl);
            fprintf(g->out, "    ");
            wg_stmt(g, ast->while_stmt.body);
            fprintf(g->out, "    br $loop_%d\n", loop_lbl);
            fprintf(g->out, "    $end_%d\n", end_lbl);
            fprintf(g->out, "  )) ;; end while\n");
            break;
        }
        case AST_FOR: {
            int loop_lbl = wg_label(g);
            int end_lbl = wg_label(g);
            fprintf(g->out, "  (local $%s i32)\n", ast->for_stmt.var);
            fprintf(g->out, "  ");
            wg_expr(g, ast->for_stmt.start);
            fprintf(g->out, "\n  local.set $%s\n", ast->for_stmt.var);
            fprintf(g->out, "  (block $end_%d (loop $loop_%d\n", end_lbl, loop_lbl);
            fprintf(g->out, "    local.get $%s\n", ast->for_stmt.var);
            wg_expr(g, ast->for_stmt.end);
            fprintf(g->out, "\n    i32.ge_s\n    br_if $end_%d\n", end_lbl);
            fprintf(g->out, "    ");
            wg_stmt(g, ast->for_stmt.body);
            fprintf(g->out, "    local.get $%s\n  i32.const 1\n  i32.add\n  local.set $%s\n",
                    ast->for_stmt.var, ast->for_stmt.var);
            fprintf(g->out, "    br $loop_%d\n", loop_lbl);
            fprintf(g->out, "    $end_%d\n", end_lbl);
            fprintf(g->out, "  )) ;; end for\n");
            break;
        }
        case AST_BLOCK: {
            for (int i = 0; i < ast->block.count; i++) {
                wg_stmt(g, ast->block.stmts[i]);
            }
            break;
        }
        default: {
            fprintf(g->out, "  ;; unsupported stmt type=%d\n", ast->type);
            break;
        }
    }
}

static void wg_expr(WasmGen *g, AST *ast) {
    if (!ast) {
        fprintf(g->out, "i32.const 0");
        return;
    }
    switch (ast->type) {
        case AST_INT:
            fprintf(g->out, "i32.const %d", ast->int_val);
            break;
        case AST_BOOL:
            fprintf(g->out, "i32.const %d", ast->bool_val ? 1 : 0);
            break;
        case AST_VAR:
            fprintf(g->out, "local.get $%s", ast->var_name);
            break;
        case AST_BINOP:
            wg_expr(g, ast->binop.left);
            fprintf(g->out, "\n    ");
            wg_expr(g, ast->binop.right);
            fprintf(g->out, "\n    ");
            switch (ast->binop.op) {
                case TOK_PLUS: fprintf(g->out, "i32.add"); break;
                case TOK_MINUS: fprintf(g->out, "i32.sub"); break;
                case TOK_STAR: fprintf(g->out, "i32.mul"); break;
                case TOK_SLASH: fprintf(g->out, "i32.div_s"); break;
                case TOK_PERCENT: fprintf(g->out, "i32.rem_s"); break;
                case TOK_EQ: fprintf(g->out, "i32.eq"); break;
                case TOK_NE: fprintf(g->out, "i32.ne"); break;
                case TOK_LT: fprintf(g->out, "i32.lt_s"); break;
                case TOK_GT: fprintf(g->out, "i32.gt_s"); break;
                case TOK_LE: fprintf(g->out, "i32.le_s"); break;
                case TOK_GE: fprintf(g->out, "i32.ge_s"); break;
                case TOK_AND: fprintf(g->out, "i32.and"); break;
                case TOK_OR: fprintf(g->out, "i32.or"); break;
                default: fprintf(g->out, "i32.const 0"); break;
            }
            break;
        case AST_UNARY:
            wg_expr(g, ast->unary.operand);
            fprintf(g->out, "\n    i32.const 0\n    i32.eq\n    i32.const 1\n    i32.xor ;; not\n");
            break;
        case AST_CALL: {
            const char *name = ast->call.name;
            if (strcmp(name, "abs") == 0) name = "ado_abs";
            else if (strcmp(name, "min") == 0) name = "ado_min";
            else if (strcmp(name, "max") == 0) name = "ado_max";
            else if (strcmp(name, "pow") == 0) name = "ado_pow";
            else if (strcmp(name, "factorial") == 0) name = "ado_factorial";
            else if (strcmp(name, "fib") == 0) name = "ado_fib";
            else if (strcmp(name, "sum") == 0) name = "ado_sum";
            else if (strcmp(name, "count_if") == 0) name = "ado_count_if";
            else if (strcmp(name, "find") == 0) name = "ado_find";
            else if (strcmp(name, "all") == 0) name = "ado_all";
            else if (strcmp(name, "any") == 0) name = "ado_any";
            fprintf(g->out, "call $%s", name);
            for (int i = 0; i < ast->call.argc; i++) {
                fprintf(g->out, "\n    ");
                wg_expr(g, ast->call.args[i]);
            }
            break;
        }
        case AST_INDEX: {
            int t = wg_temp(g);
            fprintf(g->out, "  (local $tmp%d i32)\n", t);
            wg_expr(g, ast->index.arr);
            fprintf(g->out, "\n  local.set $tmp%d\n", t);
            fprintf(g->out, "  local.get $tmp%d\n", t);
            wg_expr(g, ast->index.idx);
            fprintf(g->out, "\n  i32.const 4\n  i32.mul\n  i32.add\n  i32.load align=4 ;; arr.data[idx]\n");
            break;
        }
        case AST_LEN: {
            wg_expr(g, ast->len.arr);
            fprintf(g->out, "\n  i32.load align=4 ;; arr.len\n");
            break;
        }
        case AST_ARRAY: {
            int count = ast->array.count;
            int size = (count * 4) + 4;
            int t = wg_temp(g);
            fprintf(g->out, "  (local $tmp%d i32)\n", t);
            fprintf(g->out, "  i32.const %d\n  call $malloc\n  local.set $tmp%d\n", size, t);
            // Store length at offset 0
            fprintf(g->out, "  i32.const %d\n  local.get $tmp%d\n  i32.store align=4\n",
                    count, t);
            // Store elements at offsets 4, 8, 12, ...
            for (int i = 0; i < count; i++) {
                fprintf(g->out, "  ");
                wg_expr(g, ast->array.elems[i]);
                fprintf(g->out, "\n  local.get $tmp%d\n  i32.const %d\n  i32.add\n  i32.store align=4\n",
                        t, (i + 1) * 4);
            }
            fprintf(g->out, "  local.get $tmp%d", t);
            break;
        }
        default:
            fprintf(g->out, "i32.const 0");
            break;
    }
}

void codegen_wasm(AST *ast, FILE *out) {
    WasmGen g;
    g.out = out;
    g.label_count = 0;
    g.temp_count = 0;

    // Find main function
    AST *main_fn = NULL;
    for (int i = 0; i < ast->block.count; i++) {
        if (ast->block.stmts[i]->type == AST_FN &&
            strcmp(ast->block.stmts[i]->fn.name, "main") == 0) {
            main_fn = ast->block.stmts[i];
            break;
        }
    }

    fprintf(out, "(module\n");

    // Imports for host runtime
    fprintf(out, "  (import \"env\" \"print_i32\" (func $print_i32 (param i32)))\n");
    fprintf(out, "  (import \"env\" \"memcpy\" (func $memcpy (param i32 i32 i32) (result i32)))\n");
    fprintf(out, "  (import \"env\" \"malloc\" (func $malloc (param i32) (result i32)))\n");
    fprintf(out, "  (import \"env\" \"itoa\" (func $itoa (param i32) (result i32)))\n");
    fprintf(out, "  (memory (export \"memory\") 1 256)\n");

    // Stdlib math functions (inline WAT implementations)
    fprintf(out, "  (func $ado_abs (param $x i32) (result i32)\n");
    fprintf(out, "    local.get $x\n    i32.const 0\n    i32.lt_s\n");
    fprintf(out, "    if (result i32)\n      i32.const 0\n      local.get $x\n      i32.sub\n");
    fprintf(out, "    else\n      local.get $x\n    end\n  )\n");

    fprintf(out, "  (func $ado_min (param $a i32) (param $b i32) (result i32)\n");
    fprintf(out, "    local.get $a\n    local.get $b\n    i32.gt_s\n");
    fprintf(out, "    if (result i32)\n      local.get $b\n    else\n      local.get $a\n    end\n  )\n");

    fprintf(out, "  (func $ado_max (param $a i32) (param $b i32) (result i32)\n");
    fprintf(out, "    local.get $a\n    local.get $b\n    i32.lt_s\n");
    fprintf(out, "    if (result i32)\n      local.get $b\n    else\n      local.get $a\n    end\n  )\n");

    fprintf(out, "  (func $ado_clamp (param $x i32) (param $lo i32) (param $hi i32) (result i32)\n");
    fprintf(out, "    local.get $x\n    local.get $lo\n    i32.lt_s\n");
    fprintf(out, "    if (result i32)\n      local.get $lo\n");
    fprintf(out, "    else\n      local.get $x\n      local.get $hi\n      i32.gt_s\n");
    fprintf(out, "      if (result i32)\n        local.get $hi\n      else\n        local.get $x\n      end\n    end\n  )\n");

    fprintf(out, "  (func $ado_pow (param $b i32) (param $e i32) (result i32)\n");
    fprintf(out, "    (local $r i32)\n    i32.const 1\n    local.set $r\n");
    fprintf(out, "    (block $break (loop $continue\n");
    fprintf(out, "      local.get $e\n      i32.eqz\n      br_if $break\n");
    fprintf(out, "      local.get $r\n      local.get $b\n      i32.mul\n      local.set $r\n");
    fprintf(out, "      local.get $e\n      i32.const 1\n      i32.sub\n      local.set $e\n");
    fprintf(out, "      br $continue\n");
    fprintf(out, "    ))\n");
    fprintf(out, "    local.get $r\n  )\n");

    fprintf(out, "  (func $ado_sign (param $x i32) (result i32)\n");
    fprintf(out, "    local.get $x\n    i32.const 0\n    i32.gt_s\n");
    fprintf(out, "    if (result i32)\n      i32.const 1\n");
    fprintf(out, "    else\n      local.get $x\n      i32.const 0\n      i32.lt_s\n");
    fprintf(out, "      if (result i32)\n        i32.const -1\n      else\n        i32.const 0\n      end\n    end\n  )\n");

    fprintf(out, "  (func $ado_gcd (param $a i32) (param $b i32) (result i32)\n");
    fprintf(out, "    (block $break (loop $loop\n");
    fprintf(out, "      local.get $b\n      i32.eqz\n      br_if $break\n");
    fprintf(out, "      local.get $b\n      local.set $t\n");
    fprintf(out, "      local.get $a\n      local.get $b\n      i32.rem_s\n      local.set $a\n");
    fprintf(out, "      local.get $t\n      local.set $b\n");
    fprintf(out, "      br $loop\n");
    fprintf(out, "    ))\n");
    fprintf(out, "    local.get $a\n  )\n");

    fprintf(out, "  (func $ado_fib (param $n i32) (result i32)\n");
    fprintf(out, "    (local $a i32) (local $b i32) (local $i i32)\n");
    fprintf(out, "    i32.const 0\n    local.set $a\n");
    fprintf(out, "    i32.const 1\n    local.set $b\n");
    fprintf(out, "    i32.const 0\n    local.set $i\n");
    fprintf(out, "    (block $break (loop $loop\n");
    fprintf(out, "      local.get $i\n      local.get $n\n      i32.ge_s\n      br_if $break\n");
    fprintf(out, "      local.get $b\n      local.set $t\n");
    fprintf(out, "      local.get $a\n      local.get $b\n      i32.add\n      local.set $b\n");
    fprintf(out, "      local.get $t\n      local.set $a\n");
    fprintf(out, "      local.get $i\n      i32.const 1\n      i32.add\n      local.set $i\n");
    fprintf(out, "      br $loop\n");
    fprintf(out, "    ))\n");
    fprintf(out, "    local.get $a\n  )\n");

    // Array operations
    fprintf(out, "  (func $ado_push (param $arr i32) (param $val i32)\n");
    fprintf(out, "    local.get $arr\n    i32.load align=4 ;; len\n");
    fprintf(out, "    local.tee $len\n");
    fprintf(out, "    local.get $arr\n    i32.load offset=4 align=4 ;; cap\n");
    fprintf(out, "    local.tee $cap\n");
    fprintf(out, "    local.get $len\n    i32.eqz\n");
    fprintf(out, "    if\n      i32.const 4\n      local.set $cap\n    end\n");
    fprintf(out, "    local.get $len\n    local.get $cap\n    i32.ge_s\n");
    fprintf(out, "    if (result i32)\n      local.get $cap\n      i32.const 2\n      i32.mul ;; new_cap\n");
    fprintf(out, "    else\n      local.get $cap\n    end\n");
    fprintf(out, "    local.tee $newcap\n");
    fprintf(out, "    (; grow array data ;)\n");
    fprintf(out, "    ;; simple version: just store if room, else grow\n");

    // emcc-compatible inline stdlib

    // User-defined functions (non-main)
    for (int i = 0; i < ast->block.count; i++) {
        AST *node = ast->block.stmts[i];
        if (node->type == AST_FN && strcmp(node->fn.name, "main") != 0) {
            fprintf(out, "  (func $%s (param $%s i32)", node->fn.name, node->fn.params[0]);
            for (int j = 1; j < node->fn.paramc; j++) {
                fprintf(out, " (param $%s i32", node->fn.params[j]);
            }
            fprintf(out, ") (result i32\n");
            wg_stmt(&g, node->fn.body);
            fprintf(out, "    i32.const 0\n    return\n  )\n\n");
        }
    }

    // Main function
    if (main_fn) {
        fprintf(out, "  (func $main (export \"_start\") (result i32)\n");
        wg_stmt(&g, main_fn->fn.body);
    } else {
        fprintf(out, "  (func $main (export \"_start\") (result i32)\n");
        fprintf(out, "    ;; no main function found\n");
    }
    fprintf(out, "    i32.const 0\n    return\n  )\n");
    fprintf(out, ")\n");
}
