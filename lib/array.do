# Ado Array Standard Library
# Higher-order array operations and utilities

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

# Dot product of two arrays
fn dot_product(a, b) {
  let n = len(a)
  if n > len(b) {
    n = len(b)
  }
  let total = 0
  let i = 0
  while i < n {
    total = total + a[i] * b[i]
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

# Check if arrays are equal
fn equal(a, b) {
  if len(a) != len(b) {
    return false
  }
  let i = 0
  while i < len(a) {
    if a[i] != b[i] {
      return false
    }
    i = i + 1
  }
  return true
}

# Median of a sorted array
fn median(sorted) {
  if len(sorted) == 0 {
    return 0
  }
  let n = len(sorted)
  if n % 2 == 0 {
    return (sorted[n / 2 - 1] + sorted[n / 2]) / 2
  }
  return sorted[n / 2]
}
