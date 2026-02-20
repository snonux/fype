# Fype

**F**or **Y**our **P**rogram **E**xecution — a lightweight scripting language.

## Synopsis

```sh
fype script.fy           # Run a .fy file
fype -e "say 1 + 2;"     # Run inline code
fype -h                  # Show help
fype -v                  # Show version
```

## About

Fype is a single-pass, simultaneous-parse-and-interpret scripting language. There is no AST; the scanner produces a flat token list which the interpreter consumes directly.

Fype is developed under the BSD license.

## Building

```sh
make           # Build the fype binary
make install   # Install to /usr/local/bin
make clean     # Remove build artifacts
```

## Data Types

Fype uses automatic type conversion between these types:

- **integer** — whole numbers
- **double** — floating-point numbers
- **string** — text strings

Explicit conversion: `integer`, `double`, `string`.

## Syntax

### Comments

```fy
# Single-line comment

#* Block comment *#

#* Multi-line
   block comment *#

say 1 #* inline *# + 1;
```

### Variables

```fy
my foo = 1 + 2;       # declare and assign
my bar = 4, baz = 5;  # multiple declarations
my bay;               # defaults to 0
```

### Arithmetic

```fy
say 10 + 3;   # 13
say 10 - 3;   # 7
say 10 * 3;   # 30
say 10 / 3;   # 3 (integer division)
say neg 5;    # -5
incr x;       # x = x + 1
decr x;       # x = x - 1
```

### Comparison

```fy
5 == 5   # equal
5 != 4   # not equal
5 < 10   # less than
5 <= 5   # less or equal
5 > 3    # greater than
5 >= 5   # greater or equal
not 0    # logical not (returns 1)
```

### Bitwise

```fy
5 and 3   # bitwise AND
5 or 2    # bitwise OR
5 xor 3   # bitwise XOR
2 :< 2    # left shift (2 << 2 = 8)
8 :> 2    # right shift (8 >> 2 = 2)
```

### Conditionals

```fy
if 1 { say "true"; }
ifnot 0 { say "false"; }
```

### Loops

```fy
while x < 10 { incr x; }
until x == 10 { incr x; }
loop {
    if x == 5 { break; }
    incr x;
}
do { incr x; } while x < 10;
do { incr x; } until x == 10;
```

`break` exits the loop; `next` skips to the next iteration.

### Procedures

Procedures share the caller's scope:

```fy
proc greet {
    say "Hello";
    my local_var = 42;  # visible to caller
}
greet;
```

### Functions

Functions have local scope and support parameters and return values:

```fy
# Zero-arg function
fun answer() { ret 42; }
say answer();

# With parameters
fun add(a, b) { ret a + b; }
say add(3, 5);

# Multiple return values
fun minmax(a, b) {
    if a < b { ret a, b; }
    ret b, a;
}
say minmax(3, 7);  # prints 3 then 7
```

### Arrays

```fy
my nums = [10, 20, 30, 40, 50];
say len nums;      # 5
say nums[0];       # 10
say nums[1 + 1];   # 30 (expression index)
nums[2] = 99;      # element assignment

# Slices (half-open ranges)
my sub = nums[1:4];   # [20, 30, 40]
my head = nums[:2];   # [10, 20]
my tail = nums[3:];   # [40, 50]
my copy = nums[:];    # full shallow copy
```

### Scoping

```fy
my outer = 1;
{
    my inner = 2;
    say defined inner;  # 1
}
say defined inner;      # 0
```

### Synonyms (Aliases)

```fy
my foo = 42;
my bar = \foo;      # bar is an alias for foo
foo = 99;
say bar;            # 99 (reflects change)
say syms foo;       # 2 (two symbols point to same value)
undef foo;
say defined bar;    # 1 (alias keeps value alive)
```

### Built-in Functions

| Function | Description |
|----------|-------------|
| `put x` | Print without newline |
| `say x` | Print with newline |
| `ln` | Print newline |
| `len arr` | Array length |
| `defined id` | 1 if defined, else 0 |
| `undef id` | Undefine a symbol |
| `syms id` | Count synonyms |
| `assert cond` | Assert condition is true |
| `exit n` | Exit with code n |
| `fork` | Fork a subprocess |
| `gc` | Run garbage collector |
| `scope` | Print visible symbols |

## Examples

See the `examples/` directory for complete demonstrations:

- `variables.fy` — variable declarations
- `expressions.fy` — arithmetic expressions
- `conditionals.fy` — if/ifnot
- `control.fy` — while/until loops
- `loop_do.fy` — loop, break, next, do-while, do-until
- `functions.fy` — functions and nesting
- `func_args_ret.fy` — function arguments and return values
- `procedures.fy` — procedures
- `slices.fy` — array slicing
- `synonyms.fy` — aliases
- `scopeing.fy` — lexical scoping
- `bitwise.fy` — bitwise operators
- `types.fy` — type conversion
- `io.fy` — I/O operations
- `fork.fy` — process forking
- `comments.fy` — comment syntax
- `break_next.fy` — loop control
- `uber.fy` — comprehensive example exercising all features

## Author

Paul C. Buetow (https://buetow.org)

## Website

https://codeberg.org/snonux/fype
