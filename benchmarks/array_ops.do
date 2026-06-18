fn main() {
  let n = 20000
  let arr = []
  let i = 0
  while i < n {
    push(arr, i)
    i = i + 1
  }
  let total = ado_sum(arr)
  ado_reverse(arr)
  let check = arr[0]
  i = 0
  while i < 1000 {
    push(arr, i)
    i = i + 1
  }
  print("total:", total, "first:", check, "final_len:", len(arr))
  return total
}
