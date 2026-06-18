# LSP Features and Usage

Ado ships a Python Language Server Protocol implementation at `lsp/ado_lsp.py`. The installed binary is `ado-lsp`.

## Start the server

From a checkout:

```bash
python3 lsp/ado_lsp.py
```

After installation:

```bash
ado-lsp
```

The server communicates over stdin/stdout using the standard LSP JSON-RPC framing.

## Capabilities

`ado-lsp` advertises full text synchronization and the following capabilities:

| Capability | LSP method | What Ado provides |
|------------|------------|-------------------|
| Completion | `textDocument/completion` | Keywords, built-ins, snippets, functions, variables, parameters, enums, and enum members. |
| Go to definition | `textDocument/definition` | Jumps to parsed function, variable, parameter, enum, or enum member definitions. |
| Go to type definition | `textDocument/typeDefinition` | Uses the same definition lookup as `textDocument/definition`. |
| References | `textDocument/references` | Finds symbol references in the current file for local symbols or across open documents for functions. |
| Hover | `textDocument/hover` | Shows built-in documentation, function signatures, parameter lists, doc comments, and simple variable information. |
| Signature help | `textDocument/signatureHelp` | Shows function parameters and the active parameter based on commas. |
| Formatting | `textDocument/formatting` | Indent-aware full-document formatting. |
| Range formatting | `textDocument/rangeFormatting` | Uses the same formatter path as full-document formatting. |
| Rename | `textDocument/prepareRename`, `textDocument/rename` | Renames user-defined symbols by applying reference edits. |
| Document symbols | `textDocument/documentSymbol` | Lists functions, variables, parameters, enums, and enum members in the current document. |
| Workspace symbols | `workspace/symbol` | Fuzzy-free substring search across parsed open documents. |
| Code actions | `textDocument/codeAction` | Provides quick fixes for diagnostics such as missing `let` assignments. |
| Code lenses | `textDocument/codeLens` | Shows function signatures and enum labels. Command execution depends on the editor. |
| Semantic tokens | `textDocument/semanticTokens/full` | Provides keyword, function, variable, parameter, operator, string, number, and comment token data. |
| Inlay hints | `textDocument/inlayHint` | Shows parameter-name hints for known function calls. |
| Document highlight | `textDocument/documentHighlight` | Highlights references to the symbol under the cursor. |
| Folding ranges | `textDocument/foldingRange` | Folds brace-delimited regions and consecutive comment blocks. |
| Call hierarchy | `textDocument/prepareCallHierarchy`, `callHierarchy/incomingCalls`, `callHierarchy/outgoingCalls` | Shows callers and callees for parsed functions. |
| Diagnostics | `textDocument/publishDiagnostics` | Publishes brace, parenthesis, `let`, `match`, `guard`, undefined function, and undefined variable diagnostics. |

## Diagnostics

The LSP publishes diagnostics on document open and change.

Current diagnostics include:

- Unexpected closing brace.
- Unexpected closing parenthesis.
- `let` statement without assignment.
- `match` expression without brace-delimited arms.
- `guard` statement without `else`.
- Undefined function calls.
- Undefined variable references.
- Unmatched opening brace.
- Unclosed parenthesis.

Example:

```ado
fn main() {
  let missing
  foo()
}
```

Produces diagnostics for the missing assignment and undefined `foo`.

## Completion

Completions include:

- Keywords such as `fn`, `let`, `if`, `unless`, `while`, `forever`, `for`, `return`, `break`, `continue`, `assert`, `swap`, `match`, `enum`, `defer`, `guard`, and `until`.
- Built-ins such as `print`, `len`, `push`, `abs`, `min`, `max`, `pow`, `clamp`, `gcd`, `lcm`, `factorial`, `fib`, `sum`, `avg`, `sort`, `unique`, `reflect`, `http_get`, `getenv`, `read_file`, `sleep`, `time`, and `random`.
- Snippets for `slice`, `listcomp`, destructuring, safe indexing, `swap`, `unless`, `forever`, `assert`, `match`, `enum`, `defer`, `guard`, and `until`.
- Parsed functions, variables, parameters, enums, and enum members from open documents.

## Hover documentation

Function hovers include the signature and preceding comment lines when available.

```ado
# Adds two integers.
fn add(a, b) {
  return a + b
}

fn main() {
  add(1, 2)
}
```

Hovering `add` shows:

```ado
fn add(a, b)

Adds two integers.
```

Built-ins have fixed hover text, for example `print(values...) - Print values to stdout`.

## Rename behavior

Rename is available for user-defined symbols. Keywords and built-ins are excluded.

Local variables and parameters are renamed within their parsed scope. Functions are renamed across open documents because function resolution is global in the current LSP session.

## Symbol indexing model

The server maintains an in-memory index for open documents:

- `didOpen` parses symbols and publishes diagnostics.
- `didChange` replaces the document text, reparses symbols, and publishes diagnostics.
- References are found by masked text search so strings and comments are ignored.
- Function symbols are resolved globally across open documents.
- Variables and parameters are resolved by parsed scope.

The index is not a persistent project database.

## Limitations

`ado-lsp` is intentionally lightweight and heuristic-based.

Current limitations:

- Uses full-document synchronization only.
- Indexes open documents, not an entire workspace file tree.
- Does not run the compiler for diagnostics.
- Does not perform type checking.
- Does not support incremental text changes.
- Symbol resolution for variables and parameters is scope-based but not a full semantic model.
- Some snippets and hover entries describe syntax or built-ins that are recognized by tooling but not yet implemented by the compiler.

## Testing

Run the LSP tests from the repository root:

```bash
pytest lsp/test_lsp.py
```

Or run the unittest module directly:

```bash
python3 lsp/test_lsp.py
```
