# Ado Language Quick Reference

## File Extension
`.do`

## Basic Syntax

### Functions
```
fn name(param1, param2) {
  return param1 + param2
}
```

### Variables
```
let x = 10
let result = add(5, 3)
```

### Control Flow
```
if x > 0 {
  return x
} else {
  return 0
}
```

### Comments
```
# Single line comment
```

## Rare but Useful Features

### Safe Indexing
```
arr?idx
arr?idx:fallback
```

### Negative Branching
```
unless condition {
  # runs when condition is false
} else {
  # runs when condition is true
}
```

### Swap
```
swap a, b
```

### Assertions and Infinite Loops
```
assert n > 0
forever {
  if done {
    break
  }
  if skip {
    continue
  }
}
```

### Destructuring and List Comprehensions
```
let [a, b, ...rest] = values
let squares = [for n in 0..5 n * n]
let evens = [for n in 0..10 where n % 2 == 0 n]
```

### Expression Blocks, Match Guards, and Try/Rescue
```
let scoped = {
  let x = 1
  x + 1
}

match status {
  ok when ready => 1,
  _ => 0
}

try {
  raise 42
} rescue err {
  print(err)
}
```

## Operators

### Arithmetic
`+` `-` `*` `/`

### Comparison
`==` `!=` `<` `>` `<=` `>=`

## Types
- Integers: `42`, `-10`
- Arrays: `[1, 2, 3]`

## Built-in Functions
- `print(...)`: Prints values to the console
- `len(arr)`: Returns the length of an array
- `push(arr, val)`: Pushes a value to the end of an array

## Editor Commands

### Compile and Run
```bash
./doc file.do
```

### Edit with IDE Support
```bash
./do-edit file.do
```

### REPL
```bash
./doc
```

## LSP Keybindings (in Neovim)

| Key | Action |
|-----|--------|
| `gd` | Go to definition |
| `gr` | Find references |
| `K` | Hover information |
| `<leader>rn` | Rename symbol |
| `<leader>f` | Format document |

## Example Program

```
fn factorial(n) {
  if n <= 1 {
    return 1
  }
  return n * factorial(n - 1)
}

fn main() {
  let result = factorial(5)
  return result
}
```

## Common Patterns

### Recursion
```
fn fib(n) {
  if n <= 1 {
    return n
  }
  return fib(n - 1) + fib(n - 2)
}
```

### Conditional Expression
```
fn max(a, b) {
  if a > b {
    return a
  }
  return b
}
```

### Multiple Parameters
```
fn power(base, exp) {
  if exp == 0 {
    return 1
  }
  return base * power(base, exp - 1)
}
```

## Compilation Process

1. **Lexer** - Tokenizes source code
2. **Parser** - Builds Abstract Syntax Tree (AST)
3. **Code Generator** - Transpiles to C
4. **C Compiler** - Compiles to native binary with `-O2`

## Performance Tips

- Ado compiles to optimized C code
- Recursion is efficient (tail call optimization depends on C compiler)
- Integer arithmetic is fast (native CPU operations)

## Limitations

- Only integer and integer array types currently
- No strings as full values, maps, structs, modules, or static type checker

## Getting Help

- See `README.md` for full documentation
- See `SETUP.md` for complete setup guide
- See `lsp/FEATURES.md` for LSP capabilities
- Check `examples/` for code samples
