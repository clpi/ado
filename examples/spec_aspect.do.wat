(module
  (import "env" "print_i32" (func $print_i32 (param i32)))
  (import "env" "memcpy" (func $memcpy (param i32 i32 i32) (result i32)))
  (import "env" "malloc" (func $malloc (param i32) (result i32)))
  (import "env" "itoa" (func $itoa (param i32) (result i32)))
  (memory (export "memory") 1 256)
  (func $ado_abs (param $x i32) (result i32)
    local.get $x
    i32.const 0
    i32.lt_s
    if (result i32)
      i32.const 0
      local.get $x
      i32.sub
    else
      local.get $x
    end
  )
  (func $ado_min (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.gt_s
    if (result i32)
      local.get $b
    else
      local.get $a
    end
  )
  (func $ado_max (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.lt_s
    if (result i32)
      local.get $b
    else
      local.get $a
    end
  )
  (func $ado_clamp (param $x i32) (param $lo i32) (param $hi i32) (result i32)
    local.get $x
    local.get $lo
    i32.lt_s
    if (result i32)
      local.get $lo
    else
      local.get $x
      local.get $hi
      i32.gt_s
      if (result i32)
        local.get $hi
      else
        local.get $x
      end
    end
  )
  (func $ado_pow (param $b i32) (param $e i32) (result i32)
    (local $r i32)
    i32.const 1
    local.set $r
    (block $break (loop $continue
      local.get $e
      i32.eqz
      br_if $break
      local.get $r
      local.get $b
      i32.mul
      local.set $r
      local.get $e
      i32.const 1
      i32.sub
      local.set $e
      br $continue
    ))
    local.get $r
  )
  (func $ado_sign (param $x i32) (result i32)
    local.get $x
    i32.const 0
    i32.gt_s
    if (result i32)
      i32.const 1
    else
      local.get $x
      i32.const 0
      i32.lt_s
      if (result i32)
        i32.const -1
      else
        i32.const 0
      end
    end
  )
  (func $ado_gcd (param $a i32) (param $b i32) (result i32)
    (block $break (loop $loop
      local.get $b
      i32.eqz
      br_if $break
      local.get $b
      local.set $t
      local.get $a
      local.get $b
      i32.rem_s
      local.set $a
      local.get $t
      local.set $b
      br $loop
    ))
    local.get $a
  )
  (func $ado_fib (param $n i32) (result i32)
    (local $a i32) (local $b i32) (local $i i32)
    i32.const 0
    local.set $a
    i32.const 1
    local.set $b
    i32.const 0
    local.set $i
    (block $break (loop $loop
      local.get $i
      local.get $n
      i32.ge_s
      br_if $break
      local.get $b
      local.set $t
      local.get $a
      local.get $b
      i32.add
      local.set $b
      local.get $t
      local.set $a
      local.get $i
      i32.const 1
      i32.add
      local.set $i
      br $loop
    ))
    local.get $a
  )
  (func $ado_push (param $arr i32) (param $val i32)
    (local $len i32) (local $cap i32) (local $newcap i32)
    local.get $arr
    i32.load
    local.tee $len
    local.get $arr
    i32.load offset=4
    local.tee $cap
    local.get $len
    i32.ge_s
    if (result i32)
      local.get $cap
      i32.const 2
      i32.mul
    else
      local.get $cap
    end
    local.tee $newcap
    i32.const 4
    i32.mul
    call $malloc
    local.get $arr
    i32.load
    local.get $arr
    i32.load offset=4
    i32.const 4
    i32.mul
    call $memcpy
    drop
    local.get $arr
    local.get $val
    i32.store
    local.get $arr
    local.get $len
    i32.const 1
    i32.add
    i32.store
  )
  (import "wasi_snapshot_preview1" "fd_write" (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit" (func $proc_exit (param i32)))
  (func $greet (param $name i32)) (result i32
  i32.const 0
  call $print_i32
  local.get $name
  call $print_i32
  return i32.const 0
    i32.const 0
    return
  )

  (func $main (export "_start") (result i32)
  ;; unsupported stmt type=5
  return i32.const 0
    i32.const 0
    return
  )
)
