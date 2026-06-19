fn binary_search(arr, target) {
  let lo = 0
  let hi = len(arr) - 1
  while lo <= hi {
    let mid = (lo + hi) / 2
    if arr[mid] == target { return mid }
    if arr[mid] < target {
      lo = mid + 1
    } else {
      hi = mid - 1
    }
  }
  return -1
}

fn main() {
  let n = 100000
  let arr = []
  let i = 0
  while i < n {
    push(arr, i)
    i = i + 1
  }
  let total = 0
  i = 0
  while i < 50000 {
    let idx = i * 2
    let found = binary_search(arr, idx)
    if found >= 0 { total = total + found }
    i = i + 1
  }
  print("total:", total)
  return total
}
