# Ado

Ado is a minimal programming language that transpiles `.do` source files to C and runs the generated binary. It is designed for small, fast programs, algorithmic examples, compiler experimentation, and language-tooling prototypes.

## Quick start

```bash
make
./doc examples/features.do
```

Install the compiler and language server:

```bash
sudo make install
ado examples/features.do
ado-lsp
```

Open an Ado file with the bundled Neovim launcher:

```bash
./do-edit examples/features.do
```

## What Ado provides

- Integer-first values, arrays, booleans, and limited string literals.
- Block syntax with `fn`, `let`, `if`, `unless`, `while`, `until`, `for`, `forever`, `match`, `guard`, `defer`, `break`, and `continue`.
- Array indexing, safe indexing, slicing, dynamic `push`, and `len`.
- Inline math, array, I/O, HTTP, time, random, and memory helpers.
- A Python LSP server with completion, diagnostics, navigation, rename, formatting, semantic tokens, inlay hints, code lenses, folding ranges, document highlights, and call hierarchy.
- Tree-sitter grammar, Neovim, Vim, and Helix editor support.
- WebAssembly target output as `.wat`.

## Documentation map

| Page | Purpose |
|------|---------|
| [Language Reference](./reference.md) | Complete current language syntax, semantics, built-ins, limitations, and compiler targets. |
| [Tutorial](./tutorial.md) | First program, variables, arrays, functions, control flow, and REPL notes. |
| [Examples](./examples.md) | Algorithms and feature demonstrations. |
| [LSP Features and Usage](./lsp.md) | Language server capabilities, startup commands, diagnostics, and limitations. |
| [Editor Integration](./editor-integration.md) | Neovim, Vim, Helix, Tree-sitter, and manual editor setup. |
| [mdBook Syntax Highlighting](./syntax-highlighting.md) | How the book highlights `.do` and `ado` code fences. |
| [Roadmap](./roadmap.md) | Planned language, tooling, packaging, and ecosystem work. |
