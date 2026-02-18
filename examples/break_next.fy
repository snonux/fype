# break exits the while loop early when i reaches 5
my i = 0;
while i < 10 {
    i = i + 1;
    if i == 5 { break; }
}
say i;          # expected: 5

# next skips adding j when j == 3, so sum = 1+2+4+5 = 12
my sum = 0;
my j = 0;
while j < 5 {
    j = j + 1;
    if j == 3 { next; }
    sum = sum + j;
}
say sum;        # expected: 12

# break inside an until loop stops when k reaches 7
my k = 0;
until k == 10 {
    k = k + 1;
    if k == 7 { break; }
}
say k;          # expected: 7
