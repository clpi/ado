# New Ado Language Features Demo

fn main() {
  # Compound assignment operators
  let x = 10
  x += 5
  print("x += 5:", x)
  x -= 3
  print("x -= 3:", x)
  x *= 2
  print("x *= 2:", x)
  x /= 4
  print("x /= 4:", x)
  x %= 3
  print("x %= 3:", x)

  # For loop with step
  print("")
  print("for i in 0..20 step 3:")
  for i in 0..20 step 3 {
    print(i)
  }

  # Multi-line strings with backticks (usable in print)
  print("")
  print(`hello
multi-line
world`)

  print("")
  print("All new features work!")
  return 0
}
