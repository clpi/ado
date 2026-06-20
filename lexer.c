#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

Lexer *lexer_new(char *src) {
    Lexer *lex = malloc(sizeof(Lexer));
    lex->src = src;
    lex->pos = 0;
    lex->line = 1;
    return lex;
}

void lexer_free(Lexer *lex) { free(lex); }

static void skip_ws(Lexer *lex) {
    while (lex->src[lex->pos] == ' ' || lex->src[lex->pos] == '\t' || 
           lex->src[lex->pos] == '\n' || lex->src[lex->pos] == '\r') {
        if (lex->src[lex->pos] == '\n') lex->line++;
        lex->pos++;
    }
}

static void skip_comment(Lexer *lex) {
    skip_ws(lex);
    while (lex->src[lex->pos] == '#') {
        while (lex->src[lex->pos] != '\n' && lex->src[lex->pos] != '\0') lex->pos++;
        if (lex->src[lex->pos] == '\n') { lex->line++; lex->pos++; }
        skip_ws(lex);
    }
}

static TokenType kw_lookup(const char *s, int len) {
    if (len == 2) {
        if (s[0] == 'f' && s[1] == 'n') return TOK_FN;
        if (s[0] == 'i' && s[1] == 'f') return TOK_IF;
        if (s[0] == 'i' && s[1] == 'n') return TOK_IN;
        if (s[0] == 'o' && s[1] == 'r') return TOK_OR;
    }
    if (len == 3) {
        if (s[0] == 'l' && s[1] == 'e' && s[2] == 't') return TOK_LET;
        if (s[0] == 'f' && s[1] == 'o' && s[2] == 'r') return TOK_FOR;
        if (s[0] == 'n' && s[1] == 'o' && s[2] == 't') return TOK_NOT;
        if (s[0] == 'a' && s[1] == 'n' && s[2] == 'd') return TOK_AND;
        if (s[0] == 'l' && s[1] == 'e' && s[2] == 'n') return TOK_LEN;
        if (s[0] == 't' && s[1] == 'r' && s[2] == 'y') return TOK_TRY;
    }
    if (len == 4) {
        if (s[0] == 'e' && s[1] == 'l' && s[2] == 's' && s[3] == 'e') return TOK_ELSE;
        if (s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e') return TOK_TRUE;
        if (s[0] == 'p' && s[1] == 'u' && s[2] == 's' && s[3] == 'h') return TOK_PUSH;
        if (s[0] == 'h' && s[1] == 'i' && s[2] == 'n' && s[3] == 't') return TOK_HINT;
        if (s[0] == 'e' && s[1] == 'n' && s[2] == 'u' && s[3] == 'm') return TOK_ENUM;
        if (s[0] == 's' && s[1] == 'w' && s[2] == 'a' && s[3] == 'p') return TOK_SWAP;
        if (s[0] == 's' && s[1] == 't' && s[2] == 'e' && s[3] == 'p') return TOK_STEP;
        if (s[0] == 'w' && s[1] == 'h' && s[2] == 'e' && s[3] == 'n') return TOK_WHEN;
        if (s[0] == 'o' && s[1] == 'n' && s[2] == 'c' && s[3] == 'e') return TOK_ONCE;
    }
    if (len == 5) {
        if (memcmp(s, "while", 5) == 0) return TOK_WHILE;
        if (memcmp(s, "false", 5) == 0) return TOK_FALSE;
        if (memcmp(s, "print", 5) == 0) return TOK_PRINT;
        if (memcmp(s, "break", 5) == 0) return TOK_BREAK;
        if (memcmp(s, "types", 5) == 0) return TOK_TYPE;
        if (memcmp(s, "defer", 5) == 0) return TOK_DEFER;
        if (memcmp(s, "guard", 5) == 0) return TOK_GUARD;
        if (memcmp(s, "until", 5) == 0) return TOK_UNTIL;
        if (memcmp(s, "match", 5) == 0) return TOK_MATCH;
        if (memcmp(s, "where", 5) == 0) return TOK_WHERE;
        if (memcmp(s, "raise", 5) == 0) return TOK_RAISE;
        if (memcmp(s, "maybe", 5) == 0) return TOK_MAYBE;
        if (memcmp(s, "after", 5) == 0) return TOK_AFTER;
    }
    if (len == 6) {
        if (memcmp(s, "return", 6) == 0) return TOK_RETURN;
        if (memcmp(s, "assert", 6) == 0) return TOK_ASSERT;
        if (memcmp(s, "unless", 6) == 0) return TOK_UNLESS;
        if (memcmp(s, "rescue", 6) == 0) return TOK_RESCUE;
        if (memcmp(s, "before", 6) == 0) return TOK_BEFORE;
        if (memcmp(s, "around", 6) == 0) return TOK_AROUND;
    }
    if (len == 7) {
        if (memcmp(s, "forever", 7) == 0) return TOK_FOREVER;
    }
    if (len == 8) {
        if (memcmp(s, "continue", 8) == 0) return TOK_CONTINUE;
    }
    if (len == 9) {
        if (memcmp(s, "invariant", 9) == 0) return TOK_INVARIANT;
    }
    return TOK_IDENT;
}

Token lexer_next(Lexer *lex) {
    skip_comment(lex);
    Token tok = {.line = lex->line, .value = NULL};
    char c = lex->src[lex->pos];
    
    if (c == '\0') { tok.type = TOK_EOF; return tok; }
    
    if (isdigit(c)) {
        int val = 0;
        if (lex->src[lex->pos] == '0' && lex->src[lex->pos+1] == 'x') {
            lex->pos += 2;
            while (isxdigit(lex->src[lex->pos])) {
                char ch = lex->src[lex->pos];
                val = val * 16 + (ch >= '0' && ch <= '9' ? ch - '0' : (ch >= 'a' && ch <= 'f' ? ch - 'a' + 10 : ch - 'A' + 10));
                lex->pos++;
            }
        } else if (lex->src[lex->pos] == '0' && lex->src[lex->pos+1] == 'b') {
            lex->pos += 2;
            while (lex->src[lex->pos] == '0' || lex->src[lex->pos] == '1') {
                val = val * 2 + (lex->src[lex->pos] - '0');
                lex->pos++;
            }
        } else {
            while (isdigit(lex->src[lex->pos])) {
                val = val * 10 + (lex->src[lex->pos] - '0');
                lex->pos++;
            }
        }
        tok.int_val = val;
        tok.type = TOK_INT;
        return tok;
    }
    
    if (c == '"' || c == '`') {
        char delim = c;
        lex->pos++;
        int start = lex->pos;
        while (lex->src[lex->pos] != delim && lex->src[lex->pos] != '\0') {
            if (delim == '"' && lex->src[lex->pos] == '\\') lex->pos++;
            if (lex->src[lex->pos] == '\n') lex->line++;
            lex->pos++;
        }
        int len = lex->pos - start;
        tok.value = strndup(lex->src + start, len);
        tok.type = TOK_STR;
        lex->pos++;
        return tok;
    }
    
    if (isalpha(c) || c == '_') {
        int start = lex->pos;
        while (isalnum(lex->src[lex->pos]) || lex->src[lex->pos] == '_') lex->pos++;
        int len = lex->pos - start;
        
        TokenType kw = kw_lookup(lex->src + start, len);
        if (kw != TOK_IDENT) {
            tok.value = NULL;
            tok.type = kw;
        } else {
            tok.value = strndup(lex->src + start, len);
            tok.type = TOK_IDENT;
        }
        return tok;
    }
    
    lex->pos++;
    switch (c) {
        case '(': tok.type = TOK_LPAREN; break;
        case ')': tok.type = TOK_RPAREN; break;
        case '{': tok.type = TOK_LBRACE; break;
        case '}': tok.type = TOK_RBRACE; break;
        case '[': tok.type = TOK_LBRACKET; break;
        case ']': tok.type = TOK_RBRACKET; break;
        case ',': tok.type = TOK_COMMA; break;
        case ':': 
            if (lex->src[lex->pos] == ':') { lex->pos++; tok.type = TOK_COLONCOLON; }
            else tok.type = TOK_COLON;
            break;
        case '@': tok.type = TOK_AT; break;
        case ';': tok.type = TOK_SEMI; break;
        case '|':
            tok.type = TOK_PIPE;
            break;
        case '?': tok.type = TOK_QMARK; break;
        case '+':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_PLUSEQ; }
            else tok.type = TOK_PLUS;
            break;
        case '-':
            if (lex->src[lex->pos] == '>') { lex->pos++; tok.type = TOK_ARROW; }
            else if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_MINUSEQ; }
            else tok.type = TOK_MINUS;
            break;
        case '~': tok.type = TOK_TILDE; break;
        case '*':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_STAREQ; }
            else tok.type = TOK_STAR;
            break;
        case '/':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_SLASHEQ; }
            else tok.type = TOK_SLASH;
            break;
        case '%':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_PERCENTEQ; }
            else tok.type = TOK_PERCENT;
            break;
        case '.':
            if (lex->src[lex->pos] == '.') {
                lex->pos++;
                if (lex->src[lex->pos] == '.') { lex->pos++; tok.type = TOK_DOTDOTDOT; }
                else tok.type = TOK_DOTDOT;
            }
            break;
        case '=':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_EQ; }
            else if (lex->src[lex->pos] == '>') { lex->pos++; tok.type = TOK_FATARROW; }
            else tok.type = TOK_ASSIGN;
            break;
        case '!':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_NE; }
            else if (lex->src[lex->pos] == '!') { lex->pos++; tok.type = TOK_CHECK; }
            else tok.type = TOK_BANG;
            break;
        case '<':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_LE; }
            else tok.type = TOK_LT;
            break;
        case '>':
            if (lex->src[lex->pos] == '=') { lex->pos++; tok.type = TOK_GE; }
            else tok.type = TOK_GT;
            break;
    }
    return tok;
}