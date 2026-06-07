# Array slicing test
let arr = [1, 2, 3, 4, 5, 6, 7, 8]

# Test slice
let slice = arr[2..5]

# List comprehension test
let squares = [for i in 1..10 i * i]

# Destructuring test
let [a, b, ...rest] = arr

# Print results
print("slice len:", len(slice))
print("squares len:", len(squares))
print("a:", a)
print("b:", b)
print("rest len:", len(rest))
