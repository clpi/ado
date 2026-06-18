# Ado I/O and OS Standard Library
# Utility functions (int-only params/returns)

# Identity function
fn identity(x) {
  return x
}

# Compose two functions (manual)
fn compose_abs_square(x) {
  let a = abs(x)
  return a * a
}

# Safe division (returns 0 on divide by zero)
fn safe_div(a, b) {
  if b == 0 {
    return 0
  }
  return a / b
}

# Benchmark a function (runs n iterations)
fn benchmark(n) {
  let count = 0
  while count < n {
    count = count + 1
  }
  return count
}

# Main entry point for testing
fn main() {
  print("io lib test:", identity(42))
  return 0
}
