# Test array index expressions and slice notation

my a = [10, 20, 30, 40, 50];

# Expression-based index
my i = 2;
assert 30 == say a[i];
assert 40 == say a[i + 1];

# Array element assignment
a[1] = 99;
assert 99 == say a[1];

# Basic slice (half-open range: end index is exclusive)
my sub = a[1:4];
assert 3 == say len sub;
assert 99 == say sub[0];
assert 30 == say sub[1];
assert 40 == say sub[2];

# Slice from start (low bound omitted)
my head = a[:2];
assert 2 == say len head;
assert 10 == say head[0];

# Slice to end (high bound omitted)
my tail = a[3:];
assert 2 == say len tail;
assert 40 == say tail[0];

# Full copy
my copy = a[:];
assert 5 == say len copy;
