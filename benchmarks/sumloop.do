fn main() {
  let i = 0
  let total = 0
  while i < 50000000 {
    total = total + i
    i = i + 1
  }
  print("sum:", total)
  return total
}
