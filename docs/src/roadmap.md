# Roadmap

Ado is a small language with a compiler, runtime helpers, LSP, Tree-sitter grammar, and editor integration already in place. The roadmap focuses on turning the current prototype into a predictable, documented, and easy-to-use systems language.

## Near term

### Documentation and discoverability

- Keep the mdBook reference synchronized with implemented compiler features.
- Document which syntax is compiler-supported, which is tooling-only, and which is planned.
- Add examples for standard-library helpers, HTTP helpers, safe indexing, slicing, match, guard, defer, and WASM output.
- Keep editor setup instructions current for Neovim, Vim, and Helix.

### Compiler correctness

- Add regression tests for every syntax form in the reference.
- Clarify ownership and lifetime behavior for arrays returned from functions.
- Improve error messages from the lexer, parser, and generated C compilation step.
- Keep generated C readable enough for debugging while preserving `-O1` performance.

### Tooling polish

- Add project-wide file indexing to the LSP.
- Add incremental text synchronization.
- Add compiler-backed diagnostics where practical.
- Make rename safer for local variables, parameters, functions, enums, and enum members.
- Add editor-neutral LSP test fixtures for completion, hover, definition, references, formatting, semantic tokens, inlay hints, folding, and call hierarchy.

## Language evolution

### Strings

Strings should become a real value type with assignment, indexing or slicing, concatenation, length, and safe formatting.

Likely work:

- Runtime string representation.
- String literals as values, not only print fragments.
- Basic string helpers such as length, concat, slice, and comparison.
- Updated LSP hover and diagnostics for string expressions.

### Floating-point numbers

Add floating-point literals and arithmetic without breaking the integer-first design.

Likely work:

- Lexer support for decimal floats.
- Separate integer and float runtime operations.
- Optional type hints or inference rules for numeric literals.
- Updated `print` formatting.

### Modules and imports

Add a small module system for larger programs.

Likely work:

- `import path` or similar syntax.
- Module search rules.
- Public/private symbol boundaries.
- LSP workspace indexing across imported files.

### Type system improvements

Ado currently accepts type hints but does not enforce or use them for code generation.

Likely work:

- Define a small set of built-in types.
- Make `:` and `::` semantics explicit.
- Add diagnostics for required type mismatches.
- Preserve fast C generation and avoid heavyweight runtime overhead.

### Structs and records

Add user-defined aggregate types.

Likely work:

- `struct` declaration syntax.
- Field access and assignment.
- Initialization helpers.
- LSP document symbols for fields and types.

### Collections beyond integer arrays

Add at least one general collection type before dictionaries.

Likely work:

- Generic or typed array syntax.
- Bounds-checked access policy.
- Clear mutation and ownership rules.
- Runtime helpers for common collection operations.

## Ecosystem

### Package manager

A minimal package manager should make sharing Ado code easy without becoming a large build system.

Likely work:

- Package manifest format.
- Local dependency resolution.
- Version pinning.
- Integration with imports.
- Documentation examples.

### Standard library

Expand the inline runtime into a documented standard library.

Likely areas:

- Math.
- Arrays and collections.
- Strings.
- File I/O.
- Environment and process helpers.
- HTTP client utilities.
- Testing helpers.

### Editor extensions

The LSP already provides the core editor experience. Future work can package it for common editors.

Likely work:

- Official VS Code extension.
- Helix language pack.
- Vim plugin with filetype, syntax, LSP config, and snippets.
- Neovim plugin using the bundled Tree-sitter grammar and LSP config.

### Debugging

Add debugging support for real development workflows.

Likely work:

- Debug-friendly generated C.
- Source maps or stable line mapping.
- GDB/LLDB examples.
- Optional DAP adapter.

## Performance and targets

### Optimization profile

Ado currently compiles generated C with `-O1`. The roadmap should preserve the existing benchmark profile while adding correctness and debugging features.

Likely work:

- Keep benchmark scripts in the repository.
- Track integer, recursive, and array-heavy workloads.
- Add optional compiler flags for optimization level when safe.

### WebAssembly

The compiler already has a `--target wasm` path that emits WAT.

Likely work:

- Document supported WASI imports.
- Add WAT tests.
- Add examples that run under `wasmtime`.
- Decide whether the default target remains native C or can be selected per project.

### Packaging

Continue publishing binaries and packages through CI.

Likely work:

- Homebrew formula updates.
- Nix flake updates.
- Debian, RPM, APK, and Arch packages.
- GitHub Pages documentation deployment from `main`.

## Success criteria

A release is ready when:

- The language reference matches implemented syntax.
- `make test` passes on supported platforms.
- LSP tests cover the advertised capabilities.
- The mdBook builds cleanly and deploys from `main`.
- At least one editor integration works out of the box.
- Examples compile and run with the installed `ado` binary.
