# Ado Math Standard Library
# Advanced math utilities beyond builtins

# Check if a number is prime
fn is_prime(n) {
  if n < 2 {
    return false
  }
  let i = 2
  while i * i <= n {
    if n % i == 0 {
      return false
    }
    i = i + 1
  }
  return true
}

# Sum of digits of a number
fn digit_sum(n) {
  n = abs(n)
  let total = 0
  while n > 0 {
    total = total + n % 10
    n = n / 10
  }
  return total
}

# Check if a number is a perfect square
fn is_square(n) {
  if n < 0 {
    return false
  }
  let i = 0
  while i * i < n {
    i = i + 1
  }
  return i * i == n
}

# Integer square root (floor)
fn isqrt(n) {
  if n < 0 {
    return 0
  }
  let x = n
  let y = (x + 1) / 2
  while y < x {
    x = y
    y = (x + n / x) / 2
  }
  return x
}

# Triangular number: 1 + 2 + ... + n
fn triangular(n) {
  if n <= 0 {
    return 0
  }
  return n * (n + 1) / 2
}

# Sum of first n squares
fn sum_of_squares(n) {
  if n <= 0 {
    return 0
  }
  return n * (n + 1) * (2 * n + 1) / 6
}

# Check if n is a perfect number (sum of proper divisors = n)
fn is_perfect(n) {
  if n <= 1 {
    return false
  }
  let total = 1
  let i = 2
  while i * i <= n {
    if n % i == 0 {
      total = total + i
      if i != n / i {
        total = total + n / i
      }
    }
    i = i + 1
  }
  return total == n
}

# Number of divisors
fn divisor_count(n) {
  if n <= 0 {
    return 0
  }
  let count = 0
  let i = 1
  while i * i <= n {
    if n % i == 0 {
      count = count + 1
      if i != n / i {
        count = count + 1
      }
    }
    i = i + 1
  }
  return count
}
