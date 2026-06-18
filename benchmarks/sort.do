fn main() {
  let n = 5000
  let arr = []
  let i = 0
  while i < n {
    push(arr, n - i)
    i = i + 1
  }
  ado_sort(arr)
  let ok = 1
  i = 0
  while i < n - 1 {
    if arr[i] > arr[i + 1] {
      ok = 0
    }
    i = i + 1
  }
  print("sorted:", ok)
  return ok
}
