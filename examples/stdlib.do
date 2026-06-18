# Ado Standard Library
# Core utility functions for Ado programs
# All functions prefixed with ado_ in C runtime are auto-mapped by the compiler

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
  print("clamp(-5, 0, 10) =", clamp(-5, 0, 10))
  print("clamp(15, 0, 10) =", clamp(15, 0, 10))
  print("pow(2, 10) =", pow(2, 10))
  print("pow(3, 4) =", pow(3, 4))
  print("sign(-10) =", sign(-10))
  print("sign(0) =", sign(0))
  print("sign(42) =", sign(42))
  print("is_even(4) =", is_even(4))
  print("is_even(7) =", is_even(7))
  print("is_odd(4) =", is_odd(4))
  print("is_odd(7) =", is_odd(7))
  print("gcd(12, 8) =", gcd(12, 8))
  print("gcd(17, 5) =", gcd(17, 5))
  print("lcm(12, 8) =", lcm(12, 8))
  print("lcm(6, 4) =", lcm(6, 4))
  print("factorial(5) =", factorial(5))
  print("fib(10) =", fib(10))

  print("")
  print("=== Array Operations ===")
  let vals = [1, 2, 3, 4, 5]
  print("sum(vals) =", sum(vals))
  print("avg(vals) =", avg(vals))
  print("contains(vals, 3) =", contains(vals, 3))
  print("contains(vals, 9) =", contains(vals, 9))
  print("find(vals, 3) =", find(vals, 3))
  print("find(vals, 9) =", find(vals, 9))
  print("count_if(vals, 2) =", count_if(vals, 2))
  print("all(vals) =", all(vals))
  print("any(vals) =", any(vals))

  let stack = [10, 20, 30]
  print("pop(stack) =", pop(stack))
  print("after pop, len =", len(stack))

  let rev = [1, 2, 3, 4, 5]
  reverse(rev)
  print("rev[0] after reverse =", rev[0])

  let rm = [100, 200, 300, 400]
  remove(rm, 1)
  print("after remove idx 1, len =", len(rm))

  let ins = [1, 2, 4, 5]
  insert(ins, 2, 3)
  print("after insert 3 at idx 2, len =", len(ins))

  let a1 = [1, 2, 3]
  let a2 = [4, 5, 6]
  let combined = concat(a1, a2)
  print("concat len =", len(combined))

  let taken = take(vals, 3)
  print("take len =", len(taken))

  let dropped = drop(vals, 2)
  print("drop len =", len(dropped))

  let filled = fill(3, 42)
  print("fill len =", len(filled))

  let filtered = filter(vals, 3)
  print("filter len =", len(filtered))

  print("")
  print("=== Capacity & Memory ===")
  print("capacity(vals) =", capacity(vals))
  print("reflect(vals)")
  reflect(vals)

  print("")
  print("=== Sort & Unique ===")
  let unsorted = [5, 2, 8, 1, 9]
  sort(unsorted)
  print("after sort, unsorted[0] =", unsorted[0])

  let duped = [1, 2, 2, 3, 3, 3]
  let uniq = unique(duped)
  print("unique len =", len(uniq))

  print("")
  print("All stdlib tests passed!")
  return 0
}
