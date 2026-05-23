# Ado Language Tutorial

Welcome to the Ado programming language!

Ado is a fast, minimalistic language that compiles to C.

## Variables & Arrays

Variables in Ado use implicit typing via `let`. Ado leverages type inference behind the scenes to give you powerful granular control, which means arrays and complex expressions are as fast as native C implicitly-typed variables!

```ado
# Define an integer
let count = 10

# Define an array (internally points to memory like a C array)
let arr = [1, 2, 3, 4]

# Read from an array
print(arr[2]) # Prints 3
```

## Control Flow

Ado uses a very modern and clean syntax for loops and conditionals.

### The `for` loop
The `for` loop in Ado gives you a unique but intuitive syntax to iterate over ranges:

```ado
let sum = 0
for i in 0 .. 10 {
    sum = sum + i
}
print("Sum is: ", sum)
```

### The `while` loop

```ado
let a = 1
while a < 10 {
    a = a * 2
}
```

## Functions

Functions use the `fn` keyword.

```ado
fn add(a, b) {
    return a + b
}

fn main() {
    print(add(2, 3))
    return 0
}
```

Enjoy using Ado and its integrated language server for powerful IDE auto-completion and diagnostics!
