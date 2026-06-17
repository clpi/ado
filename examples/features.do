# Rare but useful Ado features demo

fn main() {
  let arr = [10, 20, 30]
  assert len(arr) == 3
  print(arr?1)
  print(arr?9:-1)

  let a = 1
  let b = 2
  swap a, b
  print(a, b)

  unless a == 1 {
    print("not one")
  } else {
    print("one")
  }

  let i = 0
  forever {
    if i == 3 {
      break
    }
    i = i + 1
  }
  print(i)

  let j = 0
  forever {
    j = j + 1
    if j % 2 == 0 {
      continue
    }
    if j == 5 {
      break
    }
  }
  print(j)
  return 0
}
