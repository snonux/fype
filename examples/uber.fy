#*
 * Student Score Report — Fype uber-example
 *
 * Analyses a class of ten exam scores (0-100) and produces a brief
 * statistical summary.  Every Fype language feature is exercised in
 * service of the analysis; all assertions must pass for the script
 * to exit cleanly.
 *#

# ─── DATASET ───────────────────────────────────────────────────────────────
# Ten student exam scores.  We will compute sum, average, band counts, etc.
my scores = [72, 45, 89, 91, 55, 78, 63, 100, 48, 82];
my N    = 10;   # number of students
my PASS = 60;   # minimum score for a pass
my DIST = 80;   # minimum score for a distinction

# ─── 1. COMMENTS ───────────────────────────────────────────────────────────

# Single-line comment — ignored.
#* Block comment — also ignored. *#

# Inline block comment inside an expression — the #* *# part is discarded:
my ok = 1 #* invisible *# + 0;
assert 1 == ok;

# ─── 2. BASIC ARITHMETIC AND VARIABLES ─────────────────────────────────────

# Multiple declarations on one line; band defaults to 0.
my a = 10, b = 3, band;
assert 0  == say band;
assert 13 == say a + b;
assert 7  == say a - b;
assert 30 == say a * b;
assert 3  == say a / b;             # integer division truncates toward zero
assert 10 == say 2 * (4 + 2) - 2;
assert 10 == say (8 / 2) + 2 * 3;

# Assignment is also an expression that returns the new value.
assert 15 == (a = 15);
assert 15 == say a;

# ─── 3. NEGATION AND NOT ───────────────────────────────────────────────────

assert 5 == say neg neg 5;
assert 0 == say neg neg 0;
assert 1 == say not 0;
assert 0 == say not 1;

# Temporarily penalise the first score by 5, then restore it.
my s0 = scores[0];                  # 72
s0 = s0 - 5;
assert 67 == say s0;
s0 = s0 + 5;
assert 72 == say s0;

# ─── 4. INCREMENT / DECREMENT ──────────────────────────────────────────────

# Used as statements:
my tally = 0;
incr tally; incr tally; incr tally;
assert 3 == say tally;
decr tally;
assert 2 == say tally;

# Used as expressions (return the new value):
my step = 0;
assert 1 == say incr step;
assert 0 == say decr step;

# ─── 5. TYPE CONVERSION ────────────────────────────────────────────────────

assert 7  == say integer 7.9;       # truncates toward zero
assert 80 == say double 80;         # 80.0 compares equal to 80
assert 14 == say 1 + string 13;     # "13" coerced to integer 13
assert 5  == say "10 pts" / 2;      # leading digits extracted first

# ─── 6. STRING ARITHMETIC ──────────────────────────────────────────────────

assert 46   == say "12" + "34";     # "12"->12  + "34"->34  = 46
assert 1231 == say "1234" - "3";    # 1234 - 3 = 1231

# ─── 7. COMPARISON OPERATORS ───────────────────────────────────────────────

assert 1 == (put scores[0] > PASS);  ln;   # 72 > 60  => 1
assert 0 == (put scores[1] > PASS);  ln;   # 45 > 60  => 0
assert 1 == (put scores[3] >= DIST); ln;   # 91 >= 80 => 1
assert 1 == (put PASS != DIST);      ln;
assert 0 == (say PASS == DIST);
assert 1 == (put 3 < 5);  assert 0 == (say 5 < 5);   ln;
assert 1 == (put 5 <= 5); assert 0 == (say 6 <= 5);   ln;

# ─── 8. BITWISE GRADE FLAGS ────────────────────────────────────────────────
# Encode quality: bit 0 = passed (>=PASS), bit 1 = distinction (>=DIST).

# Score 91 => passed AND distinction => 0b11 = 3
my g91 = 0;
if scores[3] >= PASS { g91 = g91 or 1; }
if scores[3] >= DIST { g91 = g91 or 2; }
assert 3 == say g91;

# Score 45 => failed => 0b00 = 0
my g45 = 0;
if scores[1] >= PASS { g45 = g45 or 1; }
assert 0 == say g45;

assert 1 == say (g91 and 1);        # extract pass bit from g91
assert 0 == say (g45 and 1);        # extract pass bit from g45
assert 6 == say (5 xor 3);          # 0b101 ^ 0b011 = 0b110 = 6
assert 4 == say (1 :< 2);           # 1 << 2 = 4
assert 2 == say (8 :> 2);           # 8 >> 2 = 2

# ─── 9. CONDITIONALS ───────────────────────────────────────────────────────

# 91 is a distinction
if scores[3] >= DIST { band = 2; }
assert 2 == say band;

# 45 is a fail
ifnot scores[1] >= PASS { band = 0; }
assert 0 == say band;

if 0    { band = 99; }              # must not execute
ifnot 1 { band = 99; }             # must not execute
assert 0 == say band;

# ─── 10. WHILE + UNTIL: SUM ALL SCORES ─────────────────────────────────────
# 72+45+89+91+55+78+63+100+48+82 = 723

my total = 0, wi = 0;
while wi < N {
    total = total + scores[wi];
    incr wi;
}
assert 723 == say total;

my total2 = 0, ui = 0;
until ui >= N {
    total2 = total2 + scores[ui];
    incr ui;
}
assert 723 == say total2;

# ─── 11. LOOP + BREAK + NEXT: COUNT PASSING SCORES ─────────────────────────
# Scores >= 60: 72,89,91,78,63,100,82 => 7 students

my passing = 0, li = 0;
loop {
    if li >= N            { break; }
    if scores[li] < PASS  { li = li + 1; next; }
    incr passing;
    incr li;
}
assert 7 == say passing;

# ─── 12. WHILE + BREAK + NEXT: COUNT DISTINCTIONS ──────────────────────────
# Scores >= 80: 89,91,100,82 => 4 students

my dists = 0, di = 0;
while di < N {
    incr di;
    if scores[di - 1] < DIST { next; }
    incr dists;
}
assert 4 == say dists;

# ─── 13. DO-WHILE / DO-UNTIL ───────────────────────────────────────────────

# Body runs once even though the condition is already false.
my dw = 10;
do { dw = dw + 1; } while dw < 10;
assert 11 == say dw;

# Accumulate scores until the running total exceeds 500 (runs at least once).
# After 8 iterations: 72+45+89+91+55+78+63+100 = 593.
my rsum = 0, ri = 0;
do {
    rsum = rsum + scores[ri];
    incr ri;
} while rsum < 500;
assert 593 == say rsum;
assert 8   == say ri;

# do-until: count down from 3 to 0.
my cd = 3;
do { decr cd; } until cd == 0;
assert 0 == say cd;

# ─── 14. FUNCTIONS ─────────────────────────────────────────────────────────

# Sum the first n elements of an array.
fun arr_sum(arr, n) {
    my s = 0, i = 0;
    while i < n {
        s = s + arr[i];
        incr i;
    }
    ret s;
}
assert 723 == say arr_sum(scores, N);

# Integer average (floor division).
fun avg(arr, n) {
    ret arr_sum(arr, n) / n;
}
assert 72 == say avg(scores, N);

# Conditional return: grade band (0=fail, 1=pass, 2=distinction).
fun grade_band(score) {
    if score >= DIST { ret 2; }
    if score >= PASS { ret 1; }
    ret 0;
}
assert 2 == say grade_band(91);
assert 1 == say grade_band(72);
assert 0 == say grade_band(45);

# Multiple return values: minimum and maximum of an array.
fun arr_minmax(arr, n) {
    my mn = arr[0], mx = arr[0], i = 1;
    while i < n {
        if arr[i] < mn { mn = arr[i]; }
        if arr[i] > mx { mx = arr[i]; }
        incr i;
    }
    ret mn, mx;                     # prints 45 then 100
}
say arr_minmax(scores, N);

# Nested function: range check (returns 1 if 0 <= score <= 100).
fun valid_score(score) {
    fun in_range(v) {
        if v >= 0 { if v <= 100 { ret 1; } }
        ret 0;
    }
    ret in_range(score);
}
assert 1 == say valid_score(72);
assert 0 == say valid_score(neg 1);
assert 0 == defined in_range;      # nested function gone after the call

# Old-style zero-arg function (no parentheses).
fun banner { say "=== Student Score Report ==="; }
banner;

# Self-undefining one-shot function.
fun init_once { say "Initialising report..."; undef init_once; }
init_once;
assert 0 == defined init_once;

# ─── 15. PROCEDURES ────────────────────────────────────────────────────────
# Procedures share the caller's scope; mutations are visible to the caller.

my accum = total;                   # start from the already-computed sum
proc apply_bonus { accum = accum + 10; }
apply_bonus; apply_bonus;
assert 743 == say accum;            # 723 + 20

# Variables declared inside a procedure leak into the caller's scope.
proc make_summary {
    my summary_pass = passing;
    my summary_dist = dists;
}
make_summary;
assert 7 == say summary_pass;
assert 4 == say summary_dist;

# ─── 16. SCOPING ───────────────────────────────────────────────────────────

my outer = 100;
{
    my inner = 50;
    outer = outer + inner;
    assert 1 == defined inner;
    scope;                          # prints all symbols visible here
}
assert 150 == say outer;
assert 0   == defined inner;       # inner vanished when the block closed

# Deep nesting: mutate a counter from three levels deep.
my depth = 0;
{
    incr depth;
    { incr depth; { incr depth; } }
}
assert 3 == say depth;

# ─── 17. SYNONYMS / ALIASES ────────────────────────────────────────────────

my best = scores[3];               # 91
my top  = \best;                   # top is a synonym for best
assert 91 == say top;

best = 95;                         # update through the original name
assert 95 == say top;              # synonym reflects the change

assert 2 == syms best;
undef best;
assert 1 == syms top;
assert 0 == defined best;
assert 1 == defined top;
undef top;

# Procedure synonym keeps the procedure alive after the original is undefined.
proc show_band { put "band="; say grade_band(91); }
my show_band2 = \show_band;
assert 2 == syms show_band;
show_band2;
undef show_band;
show_band2;                        # alias still callable
assert 0 == defined show_band;
undef show_band2;

# ─── 18. DEFINED / UNDEF ───────────────────────────────────────────────────

my tmp = 42;
assert 1 == defined tmp;
assert 0 == defined nosuchvar;
undef tmp;
assert 0 == defined tmp;

# ─── 19. ARRAYS — LITERALS, INDEXING, ASSIGNMENT ────────────────────────────

my extra = [5, 10, 15, 20, 25];
assert 5  == say len extra;
assert 5  == say extra[0];
assert 15 == say extra[2];

# Element assignment.
extra[2] = 99;
assert 99 == say extra[2];

# Expression-based index.
my ei = 1;
assert 10 == say extra[ei];
assert 99 == say extra[ei + 1];

# ─── 20. ARRAY SLICES ──────────────────────────────────────────────────────
# scores is [72, 45, 89, 91, 55, 78, 63, 100, 48, 82]

# Half-open slice: indices 2, 3, 4 => [89, 91, 55]
my mid = scores[2:5];
assert 3  == say len mid;
assert 89 == say mid[0];
assert 55 == say mid[2];

# Slice from start (low bound omitted): first three => [72, 45, 89]
my first3 = scores[:3];
assert 3  == say len first3;
assert 72 == say first3[0];
assert 89 == say first3[2];

# Slice to end (high bound omitted): from index 7 => [100, 48, 82]
my tail = scores[7:];
assert 3   == say len tail;
assert 100 == say tail[0];
assert 82  == say tail[2];

# Full shallow copy.
my copy = scores[:];
assert 10 == say len copy;
assert 72 == say copy[0];
assert 82 == say copy[9];

# ─── 21. I/O ───────────────────────────────────────────────────────────────

put "Total: ";     put total;      ln;
put "Average: ";   put avg(scores, N);  ln;
put "Passing: ";   put passing;    ln;
put "Distincts: "; say dists;

# ─── 22. FORK ──────────────────────────────────────────────────────────────
# Child process computes the same sum independently; parent waits.

my pid = fork;

if pid {
    put "Parent: child pid = "; say pid;
}

ifnot pid {
    put "Child: score sum = "; say arr_sum(scores, N);
    exit 0;
}

# ─── DONE ──────────────────────────────────────────────────────────────────
say "All assertions passed.";
