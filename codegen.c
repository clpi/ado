#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static void gen_expr(AST *ast, FILE *out);
static void gen_stmt(AST *ast, FILE *out, int indent);

static int ado_temp_counter = 0;
static int ado_defer_counter = 0;
static int next_temp(void) { return ado_temp_counter++; }
static int next_defer(void) { return ado_defer_counter++; }

static void gen_indent(FILE *out, int indent);
static void gen_stmt_as_body(AST *ast, FILE *out, int indent);

static void gen_defer_cleanup(AST *ast, FILE *out, int indent, int id) {
    gen_indent(out, indent);
    fprintf(out, "void _ado_defer_%d(void) {\n", id);
    gen_stmt_as_body(ast, out, indent + 1);
    gen_indent(out, indent);
    fprintf(out, "}\n");
    gen_indent(out, indent);
    fprintf(out, "void _ado_defer_cleanup_%d(void *unused) { _ado_defer_%d(); }\n", id, id);
    gen_indent(out, indent);
    fprintf(out, "char _ado_defer_%d __attribute__((cleanup(_ado_defer_cleanup_%d))) = 0;\n", id, id);
}

static void gen_stmt_as_body(AST *ast, FILE *out, int indent) {
    if (!ast) return;
    switch (ast->type) {
        case AST_PRINT:
        case AST_ASSIGN:
        case AST_PUSH:
        case AST_ASSERT:
        case AST_SWAP:
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
            fprintf(out, "(!");
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
            else if (strcmp(name, "fib") == 0) name = "ado_fib";
            else if (strcmp(name, "sum") == 0) name = "ado_sum";
            else if (strcmp(name, "avg") == 0) name = "ado_avg";
            else if (strcmp(name, "contains") == 0) name = "ado_contains";
            else if (strcmp(name, "count_if") == 0) name = "ado_count_if";
            else if (strcmp(name, "find") == 0) name = "ado_find";
            else if (strcmp(name, "all") == 0) name = "ado_all";
            else if (strcmp(name, "any") == 0) name = "ado_any";
            else if (strcmp(name, "pop") == 0) name = "ado_pop";
            else if (strcmp(name, "reverse") == 0) name = "ado_reverse";
            else if (strcmp(name, "remove") == 0) name = "ado_remove";
            else if (strcmp(name, "insert") == 0) name = "ado_insert";
            else if (strcmp(name, "take") == 0) name = "ado_take";
            else if (strcmp(name, "drop") == 0) name = "ado_drop";
            else if (strcmp(name, "concat") == 0) name = "ado_concat";
            else if (strcmp(name, "fill") == 0) name = "ado_fill";
            else if (strcmp(name, "filter") == 0) name = "ado_filter";
            else if (strcmp(name, "sort") == 0) name = "ado_sort";
            else if (strcmp(name, "unique") == 0) name = "ado_unique";
            else if (strcmp(name, "reflect") == 0) name = "ado_reflect";
            else if (strcmp(name, "capacity") == 0) name = "ado_capacity";
            else if (strcmp(name, "reserve") == 0) name = "ado_reserve";
            else if (strcmp(name, "shrink_to_fit") == 0) name = "ado_shrink_to_fit";
            else if (strcmp(name, "http_get") == 0) name = "ado_http_get";
            else if (strcmp(name, "http_post") == 0) name = "ado_http_post";
            else if (strcmp(name, "http_put") == 0) name = "ado_http_put";
            else if (strcmp(name, "http_delete") == 0) name = "ado_http_delete";
            else if (strcmp(name, "http_status") == 0) name = "ado_http_status";
            else if (strcmp(name, "getenv") == 0) name = "ado_getenv";
            else if (strcmp(name, "exit") == 0) name = "ado_exit";
            else if (strcmp(name, "read_file") == 0) name = "ado_read_file";
            else if (strcmp(name, "write_file") == 0) name = "ado_write_file";
            else if (strcmp(name, "file_exists") == 0) name = "ado_file_exists";
            else if (strcmp(name, "sleep") == 0) name = "ado_sleep";
            else if (strcmp(name, "time") == 0) name = "ado_time";
            else if (strcmp(name, "random") == 0) name = "ado_random";
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
        default:
            break;
    }
}

static void gen_stmt(AST *ast, FILE *out, int indent);

static void gen_block(AST *ast, FILE *out, int indent) {
    for (int i = 0; i < ast->block.count; i++) {
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
            fprintf(out, ";%s++){\n", ast->for_stmt.var);
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
            fprintf(out, "return ");
            gen_expr(ast->ret.val, out);
            fprintf(out, ";\n");
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
        case AST_DEFER: {
            int id = next_defer();
            gen_defer_cleanup(ast->defer_stmt.expr, out, indent, id);
            break;
        }
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
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n");
    // Runtime library (inline)
    fprintf(out, "typedef struct { int *data; int len; int cap; } AdoArray;\n");
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
    fprintf(out, "static int ado_reverse(AdoArray *a) { for(int i=0;i<a->len/2;i++){int t=a->data[i];a->data[i]=a->data[a->len-1-i];a->data[a->len-1-i]=t;} return 0; }\n");
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
    fprintf(out, "static void ado_qsort(int *d, int lo, int hi) { if(lo>=hi) return; int p=d[hi],i=lo-1,j,t; for(j=lo;j<hi;j++){if(d[j]<=p){i++;t=d[i];d[i]=d[j];d[j]=t;}} t=d[i+1];d[i+1]=d[hi];d[hi]=t; ado_qsort(d,lo,i); ado_qsort(d,i+2,hi); }\n");
    fprintf(out, "static int ado_sort(AdoArray a) { if(a.len>1) ado_qsort(a.data,0,a.len-1); return 0; }\n");
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
            fprintf(out, "int %s(", node->fn.name);
            for (int j = 0; j < node->fn.paramc; j++) {
                fprintf(out, "int %s", node->fn.params[j]);
                if (j < node->fn.paramc - 1) fprintf(out, ",");
            }
            fprintf(out, "){\n");
            gen_block(node->fn.body, out, 1);
            if (strcmp(node->fn.name, "main") != 0) {
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
