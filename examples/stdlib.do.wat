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
    local.get $arr
    i32.load align=4 ;; len
    local.tee $len
    local.get $arr
    i32.load offset=4 align=4 ;; cap
    local.tee $cap
    local.get $len
    i32.eqz
    if
      i32.const 4
      local.set $cap
    end
    local.get $len
    local.get $cap
    i32.ge_s
    if (result i32)
      local.get $cap
      i32.const 2
      i32.mul ;; new_cap
    else
      local.get $cap
    end
    local.tee $newcap
    (; grow array data ;)
    ;; simple version: just store if room, else grow
  (func $main (export "_start") (result i32)
  (local $arr i32)
    (local $tmp0 i32)
  i32.const 16
  call $malloc
  local.set $tmp0
  i32.const 3
  local.get $tmp0
  i32.store align=4
  i32.const 10
  local.get $tmp0
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 20
  local.get $tmp0
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 30
  local.get $tmp0
  i32.const 12
  i32.add
  i32.store align=4
  local.get $tmp0
  local.set $arr
  ;; push start
  local.get $arr
  local.tee $push_arr
  i32.load align=4
  local.tee $push_len
  local.get $push_arr
  i32.load offset=4 align=4
  local.tee $push_cap
  local.get $push_len
  i32.ge_s
  br_if $grow_0
  local.get $push_arr
  local.get $push_len
  i32.const 4
  i32.mul
  i32.add
  i32.const 40
  i32.store align=4
  local.get $push_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.store align=4
  br $after_1
$grow_0
  local.get $push_cap
  i32.const 2
  i32.mul
  local.set $new_cap
  local.get $new_cap
  i32.const 4
  i32.mul
  call $malloc
  local.tee $new_arr
  local.get $push_arr
  i32.load align=4
  local.get $new_arr
  i32.const 0
  local.get $push_cap
  i32.const 4
  i32.mul
  call $memcpy
  drop
  local.get $new_arr
  local.get $new_cap
  i32.store offset=4 align=4
  local.get $new_arr
  ;; store new data pointer into old array header
  local.get $push_arr
  local.get $new_arr
  i32.store align=4
  local.get $new_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.const 4
  i32.mul
  i32.add
  i32.const 40
  i32.store align=4
  local.get $new_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.store align=4
$after_1
  ;; push end
  (local $tmp1 i32)
  local.get $empty
  local.set $tmp1
    (local $tmp2 i32)
  i32.const 4
  call $malloc
  local.set $tmp2
  i32.const 0
  local.get $tmp2
  i32.store align=4
  local.get $tmp2
  local.get $tmp1
  i32.store align=4
  ;; push start
  local.get $empty
  local.tee $push_arr
  i32.load align=4
  local.tee $push_len
  local.get $push_arr
  i32.load offset=4 align=4
  local.tee $push_cap
  local.get $push_len
  i32.ge_s
  br_if $grow_2
  local.get $push_arr
  local.get $push_len
  i32.const 4
  i32.mul
  i32.add
  i32.const 1
  i32.store align=4
  local.get $push_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.store align=4
  br $after_3
$grow_2
  local.get $push_cap
  i32.const 2
  i32.mul
  local.set $new_cap
  local.get $new_cap
  i32.const 4
  i32.mul
  call $malloc
  local.tee $new_arr
  local.get $push_arr
  i32.load align=4
  local.get $new_arr
  i32.const 0
  local.get $push_cap
  i32.const 4
  i32.mul
  call $memcpy
  drop
  local.get $new_arr
  local.get $new_cap
  i32.store offset=4 align=4
  local.get $new_arr
  ;; store new data pointer into old array header
  local.get $push_arr
  local.get $new_arr
  i32.store align=4
  local.get $new_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.const 4
  i32.mul
  i32.add
  i32.const 1
  i32.store align=4
  local.get $new_arr
  local.get $push_len
  i32.const 1
  i32.add
  i32.store align=4
$after_3
  ;; push end
  (local $tmp3 i32)
  local.get $vals
  local.set $tmp3
    (local $tmp4 i32)
  i32.const 24
  call $malloc
  local.set $tmp4
  i32.const 5
  local.get $tmp4
  i32.store align=4
  i32.const 1
  local.get $tmp4
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp4
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 3
  local.get $tmp4
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp4
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp4
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp4
  local.get $tmp3
  i32.store align=4
  (local $stack i32)
    (local $tmp5 i32)
  i32.const 16
  call $malloc
  local.set $tmp5
  i32.const 3
  local.get $tmp5
  i32.store align=4
  i32.const 10
  local.get $tmp5
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 20
  local.get $tmp5
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 30
  local.get $tmp5
  i32.const 12
  i32.add
  i32.store align=4
  local.get $tmp5
  local.set $stack
  call $ado_pop
    local.get $stack
  call $print_i32
  local.get $stack
  i32.load align=4 ;; arr.len

  call $print_i32
  (local $rev i32)
    (local $tmp6 i32)
  i32.const 24
  call $malloc
  local.set $tmp6
  i32.const 5
  local.get $tmp6
  i32.store align=4
  i32.const 1
  local.get $tmp6
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp6
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 3
  local.get $tmp6
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp6
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp6
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp6
  local.set $rev
  ;; unsupported stmt type=5
    (local $tmp7 i32)
local.get $rev
  local.set $tmp7
  local.get $tmp7
i32.const 0
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  (local $rm i32)
    (local $tmp8 i32)
  i32.const 20
  call $malloc
  local.set $tmp8
  i32.const 4
  local.get $tmp8
  i32.store align=4
  i32.const 100
  local.get $tmp8
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 200
  local.get $tmp8
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 300
  local.get $tmp8
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 400
  local.get $tmp8
  i32.const 16
  i32.add
  i32.store align=4
  local.get $tmp8
  local.set $rm
  ;; unsupported stmt type=5
  local.get $rm
  i32.load align=4 ;; arr.len

  call $print_i32
  (local $ins i32)
    (local $tmp9 i32)
  i32.const 20
  call $malloc
  local.set $tmp9
  i32.const 4
  local.get $tmp9
  i32.store align=4
  i32.const 1
  local.get $tmp9
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp9
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp9
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp9
  i32.const 16
  i32.add
  i32.store align=4
  local.get $tmp9
  local.set $ins
  ;; unsupported stmt type=5
  local.get $ins
  i32.load align=4 ;; arr.len

  call $print_i32
  (local $nums i32)
    (local $tmp10 i32)
  i32.const 24
  call $malloc
  local.set $tmp10
  i32.const 5
  local.get $tmp10
  i32.store align=4
  i32.const 5
  local.get $tmp10
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp10
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 8
  local.get $tmp10
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 1
  local.get $tmp10
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 9
  local.get $tmp10
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp10
  local.set $nums
  (local $i i32)
  i32.const 0
  local.set $i
  (local $total i32)
  i32.const 0
  local.set $total
  (block $end_5 (loop $loop_4
    local.get $i
    local.get $nums
  i32.load align=4 ;; arr.len

    i32.lt_s
    i32.eqz
    br_if $end_5
      (local $tmp11 i32)
  local.get $total
  local.set $tmp11
  local.get $total
      (local $tmp12 i32)
local.get $nums
  local.set $tmp12
  local.get $tmp12
local.get $i
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

    i32.add
  local.get $tmp11
  i32.store align=4
  (local $tmp13 i32)
  local.get $i
  local.set $tmp13
  local.get $i
    i32.const 1
    i32.add
  local.get $tmp13
  i32.store align=4
    br $loop_4
    $end_5
  )) ;; end while
  local.get $total
  call $print_i32
  return i32.const 0
  i32.const 0
  call $print_i32
  call $ado_min
    i32.const 9
    i32.const 2
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_max
    i32.const 3
    i32.const 7
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_max
    i32.const 9
    i32.const 2
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_clamp
    i32.const 5
    i32.const 0
    i32.const 10
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_clamp
    i32.const 0
    i32.const 5
    i32.sub
    i32.const 0
    i32.const 10
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_clamp
    i32.const 15
    i32.const 0
    i32.const 10
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_pow
    i32.const 2
    i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_pow
    i32.const 2
    i32.const 10
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_pow
    i32.const 3
    i32.const 4
  call $print_i32
  i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_sign
    i32.const 0
    i32.const 10
    i32.sub
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_sign
    i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_sign
    i32.const 42
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_is_even
    i32.const 4
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_is_even
    i32.const 7
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_is_odd
    i32.const 4
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_is_odd
    i32.const 7
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_gcd
    i32.const 12
    i32.const 8
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_gcd
    i32.const 17
    i32.const 5
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_lcm
    i32.const 12
    i32.const 8
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_lcm
    i32.const 6
    i32.const 4
  call $print_i32
  i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  (local $vals i32)
    (local $tmp14 i32)
  i32.const 24
  call $malloc
  local.set $tmp14
  i32.const 5
  local.get $tmp14
  i32.store align=4
  i32.const 1
  local.get $tmp14
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp14
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 3
  local.get $tmp14
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp14
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp14
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp14
  local.set $vals
  i32.const 0
  call $print_i32
  call $ado_contains
    local.get $vals
    i32.const 3
  call $print_i32
  i32.const 0
  call $print_i32
  call $ado_contains
    local.get $vals
    i32.const 9
  call $print_i32
  (local $stack i32)
    (local $tmp15 i32)
  i32.const 16
  call $malloc
  local.set $tmp15
  i32.const 3
  local.get $tmp15
  i32.store align=4
  i32.const 10
  local.get $tmp15
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 20
  local.get $tmp15
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 30
  local.get $tmp15
  i32.const 12
  i32.add
  i32.store align=4
  local.get $tmp15
  local.set $stack
  i32.const 0
  call $print_i32
  call $ado_pop
    local.get $stack
  call $print_i32
  i32.const 0
  call $print_i32
  local.get $stack
  i32.load align=4 ;; arr.len

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp16 i32)
local.get $stack
  local.set $tmp16
  local.get $tmp16
i32.const 0
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp17 i32)
local.get $stack
  local.set $tmp17
  local.get $tmp17
i32.const 1
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  (local $rev i32)
    (local $tmp18 i32)
  i32.const 24
  call $malloc
  local.set $tmp18
  i32.const 5
  local.get $tmp18
  i32.store align=4
  i32.const 1
  local.get $tmp18
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp18
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 3
  local.get $tmp18
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp18
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp18
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp18
  local.set $rev
  ;; unsupported stmt type=5
  i32.const 0
  call $print_i32
    (local $tmp19 i32)
local.get $rev
  local.set $tmp19
  local.get $tmp19
i32.const 0
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp20 i32)
local.get $rev
  local.set $tmp20
  local.get $tmp20
i32.const 1
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp21 i32)
local.get $rev
  local.set $tmp21
  local.get $tmp21
i32.const 2
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp22 i32)
local.get $rev
  local.set $tmp22
  local.get $tmp22
i32.const 3
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp23 i32)
local.get $rev
  local.set $tmp23
  local.get $tmp23
i32.const 4
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  (local $rm i32)
    (local $tmp24 i32)
  i32.const 20
  call $malloc
  local.set $tmp24
  i32.const 4
  local.get $tmp24
  i32.store align=4
  i32.const 100
  local.get $tmp24
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 200
  local.get $tmp24
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 300
  local.get $tmp24
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 400
  local.get $tmp24
  i32.const 16
  i32.add
  i32.store align=4
  local.get $tmp24
  local.set $rm
  ;; unsupported stmt type=5
  i32.const 0
  call $print_i32
  local.get $rm
  i32.load align=4 ;; arr.len

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp25 i32)
local.get $rm
  local.set $tmp25
  local.get $tmp25
i32.const 0
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp26 i32)
local.get $rm
  local.set $tmp26
  local.get $tmp26
i32.const 1
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp27 i32)
local.get $rm
  local.set $tmp27
  local.get $tmp27
i32.const 2
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  (local $ins i32)
    (local $tmp28 i32)
  i32.const 20
  call $malloc
  local.set $tmp28
  i32.const 4
  local.get $tmp28
  i32.store align=4
  i32.const 1
  local.get $tmp28
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp28
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 4
  local.get $tmp28
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 5
  local.get $tmp28
  i32.const 16
  i32.add
  i32.store align=4
  local.get $tmp28
  local.set $ins
  ;; unsupported stmt type=5
  i32.const 0
  call $print_i32
  local.get $ins
  i32.load align=4 ;; arr.len

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp29 i32)
local.get $ins
  local.set $tmp29
  local.get $tmp29
i32.const 0
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp30 i32)
local.get $ins
  local.set $tmp30
  local.get $tmp30
i32.const 1
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp31 i32)
local.get $ins
  local.set $tmp31
  local.get $tmp31
i32.const 2
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp32 i32)
local.get $ins
  local.set $tmp32
  local.get $tmp32
i32.const 3
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
    (local $tmp33 i32)
local.get $ins
  local.set $tmp33
  local.get $tmp33
i32.const 4
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

  call $print_i32
  i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  (local $nums i32)
    (local $tmp34 i32)
  i32.const 24
  call $malloc
  local.set $tmp34
  i32.const 5
  local.get $tmp34
  i32.store align=4
  i32.const 5
  local.get $tmp34
  i32.const 4
  i32.add
  i32.store align=4
  i32.const 2
  local.get $tmp34
  i32.const 8
  i32.add
  i32.store align=4
  i32.const 8
  local.get $tmp34
  i32.const 12
  i32.add
  i32.store align=4
  i32.const 1
  local.get $tmp34
  i32.const 16
  i32.add
  i32.store align=4
  i32.const 9
  local.get $tmp34
  i32.const 20
  i32.add
  i32.store align=4
  local.get $tmp34
  local.set $nums
  (local $total i32)
  i32.const 0
  local.set $total
  (local $idx i32)
  i32.const 0
  local.set $idx
  (block $end_7 (loop $loop_6
    local.get $idx
i32.const 5
    i32.ge_s
    br_if $end_7
      (local $tmp35 i32)
  local.get $total
  local.set $tmp35
  local.get $total
      (local $tmp36 i32)
local.get $nums
  local.set $tmp36
  local.get $tmp36
local.get $idx
  i32.const 4
  i32.mul
  i32.add
  i32.load align=4 ;; arr.data[idx]

    i32.add
  local.get $tmp35
  i32.store align=4
    local.get $idx
  i32.const 1
  i32.add
  local.set $idx
    br $loop_6
    $end_7
  )) ;; end for
  i32.const 0
  call $print_i32
  local.get $total
  call $print_i32
  i32.const 0
  call $print_i32
  i32.const 0
  call $print_i32
  return i32.const 0
    i32.const 0
    return
  )
)
