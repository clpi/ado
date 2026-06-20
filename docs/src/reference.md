# Language Reference

Ado source files use the `.do` extension. The current compiler reads a `.do` file, builds an AST, emits C, compiles the C with `cc -O1`, and runs the resulting binary.

```ado
fn main() {
  let n = 10
  print("Factorial", factorial(n))
  return 0
}

fn factorial(n) {
  if n <= 1 {
    return 1
  }
  return n * factorial(n - 1)
}
```

## Source layout

Ado programs are made of top-level function declarations and statements. Statements do not use trailing semicolons.

```ado
let x = 1
print(x)

fn helper(n) {
  return n + 1
}
```

Blocks are delimited with `{` and `}`. Indentation is not syntactically significant, but two spaces are the project convention.

## Lexical syntax

### Comments

Single-line comments start with `#` and run to the end of the line.

```ado
# A standalone comment
let x = 1 # An inline comment
```

Block comments are not supported.

### Identifiers

Identifiers start with a letter or underscore and may contain letters, digits, and underscores.

```ado
let total_count = 0
let _private = 1
```

Identifiers are case-sensitive.

### Numbers

Ado accepts decimal, hexadecimal, and binary integer literals. Negative numbers are parsed as unary minus.

```ado
let decimal = 42
let hex = 0x2a
let binary = 0b101010
let negative = -10
```

All numeric operations use signed C `int` arithmetic.

### Strings

String literals support `\"`, `\\`, `\n`, and `\t` escapes. Strings are currently intended for `print` arguments and generated C format fragments rather than a full string value type.

```ado
print("hello")
print("line\nbreak")
```

### Booleans

`true` and `false` are boolean literals. They are emitted as integer `1` and `0`.

```ado
let ready = true
if ready {
  print("ready")
}
```

## Values and types

Ado is integer-first. The implemented value categories are:

| Category | Syntax | Notes |
|----------|--------|-------|
| Integers | `0`, `42`, `-3`, `0x2a`, `0b1010` | Stored and operated on as C `int`. |
| Booleans | `true`, `false` | Represented as `1` and `0`. |
| Arrays | `[1, 2, 3]`, `[]` | Dynamic integer arrays backed by the Ado runtime array struct. |
| Strings | `"text"` | Limited literal support, mainly for `print`. |

Ado does not currently have floating-point numbers, structs, dictionaries, modules, packages, first-class functions, or a static type checker.

## Variables and assignments

Variables are introduced with `let`.

```ado
let x = 10
let y = x + 5
y = 20
```

Variables are mutable bindings. The compiler emits them with C's `__auto_type`, so the generated type follows the initializer or assignment target.

### Optional type hints

Ado accepts type hints on variables, but the current code generator treats them as hints only.

```ado
let x:Int = 1
let y::Int = 2
```

`:` marks an optional hint. `::` marks a required hint. Neither changes generated C today.

### Destructuring

Destructuring binds array elements to names. `...rest` collects the remaining elements into a new array.

```ado
let [a, b, ...rest] = values
[a, b] = values # creates or shadows bindings in the current scope
```

## Expressions

Expressions include literals, identifiers, function calls, array operations, arithmetic, comparisons, logical operators, ranges, slices, safe indexing, pipe-forward, expression blocks, list comprehensions, match expressions, and `trace`.

### Function calls

```ado
print("sum", sum([1, 2, 3]))
let doubled = helper(x)
```

`print`, `len`, and `push` are parsed specially by the compiler. Other built-ins are rewritten to `ado_*` runtime helpers.

### Array literals and indexing

Arrays are zero-indexed.

```ado
let arr = [10, 20, 30]
print(arr[0]) # 10
arr[1] = 25
```

### Safe indexing

`arr?idx` returns `arr[idx]` when `idx` is in bounds, otherwise `0`. Add `:fallback` to choose another default.

```ado
let arr = [10, 20, 30]
print(arr?1)    # 20
print(arr?9)    # 0
print(arr?9:-1) # -1
```

### Slices

`arr[start..end]` returns a new array containing elements from `start` through `end - 1`.

```ado
let arr = [10, 20, 30, 40]
let middle = arr[1..3] # [20, 30]
```

### Range literals

An integer range literal `start..end` creates an array containing `start` through `end`, inclusive.

```ado
let nums = 1..3 # [1, 2, 3]
```

This differs from `for i in start..end`, where `end` is exclusive.

### Pipe-forward

`value |> fn` rewrites to `fn(value)`.

```ado
fn double(n) {
  return n * 2
}

let x = 5 |> double # double(5)
```

The right side currently expects a function name.

### Expression blocks

A block can be used as an expression. Earlier statements run normally and the final expression result becomes the block value.

```ado
let scoped = {
  let local = 40
  local = local + 2
} # 42
```

### List comprehensions

`[for name in start..end where filter body]` builds an integer array. The `where` clause is optional.

```ado
let squares = [for n in 0..5 n * n]
let evens = [for n in 0..10 where n % 2 == 0 n]
```

`trace(value)` prints an integer and returns it, which is useful while composing expressions.

### Match expressions and guards

`match` can also be used as an expression. Arms use `_` as the default and `when` as a guard.

```ado
let result = match status {
  ok when ready => 1,
  ok => 2,
  _ => 0
}
```

## Operators

### Arithmetic

| Operator | Meaning |
|----------|---------|
| `+` | Addition |
| `-` | Subtraction or unary negation |
| `*` | Multiplication |
| `/` | Integer division |
| `%` | Modulo |

### Comparison

| Operator | Meaning |
|----------|---------|
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal |
| `>=` | Greater than or equal |

Comparisons can be chained.

```ado
if 0 < x < 10 {
  print("x is between 0 and 10")
}
```

### Logical

| Operator | Meaning |
|----------|---------|
| `and` | Logical AND |
| `or` | Logical OR |
| `not` | Logical NOT |

### Special

| Operator | Meaning |
|----------|---------|
| `..` | Range literal or slice delimiter |
| `...` | Rest element in destructuring |
| `=>` | Match arm separator |
| `|>` | Pipe-forward |
| `?` | Safe array index |
| `where` | List comprehension filter |
| `when` | Match arm guard |

## Control flow

### `if` / `else`

```ado
if x > 0 {
  print("positive")
} else {
  print("not positive")
}
```

### `unless`

`unless condition { }` runs the block when the condition is false.

```ado
unless done {
  print("keep working")
} else {
  print("finished")
}
```

### `guard`

`guard condition else { body }` runs `body` and returns `0` when `condition` is false.

```ado
guard count > 0 else {
  print("empty")
  return 1
}
```

### `try` / `rescue`

`try { } rescue err { }` catches integer exceptions raised with `raise value`.

```ado
let result = 0
try {
  raise 42
  result = 1
} rescue err {
  result = err
}
```

### `while`

```ado
let i = 0
while i < 5 {
  print(i)
  i = i + 1
}
```

### `until`

`until condition { }` loops while `condition` is false.

```ado
let total = 0
until total >= 10 {
  total = total + 1
}
```

### `forever`

`forever { }` creates an infinite loop. Use `break` and `continue`.

```ado
forever {
  if done {
    break
  }
  if skip {
    continue
  }
}
```

### `once`

`once { }` runs its block at most once per syntactic block. The guard is emitted as a C `static` local, so repeated loop iterations do not re-run the body.

```ado
let seen = 0
for i in 0..3 {
  once {
    seen = seen + 1
  }
}
print(seen) # 1
```

### `maybe`

`maybe chance { } else { }` runs the first block when `ado_random(100) < chance`, otherwise it runs the optional `else` block. `chance` is an integer percentage.

```ado
maybe 100 {
  print("always")
} else {
  print("never")
}
```

### `for`

`for i in start..end` iterates from `start` to `end - 1`.

```ado
for i in 0..5 {
  print(i)
}
```

`for name in arr` iterates over integer array elements.

```ado
for value in [10, 20, 30] {
  print(value)
}
```

### `match`

`match` compares an expression against patterns using `==`. `_` is the default arm. `when` adds a guard condition to an arm.

```ado
enum Status { ok, err }

let status = ok
match status {
  ok when ready => print("ready"),
  ok => print("ok"),
  err => print("err"),
  _ => print("other")
}
```

Match can be used as a statement or as an expression returning the selected arm expression.

Match arms can contain a statement, a block, or an expression.

### `break` and `continue`

`break` exits the current loop. `continue` jumps to the next loop iteration.

```ado
for i in 0..10 {
  if i == 3 {
    continue
  }
  if i == 7 {
    break
  }
  print(i)
}
```

## Functions

Functions are declared with `fn`.

```ado
fn add(a, b) {
  return a + b
}

fn main() {
  print(add(2, 3))
  return 0
}
```

Function parameters are untyped identifiers. Return values use `return`. Non-`main` functions implicitly return `0` if control reaches the end of the generated C function.

`main` is the program entry point and its return value becomes the process exit code.

```ado
fn main() {
  return 0
}
```

Ado does not currently support first-class functions or function pointers.

## Enums

Enums emit C enum constants.

```ado
enum Color { red, green, blue }

fn main() {
  let c = green
  match c {
    red => print("red"),
    green => print("green"),
    blue => print("blue"),
    _ => print("unknown")
  }
  return 0
}
```

Enum variant names are in the C enum namespace.

## Assertions, defer, and swap

### Assertions

`assert expr` exits with status `1` and prints `Ado assertion failed` when `expr` is false.

```ado
let n = len([1, 2, 3])
assert n == 3
```

### Deferred execution

`defer statement` runs a statement when the surrounding C scope exits.

```ado
defer print("cleanup")
{
  defer print("inner cleanup")
}
```

### Parallel swap

`swap a, b` exchanges two assignable expressions.

```ado
let a = 1
let b = 2
swap a, b
print(a, b) # 2 1
```

## Compiler hints

The lexer recognizes `@hint` tokens and the help text mentions hints such as `@inline` and `@memo`, but current code generation does not apply optimization or memoization from these hints.

```ado
@inline
fn fast(n) {
  return n + 1
}
```

Treat compiler hints as accepted syntax for future tooling unless the compiler is extended.

## Standard library and built-ins

### Core functions

| Function | Syntax | Returns | Notes |
|----------|--------|---------|-------|
| `print` | `print(a, b, ...)` | `void` | Prints values separated by spaces and a newline. String literals become format text. |
| `trace` | `trace(value)` | `int` | Prints an integer and returns the same integer. |
| `len` | `len(arr)` | `int` | Array length. |
| `push` | `push(arr, value)` | `void` | Appends to an array. |

### Math helpers

| Function | Meaning |
|----------|---------|
| `abs(x)` | Absolute value |
| `min(a, b)` | Minimum |
| `max(a, b)` | Maximum |
| `clamp(x, lo, hi)` | Clamp to inclusive range |
| `pow(base, exp)` | Integer exponentiation |
| `sign(x)` | `-1`, `0`, or `1` |
| `is_even(x)` | `1` if even |
| `is_odd(x)` | `1` if odd |
| `gcd(a, b)` | Greatest common divisor |
| `lcm(a, b)` | Least common multiple |
| `factorial(n)` | Factorial |
| `fib(n)` | Fibonacci number at index `n` |

### Array helpers

| Function | Meaning |
|----------|---------|
| `sum(arr)` | Sum of elements |
| `avg(arr)` | Integer average |
| `contains(arr, value)` | `1` if value exists |
| `find(arr, value)` | First index or `-1` |
| `count_if(arr, value)` | Number of matching elements |
| `all(arr)` | `1` if all elements are truthy |
| `any(arr)` | `1` if any element is truthy |
| `pop(arr)` | Remove and return last element |
| `reverse(arr)` | Reverse in place |
| `remove(arr, index)` | Remove element at index |
| `insert(arr, index, value)` | Insert at index |
| `take(arr, count)` | New array with first `count` elements |
| `drop(arr, count)` | New array without first `count` elements |
| `concat(a, b)` | New concatenated array |
| `fill(count, value)` | New array with repeated value |
| `filter(arr, value)` | New array without matching value |
| `sort(arr)` | Sort in place |
| `unique(arr)` | New deduplicated array |

### Capacity and reflection

| Function | Meaning |
|----------|---------|
| `capacity(arr)` | Current allocated capacity |
| `reserve(arr, capacity)` | Reserve capacity |
| `shrink_to_fit(arr)` | Shrink capacity to length |
| `reflect(arr)` | Print array length and capacity |

### File, environment, time, and process

| Function | Meaning |
|----------|---------|
| `getenv(name)` | Print environment variable and return `1` if found, otherwise `0` |
| `exit(code)` | Exit process |
| `read_file(path)` | Print file contents and return `0` on success, `-1` on failure |
| `write_file(path, value)` | Write an integer to a file |
| `file_exists(path)` | Return `1` if file exists |
| `sleep(ms)` | Sleep for milliseconds |
| `time()` | Current Unix timestamp |
| `random(max)` | Random integer in `[0, max)` |

### HTTP helpers

HTTP helpers require libcurl. Response bodies are printed to stderr. Return values are HTTP status codes or `-1` on failure.

| Function | Meaning |
|----------|---------|
| `http_get(url)` | GET request |
| `http_post(url, body)` | POST request |
| `http_put(url, body)` | PUT request |
| `http_delete(url)` | DELETE request |
| `http_status(url)` | GET request and return status only |

```ado
fn main() {
  let code = http_status("https://httpbin.org/status/204")
  print(code)
  return 0
}
```

## Compiler and targets

### Build and run

```bash
make
./doc file.do
```

`./doc file.do` writes generated C to a temporary directory, compiles it with `cc -O1`, runs the binary, then removes the temporary files.

### Install

```bash
sudo make install
ado file.do
ado-lsp
```

The install target installs `ado` and `ado-lsp` into `$(PREFIX)/bin`.

### WebAssembly target

The compiler can emit WebAssembly text format.

```bash
./doc --target wasm examples/stdlib.do
wat2wasm examples/stdlib.do.wat -o examples/stdlib.do.wasm
wasmtime examples/stdlib.do.wasm
```

The Makefile provides the same flow:

```bash
make wasm
```

WASM support depends on the current `codegen_wasm.c` implementation and external tools such as `wat2wasm` and a WASI runtime.

## Implementation notes and limitations

- Generated C uses `__auto_type`, so a GCC or Clang-compatible compiler is expected.
- The runtime uses libcurl for HTTP helpers.
- Arrays are dynamic integer arrays, not multi-dimensional or generic containers.
- Strings are not a complete value type yet.
- Type hints and compiler hints are parsed but do not affect generated C yet.
- There is no module system, package manager, floating-point type, struct type, dictionary type, or static type checker yet.
