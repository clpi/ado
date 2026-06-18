# Rare but useful Ado features demo

enum Status { ok, err }

fn main() {
  let status = ok
  match status {
    ok => print("ok"),
    err => print("err"),
    _ => print("other")
  }

  let guarded = 4
  guard guarded > 0 else {
    print("invalid")
    return 1
  }

  let total = 0
  until total >= 3 {
    total = total + 1
  }

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

  defer print("cleanup")
  {
    let inner = 9
    defer print("inner cleanup")
    guard inner == 9 else {
      print("bad inner")
      return 2
    }
  }

  return 0
}
