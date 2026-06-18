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

static inline unsigned kw_load2(const char *s) {
    return ((unsigned char)s[0]) | ((unsigned char)s[1]<<8);
}
static inline unsigned kw_load3(const char *s) {
    return ((unsigned char)s[0]) | ((unsigned char)s[1]<<8) | ((unsigned char)s[2]<<16);
}
static inline unsigned kw_load4(const char *s) {
    return ((unsigned char)s[0]) | ((unsigned char)s[1]<<8) | ((unsigned char)s[2]<<16) | ((unsigned char)s[3]<<24);
}

static TokenType kw_lookup(const char *s, int len) {
    switch (len) {
        case 2: {
            unsigned h = kw_load2(s);
            if (h == 0x6e66) return TOK_FN;      // "fn"
            if (h == 0x6669) return TOK_IF;      // "if"
            if (h == 0x6e69) return TOK_IN;      // "in"
            if (h == 0x726f) return TOK_OR;      // "or"
            break;
        }
        case 3: {
            unsigned h = kw_load3(s);
            if (h == 0x74656c) return TOK_LET;   // "let"
            if (h == 0x726f66) return TOK_FOR;   // "for"
            if (h == 0x746f6e) return TOK_NOT;   // "not"
            if (h == 0x646e61) return TOK_AND;   // "and"
            if (h == 0x6e656c) return TOK_LEN;   // "len"
            break;
        }
        case 4: {
            unsigned h = kw_load4(s);
            if (h == 0x65736c65) return TOK_ELSE;  // "else"
            if (h == 0x65757274) return TOK_TRUE;  // "true"
            if (h == 0x68737570) return TOK_PUSH;  // "push"
            if (h == 0x746e6968) return TOK_HINT;  // "hint"
            if (h == 0x6d756e65) return TOK_ENUM;  // "enum"
            if (h == 0x70617773) return TOK_SWAP;  // "swap"
            break;
        }
        case 5: {
            unsigned h = kw_load4(s);
            if (h == 0x6c696877 && s[4]=='e') return TOK_WHILE;   // "while"
            if (h == 0x736c6166 && s[4]=='e') return TOK_FALSE;   // "false"
            if (h == 0x6e697270 && s[4]=='t') return TOK_PRINT;   // "print"
            if (h == 0x61657262 && s[4]=='k') return TOK_BREAK;   // "break"
            if (h == 0x65707974 && s[4]=='s') return TOK_TYPE;    // "types"
            if (h == 0x65666564 && s[4]=='r') return TOK_DEFER;   // "defer"
            if (h == 0x72617567 && s[4]=='d') return TOK_GUARD;   // "guard"
            if (h == 0x69746e75 && s[4]=='l') return TOK_UNTIL;   // "until"
            if (h == 0x6374616d && s[4]=='h') return TOK_MATCH;   // "match"
            break;
        }
        case 6: {
            unsigned h = kw_load4(s);
            if (h == 0x75746572 && s[4]=='r' && s[5]=='n') return TOK_RETURN;  // "return"
            if (h == 0x65737361 && s[4]=='r' && s[5]=='t') return TOK_ASSERT;  // "assert"
            if (h == 0x656c6e75 && s[4]=='s' && s[5]=='s') return TOK_UNLESS;  // "unless"
            break;
        }
        case 7: {
            unsigned h = kw_load4(s);
            if (h == 0x65726f66 && s[4]=='v' && s[5]=='e' && s[6]=='r') return TOK_FOREVER; // "forever"
            break;
        }
        case 8: {
            unsigned h = kw_load4(s);
            if (h == 0x746e6f63 && s[4]=='i' && s[5]=='n' && s[6]=='u' && s[7]=='e') return TOK_CONTINUE; // "continue"
            break;
        }
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
    
    if (c == '"') {
        lex->pos++;
        int start = lex->pos;
        while (lex->src[lex->pos] != '"' && lex->src[lex->pos] != '\0') {
            if (lex->src[lex->pos] == '\\') lex->pos++;
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
            if (lex->src[lex->pos] == '>') { lex->pos++; }
            tok.type = TOK_PIPE;
            break;
        case '?': tok.type = TOK_QMARK; break;
        case '+': tok.type = TOK_PLUS; break;
        case '-':
            if (lex->src[lex->pos] == '>') { lex->pos++; tok.type = TOK_ARROW; }
            else tok.type = TOK_MINUS;
            break;
        case '~': tok.type = TOK_TILDE; break;
        case '*': tok.type = TOK_STAR; break;
        case '/': tok.type = TOK_SLASH; break;
        case '%': tok.type = TOK_PERCENT; break;
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