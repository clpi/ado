# Ado Language - Spec-Oriented Design
# 
# Unique Vision: "Specify-Verify-Execute"
# - Contracts as first-class language constructs
# - Aspect-like hooks without traditional inheritance
# - Minimal syntax with maximal expressiveness

# --- UNIQUE SYNTAX FEATURES ---

# 1. Function contracts (spec) - pre/post conditions checked at runtime
spec square(x) {
    pre: x >= 0
    post: result >= 0
}

fn square(x) {
    return x * x
}

# 2. Invariant blocks - state consistency checks within loops
fn find_max(arr) {
    let i = 0
    while i < len(arr) - 1 {
        invariant arr {
            arr[i] >= 0
        }
        if arr[i] > arr[i + 1] {
            swap arr[i], arr[i + 1]
        }
        i = i + 1
    }
    return arr[len(arr) - 1]
}

# 3. Aspect-like hooks for cross-cutting concerns
# Before/after/around execute code around function calls

before square {
    print("About to square a number")
}

after square {
    print("Squared successfully")
}

# 4. Pointcut definitions for grouping join points
pointcut math_ops {
    square
}

around math_ops {
    print("Math operation wrapper")
    body
}

# 5. Runtime check assertions
fn main() {
    # Contract checking demo
    check square(5)
    print(square(5))
    
    # Invariant demo with array
    let data = [3, 1, 4, 1, 5, 9, 2, 6]
    let max = find_max(data)
    print("Max:", max)
    
    return 0
}