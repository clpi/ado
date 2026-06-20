# Ado Language: Specification-Oriented Programming

## Vision

Ado is designed around the **"Specify-Verify-Execute"** paradigm - a unique approach where programs are written with explicit contracts and invariants that are verified at runtime. This gives developers immediate feedback when assumptions are violated.

## Core Concepts

### 1. Runtime Checks (`check`)

Explicitly verify conditions at specific points during execution:

```ado
fn main() {
    let x = square(5)
    check x > 0         # Verify condition, exits on failure
    check x == 25       # Assertion-style check
    return 0
}
```

**Syntax**: `check expression`
- If expression evaluates to false (0), the program prints "check failed: check" and exits
- Simple and direct - no parentheses needed

### 2. Loop Invariants (`invariant`)

Ensure state properties remain true throughout loop execution:

```ado
fn main() {
    let i = 0
    while i < 10 {
        invariant i >= 0    # Must remain true each iteration
        i = i + 1
    }
    return 0
}
```

**Syntax**: `invariant condition` inside loops
- Checked at the beginning of each iteration
- Violations print "check failed: invariant" and exit

### 3. Aspect-Oriented Hooks (`~` syntax)

Cross-cutting concerns using unique `~` prefix syntax:

```ado
# Around advice - wraps function execution
~square {
    print("Before square")
    let result = square(_)
    print("After:", result)
    return result
}
```

**Syntax**:
- `~fn_name { body }` - around advice (body is the wrapped function)
- `~before fn_name { body }` - before advice
- `~after fn_name { body }` - after advice

## Unique Language Features

### Type Hints: Required vs Optional

```ado
let x::Int = 5      # Required type - must satisfy condition
let y:Int = 10      # Optional type - hint only, no runtime check
```

### Expression Blocks

```ado
let result = {
    let temp = 42
    temp * 2         # Block returns last expression
}
```

### Pattern Matching

```ado
match value {
    0 => "zero"
    1 => "one"
    n when n > 0 => "positive"
    _ => "other"
}
```

## Design Philosophy

1. **Explicit Contracts**: Critical invariants are declared directly
2. **Runtime Verification**: All checks execute during program run
3. **Minimal Syntax**: Compact syntax using lowercase keywords and symbols
4. **Immediate Feedback**: Violations report precise location and condition

The combination of `check` and `invariant` creates a specification-oriented workflow where developers can express critical assumptions directly in code.