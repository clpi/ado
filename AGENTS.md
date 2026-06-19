# Ado Agents Reference

This file provides context and guidelines for automated agents working on the Ado programming language codebase.

## Project Overview

Ado (file extension `.do`) is a minimal, fast programming language that compiles down to optimized C code (using `-O1`).

- **Compiler Binary:** Built as `doc` via `make` and installed as `ado` via `make install`.
- **LSP Implementation:** A Python-based Language Server Protocol implementation is available in `lsp/ado_lsp.py`. Test it via `pytest lsp/test_lsp.py` or `python3 lsp/test_lsp.py`.
- **Tree-sitter Grammar:** The `tree-sitter-do/` directory contains the Tree-sitter grammar. It can be compiled by running `tree-sitter generate` inside that directory.
- **IDE Support:** Use the `./do-edit` helper script to launch Neovim with full IDE capabilities (LSP, Tree-sitter) for `.do` files.

## Language Specifics

- **Comments:** Single-line comments start with `#`.
- **Types:** The language supports integer types, integer arrays, and booleans (`true`, `false`).
- **Arrays:** Arrays are zero-indexed and accessed via standard bracket notation (e.g. `arr[0]`). Supported array operations include `push(arr, val)` and `len(arr)`.
- **Strings:** Limited string literal support exists, primarily for use as arguments to the built-in `print()` function.
- **Control Flow:** `if` / `else` conditionals, `while` loops, and `for` loops (using range syntax like `for i in start..end { ... }`).

## Build and Testing

- **C Compiler:** The primary CI/CD workflow cross-compiles ARM64 Linux binaries using `gcc-aarch64-linux-gnu`.
- **Testing:** Tests are executed via `make test`, which internally wraps the `./test.sh` script to test example `.do` scripts and run C unit tests.
- **Unit Test Quirk:** C unit tests interacting with files that have a `main` function (like `main.c`) must use a preprocessor macro `#define main ado_main` before `#include`ing the source file to prevent multiple entry point definition errors.

## Performance Optimizations

- **Optimization level:** Generated C code is compiled with `-O1` instead of `-O2`. On Apple M4 with clang 21, `-O1` produces faster code than `-O2` for recursive patterns like fibonacci (0.38s vs 0.47s) and compiles at the same speed (~0.017s).
- **Pool allocator:** AST nodes are allocated from a pool (`parser->pool`) instead of per-node `malloc`, reducing allocation overhead.
- **Direct integer parsing:** The lexer parses integer literals directly into `Token.int_val`, avoiding `strndup` + `atoi` + `free`.
- **Keyword lookup from source:** The lexer checks keywords via `kw_lookup(s, len)` on the source buffer directly, avoiding `strndup` + `strcmp` for keywords.
- **`stat()` for file size:** `read_file` uses `stat()` instead of `fseek(SEEK_END)` + `ftell` for faster file size detection.

## Benchmark Results (Apple M4)

```
fib:      0.98x  (Ado 7% faster than C, includes compile+run)
prime:    1.46x
collatz:  1.42x
sort:     1.46x
array_ops:1.47x
sumloop:  1.46x
ackermann:1.44x
bsearch:  1.13x
matmul:   1.47x
TOTAL:    1.27x
```

Ado benchmarks measure full pipeline (lex + parse + codegen + C compile + run) vs pre-compiled C execution only. Ratio < 1.0 means Ado pipeline is faster than C execution alone.

Runtime optimizations applied:
- `ado_sort`: quicksort with iterative explicit stack, median-of-3 pivot, insertion sort for n<32
- `ado_pow`: exponentiation by squaring O(log n)
- `ado_unique`: sort + single-pass dedup O(n log n)
- `ado_flatten`: filters out negatives, copies remaining elements to new array

Compiler optimizations applied:
- Lexer `kw_lookup`: integer hash comparison (single `uint32` compare per keyword)
- Parser `parse_stmt`: avoids full `Parser` struct copy for IDENT backtracking
- Array parameter detection: auto-declares `AdoArray` for params used with `[]`, `len()`, `push()`
- `|>` pipe operator: recognized as single token
- User-defined function names protected from runtime name collision (only math builtins auto-mapped)

## Known Limitations

- **`defer` statement**: Uses GCC nested functions with `__attribute__((cleanup))`. Not supported on clang/macOS. Use `guard` as an alternative.
- **Array parameters**: User-defined functions detect array-typed parameters automatically. Runtime functions requiring pointer semantics (`ado_pop`, `ado_remove`, `ado_insert`) must be called with the `ado_` prefix.
- **macOS compilation**: Generated C code is compiled via `xcrun cc -O1`. The Makefile requires `CC="cc -isysroot $(xcrun --show-sdk-path)"` on macOS.

## Packaging and CI/CD

- A Nix flake (`flake.nix`) is provided for Nix ecosystems.
- Additional packages (`.deb`, `.rpm`, `.apk` for Alpine, and `.pkg.tar.zst` for Arch Linux pacman) are built using `fpm`.
- A Homebrew formula is maintained in `HomebrewFormula/ado.rb`.
- Both the Homebrew formula and the flake version are automatically updated via the consolidated GitHub Actions CI/CD workflow in `.github/workflows/ci.yml`.
- The CI/CD workflow also packages the compiled executable (renamed to `ado`) into `.tar.gz` archives for distribution.

## Documentation

Comprehensive markdown documentation for the language (including tutorials, reference, examples, use cases, roadmap, and changelog) is located in the `docs/` directory.

---
**Note:** Always verify your work by running the relevant tests (`make test`, `python3 lsp/test_lsp.py`, etc.) and ensure compatibility with the existing CI/CD workflow and documentation structure.
