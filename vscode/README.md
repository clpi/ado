# Ado Language Support for VS Code

## Features

- **Syntax Highlighting**: Full support for Ado language keywords, operators, and literals
- **Snippets**: Quick snippets for common Ado patterns including slices, list comprehensions, and destructuring
- **LSP Ready**: Works with the built-in LSP client from the project's `lsp/ado_lsp.py`

## Installation

### From Source

1. Copy the `vscode` directory to your VS Code extensions folder:

```bash
# On macOS
cp -r vscode ~/.vscode/extensions/ado-lang

# On Linux
cp -r vscode ~/.vscode/extensions/ado-lang

# On Windows
xcopy /E vscode %USERPROFILE%\.vscode\extensions\ado-lang
```

### Using VS Code Extension Development

1. Open the project in VS Code
2. Press `F5` to launch a new window with the extension loaded
3. Open any `.do` file to test syntax highlighting

## Features Supported

| Feature | Syntax | Snippet Prefix |
|---------|--------|---------------|
| Array Slice | `arr[2..5]` | `slice` |
| List Comprehension | `[for i in 1..10 i*i]` | `listcomp` |
| List Comprehension with Filter | `[for i in 1..10 i*i if i > 5]` | `listcompf` |
| Destructuring | `let [a, b, ...rest] = arr` | `destruct` |

## LSP Setup

To use LSP features:

1. Install the Ado LSP:
```bash
chmod +x lsp/ado_lsp.py
```

2. Add to VS Code settings (`settings.json`):
```json
{
  "languageserver.ado-lsp": {
    "command": "python3",
    "args": ["/path/to/ado/lsp/ado_lsp.py"],
    "filetypes": ["ado"]
  }
}
```

## Syntax Highlighting

The extension provides highlighting for:

- Keywords: `fn`, `let`, `if`, `else`, `for`, `while`, `return`, `in`
- Builtins: `print`, `len`, `push`, `abs`, `min`, `max`, `pow`, etc.
- Operators: `..` (slice), `...` (spread), arithmetic and comparison ops
- Comments: `#` style comments
- Strings: Double-quoted strings

## Example

```ado
let numbers = [1, 2, 3, 4, 5]
let [first, second, ...rest] = numbers
print("first:", first)

# Array slicing
let slice = numbers[1..3]

# List comprehension - squares
let squares = [for i in 1..10 i * i]
print("squares:", len(squares))
```

## Development

To modify the extension:

1. Edit `vscode/syntaxes/ado.tmLanguage.json` for syntax highlighting rules
2. Edit `vscode/snippets/ado.json` for code snippets
3. Package with `vsce package` if you have the VS Code Extension CLI installed