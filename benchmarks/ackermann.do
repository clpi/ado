fn ack(m, n) {
  if m == 0 { return n + 1 }
  if n == 0 { return ack(m - 1, 1) }
  return ack(m - 1, ack(m, n - 1))
}

fn main() {
  let result = ack(3, 8)
  print("ack(3,8):", result)
  return result
}
