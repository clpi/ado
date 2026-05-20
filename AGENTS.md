# Agent Guidelines for Ado

This repository contains the Ado programming language compiler, language server, and associated documentation. When working in this repository, please adhere to the following guidelines:

## Core Project Layout

- `*.c`, `*.h`: The core compiler logic (Lexer, Parser, Codegen) written in C.
- `main.c`: The compiler CLI entrypoint (`doc`).
- `test_main.c`: The entrypoint for C unit testing.
- `lsp/do-lsp.py`: The Language Server Protocol (LSP) implementation in Python.
- `docs/`: Comprehensive markdown documentation.
- `examples/`: Sample Ado code (`.do` files).
- `flake.nix`: Nix deployment flake.
- `HomebrewFormula/ado.rb`: Homebrew formula (auto-updated by CI).

## Testing Rules

Whenever making code changes to either the compiler or the LSP, you **must** verify the changes by running tests.

### Compiler Tests
- Run `make test` to execute both the language example tests (via `test.sh`) and the C unit tests (via `test_main`).
- **C Unit Test Workaround:** When modifying or writing new C unit tests that include files with a `main` function (like `main.c`), use the following preprocessor macro prior to `#include`ing the source file to prevent "multiple definition" errors:
  ```c
  #define main ado_main
  #include "main.c"
  #undef main
  ```

### LSP Tests
- The Language Server uses `pytest`. Run tests via:
  ```bash
  pytest lsp/test_lsp.py
  ```
  or alternatively:
  ```bash
  python3 lsp/test_lsp.py
  ```

## Development Workflow
- When building manually, use `./build.sh` or `make`.
- The compiler binary is generated as `doc`.
- Do **not** commit generated artifacts like the `doc` binary, `.o` files, or test binaries.

By following these instructions, we ensure the high quality and performance expected from the Ado programming language ecosystem.
