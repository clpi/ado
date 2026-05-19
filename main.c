#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

char *read_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

int run_process(const char *program, char *const argv[], int quiet_stderr) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        if (quiet_stderr) {
            if (freopen("/dev/null", "w", stderr) == NULL) _exit(1);
        }
        execvp(program, argv);
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) return -1;
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

int compile_and_run(const char *src_path, const char *bin_path, int quiet_compile_stderr) {
    char *compile_argv[] = {"cc", "-O2", "-o", (char *)bin_path, (char *)src_path, NULL};
    int compile_ret = run_process("cc", compile_argv, quiet_compile_stderr);
    if (compile_ret != 0) return compile_ret;

    char *run_argv[] = {(char *)bin_path, NULL};
    return run_process(bin_path, run_argv, 0);
}

void repl(void) {
    char line[4096];
    char buffer[65536];
    size_t buffer_len = 0;
    buffer[0] = '\0';
    int paren_depth = 0;
    int brace_depth = 0;
    
    printf("Ado REPL v1.0\n");
    printf("Type 'quit' to exit, 'help' for commands\n\n");
    
    while (1) {
        printf(paren_depth > 0 || brace_depth > 0 ? "... " : ">>> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        if (strncmp(line, "quit", 4) == 0) break;
        if (strncmp(line, "help", 4) == 0) {
            printf("Commands:\n");
            printf("  quit  - Exit REPL\n");
            printf("  clear - Clear buffer\n");
            printf("  run   - Execute current code\n\n");
            printf("Language features:\n");
            printf("  Functions: fn name(params) { body }\n");
            printf("  Variables: let x = value\n");
            printf("  Control: if condition { } else { }, while condition { }, for i in start..end { }\n");
            printf("  I/O: print(expr1, expr2, ...)\n");
            printf("  Operators: + - * / %% == != < > <= >= and or not\n");
            printf("  Arrays: [1, 2, 3], arr[index]\n");
            printf("  Strings: \"hello\\nworld\"\n\n");
            continue;
        }
        if (strncmp(line, "clear", 5) == 0) {
            buffer[0] = '\0';
            buffer_len = 0;
            paren_depth = 0;
            brace_depth = 0;
            continue;
        }
        
        size_t line_len = 0;
        for (int i = 0; line[i]; i++) {
            if (line[i] == '(') paren_depth++;
            if (line[i] == ')') paren_depth--;
            if (line[i] == '{') brace_depth++;
            if (line[i] == '}') brace_depth--;
            line_len++;
        }
        
        if (buffer_len + line_len < sizeof(buffer)) {
            memcpy(buffer + buffer_len, line, line_len + 1);
            buffer_len += line_len;
        } else {
            printf("Error: Input buffer overflow\n");
            buffer[0] = '\0';
            buffer_len = 0;
            paren_depth = 0;
            brace_depth = 0;
            continue;
        }
        
        if (paren_depth <= 0 && brace_depth <= 0) {
            char *src = strdup(buffer);
            
            Lexer *lex = lexer_new(src);
            Parser *p = parser_new(lex);
            AST *ast = parse_program(p);
            
            char template[] = "/tmp/ado_XXXXXX";
            char *tmp_dir = mkdtemp(template);
            if (!tmp_dir) {
                perror("mkdtemp");
                ast_free(ast);
                free(p);
                lexer_free(lex);
                free(src);
                continue;
            }
            char src_path[256];
            char bin_path[256];
            snprintf(src_path, sizeof(src_path), "%s/out.c", tmp_dir);
            snprintf(bin_path, sizeof(bin_path), "%s/out", tmp_dir);

            FILE *out = fopen(src_path, "w");
            if (!out) {
                perror("fopen");
                rmdir(tmp_dir);
                ast_free(ast);
                free(p);
                lexer_free(lex);
                free(src);
                continue;
            }
            codegen(ast, out);
            fclose(out);
            
            int ret = compile_and_run(src_path, bin_path, 1);
            if (ret != 0) {
                printf("Error compiling or running code\n");
            }
            
            ast_free(ast);
            free(p);
            lexer_free(lex);
            free(src);
            
            remove(src_path);
            remove(bin_path);
            rmdir(tmp_dir);
            buffer[0] = '\0';
            buffer_len = 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        repl();
        return 0;
    }
    
    if (strcmp(argv[1], "-i") == 0 && argc >= 3) {
        // Interactive with file as template
        char *src = read_file(argv[2]);
        if (!src) {
            fprintf(stderr, "Error: Cannot read file %s\n", argv[2]);
            return 1;
        }
        // Execute file then start REPL
        Lexer *lex = lexer_new(src);
        Parser *p = parser_new(lex);
        AST *ast = parse_program(p);
        
        char template[] = "/tmp/ado_XXXXXX";
        char *tmp_dir = mkdtemp(template);
        if (!tmp_dir) {
            perror("mkdtemp");
            ast_free(ast);
            free(p);
            lexer_free(lex);
            free(src);
            return 1;
        }
        char src_path[256];
        char bin_path[256];
        snprintf(src_path, sizeof(src_path), "%s/out.c", tmp_dir);
        snprintf(bin_path, sizeof(bin_path), "%s/out", tmp_dir);

        FILE *out = fopen(src_path, "w");
        if (!out) {
            perror("fopen");
            rmdir(tmp_dir);
            ast_free(ast);
            free(p);
            lexer_free(lex);
            free(src);
            return 1;
        }
        codegen(ast, out);
        fclose(out);
        
        int ret = compile_and_run(src_path, bin_path, 0);
        (void)ret;
        
        ast_free(ast);
        free(p);
        lexer_free(lex);
        free(src);
        
        remove(src_path);
        remove(bin_path);
        rmdir(tmp_dir);

        repl();
        return 0;
    }
    
    char *src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "Error: Cannot read file %s\n", argv[1]);
        return 1;
    }
    
    Lexer *lex = lexer_new(src);
    Parser *p = parser_new(lex);
    AST *ast = parse_program(p);
    
    char template[] = "/tmp/ado_XXXXXX";
    char *tmp_dir = mkdtemp(template);
    if (!tmp_dir) {
        perror("mkdtemp");
        ast_free(ast);
        free(p);
        lexer_free(lex);
        free(src);
        return 1;
    }
    char src_path[256];
    char bin_path[256];
    snprintf(src_path, sizeof(src_path), "%s/out.c", tmp_dir);
    snprintf(bin_path, sizeof(bin_path), "%s/out", tmp_dir);

    FILE *out = fopen(src_path, "w");
    if (!out) {
        perror("fopen");
        rmdir(tmp_dir);
        ast_free(ast);
        free(p);
        lexer_free(lex);
        free(src);
        return 1;
    }
    codegen(ast, out);
    fclose(out);
    
    int ret = compile_and_run(src_path, bin_path, 1);
    if (ret != 0) {
        // Try again with verbose error output to help diagnose compilation issues
        ret = compile_and_run(src_path, bin_path, 0);
    }
    
    ast_free(ast);
    free(p);
    lexer_free(lex);
    free(src);
    
    remove(src_path);
    remove(bin_path);
    rmdir(tmp_dir);

    return ret;
}
