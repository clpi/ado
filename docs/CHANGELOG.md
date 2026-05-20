# Changelog

All notable changes to the Ado language will be documented in this file.

## [v0.1.0] - Initial Release

This is the initial release of the Ado programming language.

### Added
- **Core Language Features**:
  - Variable declarations using `let`.
  - Support for Integer and Boolean data types.
  - Functions with parameters and recursion (`fn`).
  - Control flow: `if/else` conditions and `while` loops.
  - Operators: Arithmetic (`+`, `-`, `*`, `/`, `%`), Comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`), and Logical (`and`, `or`, `not`).
  - Native `print()` built-in function.

- **Compiler**:
  - Lexer, Parser, and Code Generator that transpiles Ado code into C.
  - Integration with standard C compilers using `-O2` optimizations for native execution speeds.
  - Interactive REPL interface.

- **Tooling & IDE Support**:
  - Full Language Server Protocol (LSP) implementation via `lsp/do-lsp.py`.
  - Features include: Intelligent Code Completion, Go to Definition, Find References, Hover Information, and Real-time Diagnostics.
  - Built-in Neovim support via the `./ado-edit` script.
  - Tree-sitter grammar support.
  - Vim syntax highlighting files.

- **Documentation**:
  - Added comprehensive Markdown documentation including Tutorial, Language Reference, Examples breakdown, Use Cases, Roadmap, and this Changelog.
