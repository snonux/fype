# Test: function named arguments, explicit ret, and multiple return values

# zero-arg function with explicit return
fun answer() {
    ret 42;
}
assert 42 == say answer();

# single-arg function — factorial with a while loop and ret
fun factorial(n) {
    my result = 1;
    while n > 1 {
        result = result * n;
        decr n;
    }
    ret result;
}
assert 120 == say factorial(5);

# two-arg function
fun add(a, b) {
    ret a + b;
}
assert 8 == say add(3, 5);

# conditional return inside if
fun absval(n) {
    if n < 0 { ret neg n; }
    ret n;
}
assert 5 == say absval(5);
assert 5 == say absval(neg 5);

# multiple return values — both land on the caller's stack
fun minmax(a, b) {
    if a < b { ret a, b; }
    ret b, a;
}
say minmax(3, 7);

# old-style zero-arg function without parens still works
fun greet {
    say "hello";
}
greet;
