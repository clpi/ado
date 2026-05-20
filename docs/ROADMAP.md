# Ado Roadmap

Ado is continuously evolving. Here is a high-level roadmap of features planned for future releases.

## Phase 1: Core Data Types (Near Term)
- **Strings**: First-class support for string variables, concatenation, and manipulation. Currently, strings are only supported inside `print` statements.
- **Arrays**: Fixed-size and dynamically sized arrays.
- **Floating Point Numbers**: Support for decimal arithmetic (`float` and `double` equivalents).

## Phase 2: Structured Data & Modularity (Medium Term)
- **Structs**: User-defined compound data types, similar to C structs.
- **Module System**: The ability to `import` or `include` other `.do` files to break up large codebases into reusable libraries.
- **Pointers / References**: A safe way to pass by reference for memory optimization.

## Phase 3: Standard Library (Long Term)
- **Math Library**: Built-in math functions (`sin`, `cos`, `sqrt`, etc.).
- **File I/O**: Ability to read and write files directly from Ado.
- **String Manipulation**: Standard library for string splitting, joining, and searching.

## IDE & Tooling Enhancements
- **LSP Improvements**: Semantic highlighting and code actions (quick fixes).
- **Package Manager**: A simple system to download and share Ado modules.
- **Debug Adapter Protocol (DAP)**: Integration with editor debuggers to step through Ado code line-by-line.
