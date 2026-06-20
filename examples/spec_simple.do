# Spec-Oriented Programming Demo - Check and Invariant

fn main() {
    # Check: runtime assertion
    check 5 > 0
    
    # Invariant: loop invariant that runs each iteration
    let i = 0
    while i < 10 {
        invariant i >= 0    # Must remain true
        i = i + 1
    }
    
    print("All checks passed!")
    return 0
}