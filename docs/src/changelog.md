# Changelog

All notable changes to the Ado language will be documented in this file.

## [Unreleased]

### Added
- Comprehensive markdown documentation inside the `docs/` folder, including `tutorial.md`, `examples.md`, `reference.md`, `use_cases.md`, and `roadmap.md`.
- **LSP rename**: `do-lsp` renamed to `ado-lsp` for consistent naming.
- **Metaprogramming compiler hinting**: `@` prefix for compile-time hints (`@inline`, `@memo`).
- **Optional typing**: Type annotations on variables (`let x::Type = 5` required, `let x:Type = 5` optional).
- **Pattern matching**: `match` expression for algebraic data types.
- **Enum types**: `enum` for sum types with variants.
- **Pipe-forward operator**: `value |> fn` for left-to-right function composition.
- **Rare useful features**: destructuring, list comprehensions, expression blocks, guarded match expressions, match-as-expression, and integer `try`/`rescue` exception handling.
- **WASM compilation**: Enhanced `--target wasm` support with proper WASI exports.

## [0.1.0] - Initial Release

### Added
- **Compiler**: Basic compiler (`doc`) transpiling `.do` files to C code, and automatically executing via native C compilers with `-O2`.
- **REPL**: Interactive Read-Eval-Print Loop for testing small expressions and statements.
- **LSP Integration**: Python-based Language Server (`ado-lsp`) supporting code completion, diagnostics, formatting, and definition jumping.
- **Tree-sitter Grammar**: Custom Tree-sitter grammar definition for syntax highlighting.
- **Neovim setup**: Utility scripts (`setup-dev.sh`, `ado-edit`) for quick NeoVim IDE setup.
- **Core language features**:
  - `fn` for function declaration.
  - `let` for variable binding.
  - Standard arithmetic (`+`, `-`, `*`, `/`, `%`) and comparison (`==`, `<`, `>`, `<=`, `>=`, `!=`) operators.
  - Conditional `if` / `else` blocks, `while` loops, and `for` loops.
  - Internal `print` function for I/O operations.

## [0.2.0] - Planned

### Added
- Standard library implementation.
- Basic module system for importing code.
