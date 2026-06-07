# Vim/Neovim Syntax Highlighting for Ado

## Installation

### Manual Installation

Copy the syntax files to your Vim runtime directory:

```bash
mkdir -p ~/.vim/syntax ~/.vim/ftplugin ~/.vim/ftdetect
cp vim/syntax/ado.vim ~/.vim/syntax/
cp vim/ftplugin/ado.vim ~/.vim/ftplugin/
cp vim/ftdetect/ado.vim ~/.vim/ftdetect/
```

Add to your `.vimrc` or `init.vim`:

```vim
au BufRead,BufNewFile *.do set filetype=ado
```

### For Neovim

```bash
mkdir -p ~/.config/nvim/syntax ~/.config/nvim/ftplugin ~/.config/nvim/ftdetect
cp vim/syntax/ado.vim ~/.config/nvim/syntax/
cp vim/ftplugin/ado.vim ~/.config/nvim/ftplugin/
cp vim/ftdetect/ado.vim ~/.config/nvim/ftdetect/
```

Add to your `init.lua`:

```lua
vim.filetype.add({
  extension = {
    do = 'ado',
  },
})
```

### With LSP Support (Recommended)

For full IDE features including:
- Go to definition
- Find references
- Hover documentation
- Auto-completion
- Code formatting
- Renaming symbols

Use the provided `./do-edit` launcher which includes Tree-sitter and LSP:

```bash
./do-edit myfile.do
```

Or configure manually in your `init.lua` - see `nvim/init.lua` for an example.

## Features

### Syntax Highlighting
- **Keywords**: `fn`, `let`, `if`, `else`, `return`, `for`, `while`, `in`
- **Builtins**: `print`, `len`, `push`, `abs`, `min`, `max`, `pow`, `clamp`, `sign`, `is_even`, `is_odd`, `gcd`, `lcm`, `factorial`, `fib`, `sum`, `avg`, `take`, `drop`, `concat`, `fill`
- **Operators**: Arithmetic (`+`, `-`, `*`, `/`, `%`), Comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`), Logical (`and`, `or`, `not`)
- **Special Operators**: Slice `..`, Spread `...`
- **Numbers and Strings**
- **Comments** (with `#`)

### Code Features
- Auto-indentation (2 spaces)
- File type detection for `.do` files
- Code snippets for list comprehensions and slices
- LSP integration for IDE features

### Supported Language Features

| Feature | Syntax | Example |
|---------|--------|---------|
| Array Slicing | `arr[start..end]` | `arr[2..5]` |
| List Comprehension | `[for i in start..end expr]` | `[for i in 1..10 i * i]` |
| Destructuring | `let [a, b, ...rest] = arr` | `let [x, y, ...z] = arr` |

## File Structure

```
vim/
├── ftdetect/ado.vim  # File type detection
├── ftplugin/ado.vim  # File type plugin with settings
└── syntax/ado.vim    # Syntax highlighting rules
```

## Troubleshooting

If syntax highlighting doesn't work:
1. Ensure the files are in the correct Vim directories
2. Check `:set filetype?` shows `ado`
3. Try `:syntax on` in Vim

If LSP doesn't start:
1. Check Python 3 is installed
2. Verify `lsp/ado_lsp.py` exists and is executable
3. Check `:LspInfo` in Neovim