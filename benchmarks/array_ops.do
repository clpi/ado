fn main() {
  let n = 20000
  let arr = []
  let i = 0
  while i < n {
    push(arr, i)
    i = i + 1
  }
  let total = sum(arr)
  let odd = filter(arr, -1)
  reverse(arr)
  i = 0
  while i < 1000 {
    push(arr, i)
    i = i + 1
  }
  let popped = 0
  while len(arr) > 0 {
    popped = pop(arr)
  }
  print("total:", total, "popped:", popped)
  return total
}
