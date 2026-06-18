# Tooling

Ado tooling is split between compiler output, the language server, editor integrations, and documentation generation.

- [LSP Features and Usage](./lsp.md) covers `ado-lsp` capabilities, diagnostics, completion, navigation, formatting, rename, semantic tokens, inlay hints, folding ranges, and call hierarchy.
- [Editor Integration](./editor-integration.md) covers Neovim, Vim, Helix, Tree-sitter, and manual editor setup.
- [mdBook Syntax Highlighting](./syntax-highlighting.md) explains how the documentation book highlights `.do` and `ado` code fences.

The language server is installed by `sudo make install` as `ado-lsp`. The compiler is installed as `ado`.
