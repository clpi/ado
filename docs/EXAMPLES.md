# Ado Examples

This document highlights several example programs included in the Ado repository (in the `examples/` directory) and explains how they work. These examples demonstrate standard Ado language features in practical contexts.

## 1. Math Functions (`math.do`)

This example demonstrates functions, recursion, and mathematical operations.

```ado
# Power function using recursion
fn power(base, exp) {
  if exp == 0 {
    return 1
  }
  return base * power(base, exp - 1)
}

# Fibonacci sequence using recursion
fn fib(n) {
  if n <= 1 {
    return n
  }
  return fib(n - 1) + fib(n - 2)
}

fn main() {
  print("2^10 =", power(2, 10))
  print("10th Fibonacci number =", fib(10))
  return 0
}
```

**Key Takeaways:**
- Functions can be fully recursive. Since Ado compiles to C, recursion speed is dictated by standard C optimization (including potential tail-call optimization depending on the compiler).
- `print` can accept multiple arguments, mixing strings and integers.

## 2. Collatz Conjecture (`collatz.do`)

The Collatz conjecture is a famous mathematical sequence. This example uses a `while` loop to compute the sequence length for a given number.

```ado
fn collatz_length(n) {
  let length = 1

  while n > 1 {
    if n % 2 == 0 {
      n = n / 2
    } else {
      n = (3 * n) + 1
    }
    length = length + 1
  }

  return length
}

fn main() {
  let start = 27
  let len = collatz_length(start)
  print("Collatz sequence length for", start, "is", len)
  return 0
}
```

**Key Takeaways:**
- Demonstrates `while` loops combined with `if/else` control flow.
- Highlights variable reassignment (`n = n / 2` and `length = length + 1`).
- Uses modulo operator `%` to test for even numbers.

## 3. Conditionals (`conditionals.do`)

This example shows comprehensive conditional operators.

```ado
fn check_number(n) {
  if n > 0 {
    print(n, "is positive")
  } else {
    if n < 0 {
      print(n, "is negative")
    } else {
      print(n, "is zero")
    }
  }
  return 0
}

fn main() {
  let a = 10
  let b = -5
  let c = 0

  check_number(a)
  check_number(b)
  check_number(c)

  let is_valid = a > 0 and b < 0
  if is_valid {
    print("Complex condition met!")
  }

  return 0
}
```

**Key Takeaways:**
- `if` statements can be nested.
- Logical operators like `and` can be used to evaluate complex boolean conditions.
- Variables can store boolean results (`let is_valid = ...`).
