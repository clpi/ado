# Editor Integration

Ado editor support is built from three pieces:

1. The `ado-lsp` language server.
2. The Tree-sitter grammar in `ext/tree-sitter-do`.
3. Editor-specific runtime files in `ext/nvim`, `ext/vim`, and `ext/helix`.

## Build and install first

Build the compiler and language server:

```bash
make
sudo make install
```

This installs:

- `ado` from the `doc` compiler binary.
- `ado-lsp` from `lsp/ado_lsp.py`.

Verify:

```bash
ado --help
ado-lsp
```

`ado-lsp` stays running as an LSP server, so use it through an editor or stop it with your editor's LSP shutdown command.

## Neovim

The bundled launcher is:

```bash
./do-edit file.do
```

It starts Neovim with `ext/nvim/init.lua`.

The Neovim config:

- Treats `.do` files as filetype `ado`.
- Starts `ado-lsp` from `PATH` when installed, otherwise falls back to `lsp/ado_lsp.py`.
- Enables Tree-sitter parser configuration for `ado` when `nvim-treesitter` is available.
- Enables completion snippets when `nvim-cmp` is available.
- Maps common LSP commands.

### Neovim key bindings

| Key | Action |
|-----|--------|
| `gd` | Go to definition |
| `gD` | Go to type definition |
| `gr` | Find references |
| `gi` | Go to implementation |
| `K` | Hover |
| `<C-k>` | Signature help |
| `<leader>rn` | Rename symbol |
| `<leader>f` | Format document |
| `<leader>ca` | Code actions |
| `<leader>cl` | Run code lens command |
| `[d` | Previous diagnostic |
| `]d` | Next diagnostic |
| `<leader>q` | Put diagnostics in location list |
| `<leader>e` | Open diagnostic float |
| `<leader>o` | Document symbols |
| `<leader>s` | Workspace symbols |

### Manual Neovim setup

If you do not want to use `./do-edit`, configure the LSP in your own Neovim config.

For Neovim 0.11+:

```lua
vim.lsp.config('ado-lsp', {
  cmd = { 'ado-lsp' },
  root_markers = { '.git' },
  filetypes = { 'ado' },
})
vim.lsp.enable('ado-lsp')
```

For older Neovim versions:

```lua
vim.api.nvim_create_autocmd('FileType', {
  pattern = 'ado',
  callback = function()
    vim.lsp.start({
      name = 'ado-lsp',
      cmd = { 'ado-lsp' },
      root_dir = vim.fs.dirname(vim.fs.find('.git', { upward = true })[1]),
    })
  end,
})
```

If `ado-lsp` is not installed, point `cmd` at the repository script:

```lua
cmd = { 'python3', '/path/to/ado/lsp/ado_lsp.py' }
```

### Tree-sitter in Neovim

Build the grammar:

```bash
cd ext/tree-sitter-do
npm install
npm run build
```

Then register it with `nvim-treesitter` or use the bundled `ext/nvim/init.lua` as a starting point.

## Vim

Vim syntax and filetype files are in `ext/vim`.

Install them by adding `ext/vim` to Vim's runtime path:

```vim
set runtimepath^=/path/to/ado/ext/vim
filetype plugin indent on
syntax enable
```

The Vim files provide:

- `*.do` filetype detection.
- Keyword, builtin, operator, string, number, comment, and function-call highlighting.
- Local comment and indent settings.
- Small insert abbreviations for `fn` and `let`.

Vim syntax highlighting is separate from the LSP. Use an LSP client such as `vim-lsp`, `coc.nvim`, or Neovim's built-in LSP for navigation, completion, and diagnostics.

## Helix

Helix support files are in `ext/helix`.

- `config.toml` defines the Ado language entry.
- `queries/highlights.scm` provides Tree-sitter highlight queries.
- `queries/indents.scm` provides indentation queries.
- `queries/text-objects.scm` provides text-object queries.
- `queries/locals.scm` provides local-variable queries.

A minimal `~/.config/helix/languages.toml` entry can point at the local grammar:

```toml
[[language]]
name = "ado"
scope = "source.ado"
file-types = ["do"]
comment-token = "#"
indent = { tab-width = 2, unit = "  " }

[[grammar]]
name = "ado"
source = { path = "/path/to/ado/ext/tree-sitter-do" }
```

Copy the queries into Helix runtime if you are not loading them directly from the repository:

```bash
mkdir -p ~/.config/helix/runtime/queries/ado
cp ext/helix/queries/* ~/.config/helix/runtime/queries/ado/
```

Helix can use `ado-lsp` through its standard LSP configuration. Add the server command to your Helix LSP config if needed:

```toml
[language-server.ado-lsp]
cmd = ["ado-lsp"]
```

## Tree-sitter grammar

The grammar lives in `ext/tree-sitter-do`.

Build and test it:

```bash
cd ext/tree-sitter-do
npm install
npm run build
npm test
```

The grammar covers the core syntax used by the compiler and editor integrations, including functions, `let`, control flow, calls, arrays, strings, numbers, slices, and list-comprehension-shaped syntax for tooling.

## Manual editor configuration checklist

For any editor:

1. Associate `*.do` with filetype `ado`.
2. Start `ado-lsp` as the LSP server.
3. Enable LSP completion, hover, definition, references, formatting, rename, diagnostics, and semantic tokens if supported.
4. Add the Tree-sitter grammar for syntax highlighting and text objects.
5. Use the highlighter aliases `ado` and `do` for `.do` source.
