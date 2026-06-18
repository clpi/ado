# mdBook Syntax Highlighting for `.do`

The Ado book uses mdBook for HTML generation and registers Ado as a custom Highlight.js language. This makes fenced code blocks work with either `ado` or `do` as the language tag.

```ado
fn main() {
  let values = [1, 2, 3]
  print("sum", sum(values))
  return 0
}
```

```do
let arr = [10, 20, 30]
print(arr?1)
print(arr[1..3])
```

## Book configuration

`docs/book.toml` enables the HTML renderer and loads the custom JavaScript file:

```toml
[book]
title = "Ado Documentation"
authors = ["Chris Pecunies"]
language = "en"
src = "src"

[output.html]
additional-js = ["theme/ado-highlight.js"]
```

The JavaScript file is stored at:

```text
docs/theme/ado-highlight.js
```

mdBook copies it into the generated book output and includes it with the HTML pages.

## Highlighted constructs

The custom Highlight.js grammar highlights:

- Keywords: `fn`, `let`, `if`, `else`, `unless`, `while`, `until`, `for`, `in`, `forever`, `return`, `break`, `continue`, `match`, `enum`, `defer`, `guard`, `assert`, `swap`, `and`, `or`, `not`.
- Literals: `true`, `false`, decimal integers, hexadecimal integers, binary integers, and strings.
- Built-ins: `print`, `len`, `push`, math helpers, array helpers, I/O helpers, HTTP helpers, and runtime helpers.
- Operators and punctuation: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `=`, `..`, `...`, `=>`, `|>`, `?`, and `:`.
- Comments starting with `#`.
- Function declarations and calls.
- Variable names in `let` statements and destructuring-shaped syntax.

## Adding Ado code to documentation

Use `ado` for new examples when possible:

````markdown
```ado
fn main() {
  print("hello")
  return 0
}
```
````

Use `do` only when documenting the file extension alias or legacy examples:

````markdown
```do
print("same highlighting")
```
````

## Regenerating or changing the highlighter

Edit `docs/theme/ado-highlight.js`, then rebuild:

```bash
mdbook build docs
```

The script is self-contained and does not require a build step. It registers the grammar at page load and calls Highlight.js again so existing code blocks are highlighted even when mdBook's default highlighter runs first.
