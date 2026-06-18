# Ado I/O and OS Standard Library
# File operations and system utilities

# Print a labeled value
fn debug(label, value) {
  print(label, "=", value)
  return value
}

# Print array contents
fn print_array(arr) {
  let i = 0
  print("[")
  while i < len(arr) {
    print(arr[i])
    if i < len(arr) - 1 {
      i = i + 1
      continue
    }
    break
  }
  print("]")
  return 0
}

# Benchmark a function (runs n times, returns execution count)
fn benchmark(n) {
  let count = 0
  while count < n {
    count = count + 1
  }
  return count
}

# Identity function
fn identity(x) {
  return x
}

# Compose two functions (manual: call expects fn name as string)
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

# Clamp value to array range
fn clamp_index(idx, arr) {
  let n = len(arr)
  if idx < 0 {
    return 0
  }
  if idx >= n {
    return n - 1
  }
  return idx
}
