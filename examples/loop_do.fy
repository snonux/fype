# loop — infinite loop, break exits after 5 iterations
my i = 0;
loop {
    i = i + 1;
    if i == 5 { break; }
}
assert 5 == say i;          # expected: 5

# loop with next — skips j==3, so sum = 1+2+4+5 = 12
my sum = 0;
my j = 0;
loop {
    j = j + 1;
    if j > 5 { break; }
    if j == 3 { next; }
    sum = sum + j;
}
assert 12 == say sum;        # expected: 12

# do...while — body runs once even though k >= 10 already
my k = 10;
do {
    k = k + 1;
} while k < 10;
assert 11 == say k;          # expected: 11

# do...until — stops when m reaches 5
my m = 0;
do {
    m = m + 1;
} until m == 5;
assert 5 == say m;          # expected: 5
