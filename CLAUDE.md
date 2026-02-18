# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

**Uses GNU Make** (standard `make` on Fedora/Linux).

```sh
make           # Build the fype binary (increments build number automatically)
make clean     # Remove object files and binary
make install   # Install to /usr/local/bin and /usr/local/man/man1
make examples  # Run all .fy example scripts
```

Running Fype scripts:
```sh
./fype script.fy          # Run a .fy script file
./fype -e "say 1 + 2;"   # Run inline code
./fype -h                 # Show help / all options
./fype -v                 # Show version
./fype -V script.fy       # Verbose/debug mode
```

Code style check and formatting:
```sh
make style    # Run astyle formatter + check line lengths
make astyle   # Format all .c/.h files with astyle (3-space indent)
make check    # Check for lines exceeding 80 characters
```

## Architecture

Fype is a single-pass, simultaneous-parse-and-interpret scripting language. There is no AST; the scanner produces a flat token list which the interpreter consumes directly.

### Execution pipeline

1. **Scanner** (`src/core/scanner.c`) — reads source file or inline string, emits a `List` of `Token` objects
2. **Interpreter** (`src/core/interpret.c`) — walks the token list, dispatching to built-in or user-defined functions/procedures via the expression evaluator and `interpret_subprocess` for sub-blocks
3. **Functions dispatch** (`src/core/functions.c`, `src/core/function.c`) — resolves identifiers against the built-in function registry (`Functions`/`Hash`) or user-defined symbols in the `Scope`

### Key data structures

| File | Type | Role |
|---|---|---|
| `src/fype.h` | `Fype` | Top-level context: argv, global token list, global symbol hash |
| `src/core/token.h` | `Token` / `TokenType` | Leaf data unit; carries string/int/double value and a `TokenType` enum |
| `src/core/interpret.h` | `Interpret` | Per-invocation interpreter state: current token, scope, eval stack |
| `src/core/scope.h` | `Scope` | Lexical scoping: a global `Hash` plus a `Stack` of per-block hashes |
| `src/core/symbol.h` | `Symbol` / `SymbolType` | Symbol-table entry wrapping a value pointer (`SYM_VARIABLE`, `SYM_FUNCTION`, `SYM_PROCEDURE`, `SYM_ARRAY`, `SYM_BUILDIN`, `SYM_CONSTANT`) |
| `src/core/garbage.h` | GC | Reference-counting garbage collector for `Token` objects |
| `src/core/reference.c` | synonyms | Implements the `\identifier` synonym (alias) mechanism |
| `src/core/convert.c` | type coercion | Auto-conversion between integer, double, string |
| `src/data/` | containers | Reusable generic data structures: `List`, `Stack`, `Hash`, `Array`, `Map`, `Queue`, `Tree`, `Tupel` |

### Scope and symbol resolution

`Scope` maintains a global hash (`p_hash_global`) and a stack of local hashes pushed/popped by `scope_up`/`scope_down`. Symbol lookup walks from innermost to global. Procedures share the caller's scope; functions get their own scope frame.

### Token types

`TokenType` in `src/core/token.h` uses sentinel enum ranges (`START_TERMINALS`/`END_TERMINALS`, `START_KEYWORDS`/`END_KEYWORDS`, etc.) so membership tests are simple range comparisons (`IS_KEYWORD(t)`, `IS_OPERATOR(t)`, etc.).

### Debug flags

Uncomment defines in `src/defines.h` to enable tracing:
- `DEBUG_GC` — garbage collector activity
- `DEBUG_TOKEN_REFCOUNT` — token reference counting
- `DEBUG_FUNCTION_PROCESS` — function dispatch
- `DEBUG_TRACK` — interpreter step trace
- `DEBUG_BLOCK_GET` / `DEBUG_EXPRESSION_GET` — sub-parser tracing

## Coding Style

### File headers

Every source file begins with a license header using the special `/*:*` comment format:

```c
/*:*
 *: File: ./src/core/token.h
 *: A simple interpreter
 *: 
 *: WWW     : https://codeberg.org/snonux/fype
 *: AUTHOR  : http://buetow.org
 *: E-Mail  : fype at dev.buetow.org
 *: 
 *: Copyright (c) 2005 - 2008, Paul Buetow 
 *: All rights reserved.
 *: ...
 *:*/
```

### Naming conventions

| Category | Convention | Examples |
|----------|------------|----------|
| Types (structs, enums, typedefs) | `PascalCase` | `Token`, `TokenType`, `List`, `Scope` |
| Functions | `module_action` (snake_case) | `token_new`, `list_add_back`, `scope_get` |
| Variables | `prefix_name` with type prefix | `p_token` (pointer), `i_val` (int), `c_val` (char/string), `u_id` (unsigned) |
| Macros/constants | `UPPER_SNAKE_CASE` | `TT_INTEGER`, `DEBUG_GC`, `NO_DEFAULT` |
| Enum values | `MODULE_PREFIX_NAME` | `TT_INTEGER`, `TT_STRING`, `SYM_VARIABLE` |
| Callbacks | `name_cb` | `token_delete_cb`, `token_print_cb` |
| Static/private functions | `_prefix_name` | `_scope_get_hash`, `_list_copy_cb` |

### Formatting

- **Indentation**: 3 spaces (no tabs). Use `make astyle` to auto-format.
- **Line length**: Maximum 80 characters. Use `make check` to verify.
- **Braces**: K&R style (opening brace on same line as statement)
- **Pointer asterisk**: Attached to variable name, not type: `Token *p_token`, not `Token* p_token`
- **Return statements**: Parenthesize return values: `return (p_token);`

### Function definition style

Return type on its own line:

```c
Token*
token_new(char *c_val, TokenType tt_cur, int i_line_nr,
         int i_pos_nr, char *c_filename) {
   Token *p_token = token_new_dummy();
   // ...
   return (p_token);
}
```

### Header guards

Traditional `#ifndef` pattern:

```c
#ifndef TOKEN_H
#define TOKEN_H
// ...
#endif
```

### Comments

- Block comments: `/* ... */`
- Single-line notes: `// comment`
- No inline comments trailing code on the same line for significant explanations

