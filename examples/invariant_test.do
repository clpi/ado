# Test invariant feature

fn main() {
    let i = 0
    while i < 3 {
        invariant i {
            i >= 0
        }
        i = i + 1
    }
    print("Done")
    return 0
}