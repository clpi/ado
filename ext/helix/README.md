# Helix Editor Support for Ado

## Installation

To use Ado language support in Helix, add this repository as a custom language:

1. In your Helix configuration (`~/.config/helix/languages.toml`), add:

```toml
[[language]]
name = "ado"
scope = "source.ado"

[[grammar]]
name = "ado"
source = { path = "/path/to/ado/tree-sitter-do" }
```

2. Copy the queries to Helix's runtime:

```bash
cp -r helix/queries/* ~/.config/helix/runtime/queries/ado/
```

3. Restart Helix or run `:logreload` to load the new configuration.

## Features

- **Syntax Highlighting**: Full highlighting for keywords, operators, strings, comments, and the new features (slices `..`, spreads `...`, list comprehensions)
- **Indentation**: Smart indentation for `if`, `while`, `for`, and function blocks
- **Text Objects**: Navigate code structures with `[[` and `]]` for functions, `[]` for arrays
- **Local Variables**: Proper variable tracking for rename and find-references

## Key Bindings

| Key | Action |
|-----|--------|
| `gd` | Go to definition |
| `gr` | Find references |
| `K` | Hover documentation |
| `<C-k>` | Signature help |
| `<leader>rn` | Rename symbol |
| `<leader>f` | Format document |
| `<leader>ca` | Code action |
| `[d` / `]d` | Next/previous diagnostic |

## LSP Configuration

The LSP server is configured to use `ado_lsp.py` from the repository. Make sure you have:

1. Python 3 installed
2. The LSP script is executable: `chmod +x lsp/ado_lsp.py`
3. Tree-sitter parser compiled: `./tree-sitter-do/build.sh`

## Development

To rebuild the tree-sitter parser after grammar changes:

```bash
./tree-sitter-do/build.sh
```

To regenerate Helix queries from tree-sitter highlights:

```bash
tree-sitter highlight --query /path/to/ado/tree-sitter-do/queries/highlights.scm
```