fn main() {
  let n = 64
  let a = []
  let b = []
  let c = []
  let i = 0
  while i < n * n {
    push(a, i)
    push(b, (n * n) - i)
    push(c, 0)
    i = i + 1
  }
  i = 0
  while i < n {
    let j = 0
    while j < n {
      let sum = 0
      let k = 0
      while k < n {
        sum = sum + a[i * n + k] * b[k * n + j]
        k = k + 1
      }
      c[i * n + j] = sum
      j = j + 1
    }
    i = i + 1
  }
  print("result:", c[0], c[n*n - 1])
  return c[0]
}
