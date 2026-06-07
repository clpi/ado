#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_INT, AST_STR, AST_BOOL, AST_VAR, AST_BINOP, AST_CALL, AST_LET,
    AST_IF, AST_WHILE, AST_FOR, AST_RETURN, AST_BLOCK, AST_FN,
    AST_ARRAY, AST_INDEX, AST_ASSIGN, AST_PRINT, AST_LEN, AST_PUSH,
    AST_UNARY, AST_TYPE, AST_HINT, AST_MATCH, AST_ENUM, AST_PIPE,
    AST_SLICE, AST_LISTCOMP, AST_DESTRUCT
} ASTType;

typedef struct {
    char *name;
    int is_optional;
} TypeHint;

typedef struct AST {
    ASTType type;
    TypeHint *hint;
    union {
        int int_val;
        char *str_val;
        int bool_val;
        char *var_name;
        struct { struct AST *left, *right; TokenType op; } binop;
        struct { struct AST *operand; TokenType op; } unary;
        struct { char *name; struct AST **args; int argc; TypeHint **hints; } call;
        struct { char *name; struct AST *val; TypeHint *type_hint; } let;
        struct { struct AST *cond, *then, *els; } if_stmt;
        struct { struct AST *cond, *body; } while_stmt;
        struct { char *var; struct AST *start, *end, *body; } for_stmt;
        struct { struct AST *val; } ret;
        struct { struct AST **stmts; int count; } block;
        struct { char *name; char **params; int paramc; TypeHint **param_hints; struct AST *body; TypeHint *return_hint; } fn;
        struct { struct AST **elems; int count; } array;
        struct { struct AST *arr; struct AST *idx; } index;
        struct { struct AST *target; struct AST *val; } assign;
        struct { struct AST **vals; int count; } print;
struct { struct AST *arr; } len;
        struct { struct AST *arr; struct AST *val; } push;
        struct { char *name; struct AST *args; } hint_stmt;
        struct { char *enum_name; struct AST **variants; int variant_count; } enum_def;
        struct { struct AST *expr; struct AST **arms; int arm_count; } match_stmt;
        struct { struct AST *arr; struct AST *start; struct AST *end; } slice;
        struct { char *var; struct AST *start; struct AST *end; struct AST *body; struct AST *filter; } listcomp;
        struct { char **names; int count; int has_rest; char *rest_name; struct AST *val; } destruct;
    };
} AST;

typedef struct {
    Lexer *lex;
    Token cur;
    struct AST *pool;
    int pool_count;
    int pool_cap;
} Parser;

Parser *parser_new(Lexer *lex);
AST *parse_program(Parser *p);
void ast_free(AST *ast);
void parser_free(Parser *p);

#endif
