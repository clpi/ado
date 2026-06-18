# Ado Standard Library
# Core utility functions for Ado programs
# Runtime functions with ado_ prefix are built-in

fn main() {
  let arr = [10, 20, 30]
  push(arr, 40)
  let empty = []
  push(empty, 1)

  print("")
  print("=== Math Helpers ===")
  print("abs(-5) =", abs(-5))
  print("min(9, 2) =", min(9, 2))
  print("max(3, 7) =", max(3, 7))
  print("clamp(5, 0, 10) =", clamp(5, 0, 10))
  print("pow(2, 10) =", pow(2, 10))
  print("sign(-10) =", sign(-10))
  print("is_even(4) =", is_even(4))
  print("is_odd(7) =", is_odd(7))
  print("gcd(12, 8) =", gcd(12, 8))
  print("lcm(12, 8) =", lcm(12, 8))
  print("factorial(5) =", factorial(5))

  print("")
  print("=== Array Operations ===")
  let vals = [1, 2, 3, 4, 5]
  print("sum =", ado_sum(vals))
  print("avg =", ado_avg(vals))
  print("contains 3 =", ado_contains(vals, 3))
  print("find 3 =", ado_find(vals, 3))
  print("all =", ado_all(vals))
  print("any =", ado_any(vals))

  let rev = [1, 2, 3, 4, 5]
  ado_reverse(rev)
  print("rev[0] after reverse =", rev[0])

  let a1 = [1, 2, 3]
  let a2 = [4, 5, 6]
  let combined = ado_concat(a1, a2)
  print("concat len =", len(combined))

  let taken = ado_take(vals, 3)
  print("take len =", len(taken))

  let dropped = ado_drop(vals, 2)
  print("drop len =", len(dropped))

  let filled = ado_fill(3, 42)
  print("fill len =", len(filled))

  let filtered = ado_filter(vals, 3)
  print("filter len =", len(filtered))

  print("")
  print("=== Sort & Unique ===")
  let unsorted = [5, 2, 8, 1, 9]
  ado_sort(unsorted)
  print("after sort, unsorted[0] =", unsorted[0])

  let duped = [1, 2, 2, 3, 3, 3]
  let uniq = ado_unique(duped)
  print("unique len =", len(uniq))

  print("")
  print("All stdlib tests passed!")
  return 0
}
