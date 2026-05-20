# Ado Language Tutorial

Welcome to the Ado language tutorial! Ado is a minimal, performant programming language that compiles to C. This tutorial will guide you through the basics of the language, from "Hello World" to more advanced features.

## 1. Hello World

Let's start with the classic "Hello World" program. In Ado, the entry point for execution is the `main` function.

```ado
# This is a comment
fn main() {
  print("Hello World")
  return 0
}
```

To run this, save it as `hello.do` and use the Ado compiler:
```bash
./doc hello.do
```

## 2. Variables

Variables in Ado are declared using the `let` keyword. Currently, Ado supports integers and booleans.

```ado
fn main() {
  let x = 10
  let y = 20
  let sum = x + y

  print("The sum is:", sum)
  return 0
}
```

Variables can also be reassigned without the `let` keyword:

```ado
fn main() {
  let counter = 0
  counter = counter + 1
  return counter
}
```

## 3. Control Flow

### If/Else

Ado supports standard `if` and `else` statements for conditional logic.

```ado
fn main() {
  let number = 42

  if number > 50 {
    print("Greater than 50")
  } else {
    print("Less than or equal to 50")
  }

  return 0
}
```

### While Loops

Use `while` loops to repeat a block of code while a condition is true.

```ado
fn main() {
  let i = 0

  while i < 5 {
    print(i)
    i = i + 1
  }

  return 0
}
```

## 4. Functions

Functions are defined using the `fn` keyword. They can take parameters and return values.

```ado
fn add(a, b) {
  return a + b
}

fn factorial(n) {
  if n <= 1 {
    return 1
  }
  return n * factorial(n - 1)
}

fn main() {
  let result = add(5, 7)
  print("5 + 7 =", result)

  let fact5 = factorial(5)
  print("Factorial of 5 =", fact5)

  return 0
}
```

## 5. REPL

Ado also comes with an interactive REPL (Read-Eval-Print Loop). You can start it by running the compiler without any arguments:

```bash
./doc
```

You can type expressions directly into the REPL to see their results. You can also run a file and then enter the REPL:

```bash
./doc -i my_script.do
```

## Next Steps

Now that you know the basics, check out the [Language Reference](REFERENCE.md) for full details, or look at the [Examples](EXAMPLES.md) to see more complex programs written in Ado!
