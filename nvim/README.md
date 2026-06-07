# DO Language Editor Setup (Neovim)

## Quick Start

Launch Neovim with full DO language support:

```bash
./do-edit example.do
```

This automatically loads:
- Syntax highlighting
- LSP server (completion, go-to-definition, diagnostics)
- Tree-sitter parser (advanced syntax highlighting)

## Features

### LSP Keybindings

- `gd` - Go to definition
- `gD` - Go to type definition
- `gr` - Find references
- `K` - Show hover information
- `<C-k>` - Signature help
- `<leader>rn` - Rename symbol
- `<leader>f` - Format document
- `<leader>ca` - Code actions
- `<leader>o` - Document symbols
- `<leader>s` - Workspace symbols
- `[d` / `]d` - Next/previous diagnostic

### Tree-sitter

Advanced syntax highlighting and code understanding:
- Semantic highlighting with proper token types
- Better indentation
- Code folding support
- Inlay hints for function parameters

### New Language Features Support

| Feature | Syntax | Highlighting | Completion |
|---------|--------|--------------|------------|
| Array Slicing | `arr[2..5]` | `..` operator | Snippet `slice` |
| List Comprehension | `[for i in 1..10 i * i]` | `for` keyword | Snippet `listcomp` |
| Destructuring | `let [a, b, ...rest] = arr` | Variable names | Snippet `destruct` |

## Prerequisites

### Required
- Neovim 0.8+
- Python 3

### Optional (for Tree-sitter)
- nvim-treesitter plugin
- tree-sitter CLI: `npm install -g tree-sitter-cli`

## Building Tree-sitter Parser

```bash
cd tree-sitter-do
./build.sh
```

## Manual Setup

If you prefer to use your own Neovim config, copy the files:

```bash
mkdir -p ~/.config/nvim/syntax ~/.config/nvim/ftplugin
cp vim/syntax/ado.vim ~/.config/nvim/syntax/
cp vim/ftplugin/ado.vim ~/.config/nvim/ftplugin/

# Add to init.lua
vim.filetype.add({ extension = { do = 'ado' } })
```

For Tree-sitter support, add to your plugin manager config:

```lua
require('nvim-treesitter.parsers').get_parser_configs().ado = {
  install_info = { url = '/path/to/ado/tree-sitter-do', files = {'src/parser.c'} },
  filetype = 'ado',
}
```

## Supported Platforms

- **Neovim**: Full LSP + Tree-sitter support
- **Vim**: Basic syntax highlighting
- **Helix**: Full support via `helix/` directory