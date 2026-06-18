# Ado Array Standard Library
# Utility functions (int-only params/returns)

# Product of all elements
fn product(arr) {
  let total = 1
  let i = 0
  while i < len(arr) {
    total = total * arr[i]
    i = i + 1
  }
  return total
}

# Maximum element in array
fn max_element(arr) {
  if len(arr) == 0 {
    return 0
  }
  let m = arr[0]
  let i = 1
  while i < len(arr) {
    m = max(m, arr[i])
    i = i + 1
  }
  return m
}

# Minimum element in array
fn min_element(arr) {
  if len(arr) == 0 {
    return 0
  }
  let m = arr[0]
  let i = 1
  while i < len(arr) {
    m = min(m, arr[i])
    i = i + 1
  }
  return m
}

# Main entry point for testing
fn main() {
  let vals = [1, 2, 3, 4, 5]
  print("product:", product(vals))
  print("max:", max_element(vals))
  print("min:", min_element(vals))
  return 0
}
