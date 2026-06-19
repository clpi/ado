# Rare but useful Ado features demo

enum Kind { zero, one, two }

fn main() {
  let values = [1, 2, 3, 4, 5]

  let [a, b, ...rest] = values
  assert a == 1 and b == 2 and len(rest) == 3

  [x, y] = values
  assert x == 1 and y == 2

  let squares = [for n in 0..5 n * n]
  assert len(squares) == 5 and squares[4] == 16

  let even_squares = [for n in 0..10 where n % 2 == 0 n]
  assert len(even_squares) == 5 and even_squares[4] == 8

  let scoped = {
    let local = 40
    local = local + 2
  }
  assert scoped == 42

  let kind = one
  let matched = match kind {
    zero => 0,
    one when kind == one => 10,
    two => 20,
    _ => -1
  }
  assert matched == 10

  let handled = 0
  try {
    raise 77
    handled = 1
  } rescue err {
    handled = err
  }
  assert handled == 77

  print("rare features ok")
  return 0
}
