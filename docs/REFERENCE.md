# Ado Language Reference

This document serves as a comprehensive reference manual for the Ado programming language.

## Lexical Structure

### Comments
Comments begin with a `#` character and extend to the end of the line.

```ado
# This is a comment
let x = 10  # This is also a comment
```

### Keywords
The following words are reserved keywords and cannot be used as identifiers:
`fn`, `let`, `if`, `else`, `while`, `return`, `true`, `false`, `and`, `or`, `not`

### Identifiers
Identifiers must start with a letter and can contain letters, numbers, and underscores.

## Data Types

Currently, Ado supports the following basic types:
- **Integers**: Represented as standard base-10 numbers (e.g., `42`, `-10`).
- **Booleans**: Represented by the keywords `true` and `false`.
- **Strings**: Used currently only for `print()` statements (e.g., `"Hello"`).

## Variables

Variables are introduced using the `let` keyword and must be initialized upon declaration.

```ado
let variable_name = value
```

Variables can be reassigned later using a standard assignment expression.

```ado
variable_name = new_value
```

## Operators

### Arithmetic Operators
- `+` : Addition
- `-` : Subtraction
- `*` : Multiplication
- `/` : Division
- `%` : Modulo

### Comparison Operators
- `==` : Equal to
- `!=` : Not equal to
- `<` : Less than
- `>` : Greater than
- `<=` : Less than or equal to
- `>=` : Greater than or equal to

### Logical Operators
- `and` : Logical AND
- `or` : Logical OR
- `not` : Logical NOT

## Control Flow

### If / Else
Ado uses `if` and `else` for conditional branching. Braces `{}` are required for blocks.

```ado
if condition {
  # code executed if condition is true
} else {
  # code executed if condition is false
}
```

### While Loops
Ado uses `while` loops for repeating blocks of code.

```ado
while condition {
  # code executed as long as condition is true
}
```

## Functions

Functions are defined using the `fn` keyword. Functions can take zero or more parameters.

```ado
fn function_name(param1, param2) {
  # function body
  return result
}
```

Functions return values using the `return` keyword. A function may also execute a `return` without a value, or reach the end of its block, returning implicitly.

The program entry point is standardly the `main` function.

### Built-in Functions
- `print(expr1, expr2, ...)`: Prints expressions to the standard output.

## Compilation Process

Ado is a compiled language. The compilation process consists of several phases:

1. **Lexer**: The source file (`.do`) is tokenized into a stream of tokens (keywords, identifiers, literals, symbols).
2. **Parser**: The token stream is parsed into an Abstract Syntax Tree (AST).
3. **Code Generator (Codegen)**: The AST is traversed and transpiled into C code.
4. **C Compiler**: The generated C code is compiled by an external C compiler (like `gcc` or `clang`) with `-O2` optimization into a native executable binary.

This pipeline ensures that Ado has very little runtime overhead, achieving near-native C performance.

## IDE Support
Ado comes with full IDE support through the Language Server Protocol (LSP). See the [LSP documentation](../lsp/README.md) for editor setup instructions.
