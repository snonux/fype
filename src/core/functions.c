#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "functions.h"

#include "convert.h"
#include "scope.h"
#include "symbol.h"

#define _FUNCTIONS_ERROR(m,t) \
	 ERROR(\
		"%s: Function error in %s line %d pos %d near '%s'", m, \
		token_get_filename(t), \
		token_get_line_nr(t), \
		token_get_pos_nr(t), \
		token_get_val(t) \
		)

/* Built-in handler signature: interpret context, arg stack, calling token */
typedef void (*BuiltinFn)(Interpret *p_interpret,
                          Stack *p_stack_args,
                          Token *p_token_ident);

/* Dispatch table entry: built-in name + its handler */
typedef struct {
   const char *c_name;
   BuiltinFn   fn;
} BuiltinEntry;

/* ─── Individual built-in handler functions ──────────────────────────── */

static void
_builtin_assert(Interpret *p_interpret, Stack *p_stack_args,
                Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = stack_top(p_stack_args);
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      if (token_get_ival(p_token) == 0)
         _FUNCTIONS_ERROR("Assert failed", p_token);
      break;
   case TT_DOUBLE:
      if (token_get_dval(p_token) == 0)
         _FUNCTIONS_ERROR("Assert failed", p_token);
      break;
   case TT_STRING:
      if (atoi(token_get_val(p_token)) == 0)
         _FUNCTIONS_ERROR("Assert failed", p_token);
      break;
      NO_DEFAULT;
   }
}

static void
_builtin_decr(Interpret *p_interpret, Stack *p_stack_args,
              Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   /* Modify the top-of-stack token in place */
   Token *p_token = stack_top(p_stack_args);
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      token_set_ival(p_token, token_get_ival(p_token) - 1);
      break;
   case TT_DOUBLE:
      token_set_dval(p_token, token_get_dval(p_token) - 1);
      break;
   case TT_STRING:
      convert_to_integer(p_token);
      token_set_ival(p_token, token_get_ival(p_token) - 1);
      break;
      NO_DEFAULT;
   }
}

static void
_builtin_double(Interpret *p_interpret, Stack *p_stack_args,
                Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   convert_to_double(p_token);
   stack_push(p_stack_args, p_token);
}

static void
_builtin_end(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   (void) p_interpret;
   (void) p_stack_args;
   (void) p_token_ident;
   exit(0);
}

static void
_builtin_exit(Interpret *p_interpret, Stack *p_stack_args,
              Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_top(p_stack_args));
   convert_to_integer(p_token);
   exit(token_get_ival(p_token));
}

static void
_builtin_fork(Interpret *p_interpret, Stack *p_stack_args,
              Token *p_token_ident) {
   (void) p_interpret;
   (void) p_token_ident;
   stack_push(p_stack_args, token_new_integer((int) fork()));
}

static void
_builtin_gc(Interpret *p_interpret, Stack *p_stack_args,
            Token *p_token_ident) {
   (void) p_interpret;
   (void) p_token_ident;
   stack_push(p_stack_args, token_new_integer(garbage_collect()));
}

static void
_builtin_incr(Interpret *p_interpret, Stack *p_stack_args,
              Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   /* Modify the top-of-stack token in place */
   Token *p_token = stack_top(p_stack_args);
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      token_set_ival(p_token, token_get_ival(p_token) + 1);
      break;
   case TT_DOUBLE:
      token_set_dval(p_token, token_get_dval(p_token) + 1);
      break;
   case TT_STRING:
      convert_to_integer(p_token);
      token_set_ival(p_token, token_get_ival(p_token) + 1);
      break;
      NO_DEFAULT;
   }
}

static void
_builtin_ind(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   /* Array case (ind on TT_ARRAY) is handled by the array dispatcher;
    * reaching here means the argument was not an array. */
   (void) p_interpret;
   (void) p_stack_args;
   _FUNCTIONS_ERROR("Expected array", p_token_ident);
}

static void
_builtin_integer(Interpret *p_interpret, Stack *p_stack_args,
                 Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   convert_to_integer(p_token);
   stack_push(p_stack_args, p_token);
}

static void
_builtin_len(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   /* Convert to string to measure length, then return as integer */
   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   convert_to_string(p_token);
   token_set_tt(p_token, TT_INTEGER);
   token_set_ival(p_token, strlen(token_get_val(p_token)));
   stack_push(p_stack_args, p_token);
}

static void
_builtin_ln(Interpret *p_interpret, Stack *p_stack_args,
            Token *p_token_ident) {
   (void) p_interpret;
   (void) p_stack_args;
   (void) p_token_ident;
   printf("\n");
}

static void
_builtin_neg(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   stack_push(p_stack_args, p_token);
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      token_set_ival(p_token, -token_get_ival(p_token));
      break;
   case TT_DOUBLE:
      token_set_dval(p_token, -token_get_dval(p_token));
      break;
   case TT_STRING:
      token_set_ival(p_token, -atoi(token_get_val(p_token)));
      token_set_tt(p_token, TT_INTEGER);
      break;
      NO_DEFAULT;
   }
}

static void
_builtin_no(Interpret *p_interpret, Stack *p_stack_args,
            Token *p_token_ident) {
   /* 'no' with no args pushes integer 0; otherwise logical NOT of arg */
   (void) p_interpret;
   (void) p_token_ident;
   Token *p_token = NULL;
   if (0 == stack_size(p_stack_args)) {
      p_token = token_new_integer(0);
   } else {
      p_token = token_new_copy(stack_pop(p_stack_args));
      switch (token_get_tt(p_token)) {
      case TT_INTEGER:
         token_set_ival(p_token, !token_get_ival(p_token));
         break;
      case TT_DOUBLE:
         token_set_dval(p_token, !token_get_dval(p_token));
         break;
      case TT_STRING:
         token_set_ival(p_token, !atoi(token_get_val(p_token)));
         token_set_tt(p_token, TT_INTEGER);
         break;
         NO_DEFAULT;
      }
   }
   stack_push(p_stack_args, p_token);
}

static void
_builtin_not(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   /* 'not' requires an argument and performs logical NOT */
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   stack_push(p_stack_args, p_token);
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      token_set_ival(p_token, !token_get_ival(p_token));
      break;
   case TT_DOUBLE:
      token_set_dval(p_token, !token_get_dval(p_token));
      break;
   case TT_STRING:
      token_set_ival(p_token, !atoi(token_get_val(p_token)));
      token_set_tt(p_token, TT_INTEGER);
      break;
      NO_DEFAULT;
   }
}

static void
_builtin_put(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   /* Print all stack items without a trailing newline */
   (void) p_interpret;
   (void) p_token_ident;
   StackIterator *p_iter = stackiterator_new(p_stack_args);
   while (stackiterator_has_next(p_iter)) {
      Token *p_token = stackiterator_next(p_iter);
      switch (token_get_tt(p_token)) {
      case TT_INTEGER:
         printf("%d", token_get_ival(p_token));
         break;
      case TT_DOUBLE:
         printf("%f", token_get_dval(p_token));
         break;
      case TT_STRING:
         printf("%s", token_get_val(p_token));
         break;
         NO_DEFAULT;
      }
   }
   stackiterator_delete(p_iter);
}

static void
_builtin_refs(Interpret *p_interpret, Stack *p_stack_args,
              Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token_top = stack_pop(p_stack_args);
   stack_push(p_stack_args,
              token_new_integer(p_token_top->i_ref_count));
}

static void
_builtin_say(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   /* Print all stack items followed by a newline */
   (void) p_interpret;
   (void) p_token_ident;
   StackIterator *p_iter = stackiterator_new(p_stack_args);
   while (stackiterator_has_next(p_iter)) {
      Token *p_token = stackiterator_next(p_iter);
      switch (token_get_tt(p_token)) {
      case TT_INTEGER:
         printf("%d", token_get_ival(p_token));
         break;
      case TT_DOUBLE:
         printf("%f", token_get_dval(p_token));
         break;
      case TT_STRING:
         printf("%s", token_get_val(p_token));
         break;
         NO_DEFAULT;
      }
   }
   stackiterator_delete(p_iter);
   printf("\n");
}

static void
_builtin_scope(Interpret *p_interpret, Stack *p_stack_args,
               Token *p_token_ident) {
   (void) p_stack_args;
   (void) p_token_ident;
   scope_print(p_interpret->p_scope);
}

static void
_builtin_string(Interpret *p_interpret, Stack *p_stack_args,
                Token *p_token_ident) {
   (void) p_interpret;
   if (0 == stack_size(p_stack_args))
      _FUNCTIONS_ERROR("No argument given", p_token_ident);

   Token *p_token = token_new_copy(stack_pop(p_stack_args));
   convert_to_string(p_token);
   stack_push(p_stack_args, p_token);
}

static void
_builtin_yes(Interpret *p_interpret, Stack *p_stack_args,
             Token *p_token_ident) {
   /* 'yes' with no args pushes integer 1; otherwise forces arg to 1 */
   (void) p_interpret;
   (void) p_token_ident;
   Token *p_token = NULL;
   if (0 == stack_size(p_stack_args)) {
      p_token = token_new_integer(1);
   } else {
      p_token = token_new_copy(stack_pop(p_stack_args));
      token_set_ival(p_token, 1);
      token_set_tt(p_token, TT_INTEGER);
   }
   stack_push(p_stack_args, p_token);
}

/* ─── Dispatch table: maps built-in name to handler (NULL-terminated) ─── */

static const BuiltinEntry g_builtins[] = {
   { "assert",  _builtin_assert  },
   { "decr",    _builtin_decr    },
   { "double",  _builtin_double  },
   { "end",     _builtin_end     },
   { "exit",    _builtin_exit    },
   { "fork",    _builtin_fork    },
   { "gc",      _builtin_gc      },
   { "incr",    _builtin_incr    },
   { "ind",     _builtin_ind     },
   { "integer", _builtin_integer },
   { "len",     _builtin_len     },
   { "ln",      _builtin_ln      },
   { "neg",     _builtin_neg     },
   { "no",      _builtin_no      },
   { "not",     _builtin_not     },
   { "put",     _builtin_put     },
   { "refs",    _builtin_refs    },
   { "say",     _builtin_say     },
   { "scope",   _builtin_scope   },
   { "string",  _builtin_string  },
   { "yes",     _builtin_yes     },
   { NULL, NULL }
};

/* ─── Functions struct lifecycle ─────────────────────────────────────── */

Functions*
functions_new() {
   Functions *p_functions = malloc(sizeof(Functions));
   p_functions->p_hash_functions = hash_new(1024);
   functions_init(p_functions);
   return (p_functions);
}

void
functions_delete(Functions *p_functions) {
   hash_delete(p_functions->p_hash_functions);
   free(p_functions);
}

/* ─── Operator processing ────────────────────────────────────────────── */

/* Resolve a two-token compound operator into its canonical single TokenType.
 * For example, '!' + '=' becomes TT_NEQ, ':' + '<' becomes TT_LSHIFT.
 * Returns tt_op unchanged when no compound match is found. */
static TokenType
_resolve_composite_op(TokenType tt_op, TokenType tt_op2) {
   switch (tt_op) {
   case TT_NOT:
      if (tt_op2 == TT_ASSIGN) return (TT_NEQ);
      break;
   case TT_ASSIGN:
      if (tt_op2 == TT_ASSIGN) return (TT_EQ);
      break;
   case TT_LT:
      if (tt_op2 == TT_ASSIGN) return (TT_LE);
      break;
   case TT_GT:
      if (tt_op2 == TT_ASSIGN) return (TT_GE);
      break;
   case TT_DDOT:
      if (tt_op2 == TT_LT) return (TT_LSHIFT);
      if (tt_op2 == TT_GT) return (TT_RSHIFT);
      break;
   default:
      break;
   }
   return (tt_op);
}

/* Perform variable or array-element assignment.
 * If p_interpret->p_token_array_lhs is set, writes into that array slot;
 * otherwise writes into the named variable in p_interpret->p_token_temp. */
/* Perform variable or array-element assignment.
 * p_array_lhs and i_lhs_idx are passed explicitly so this function does
 * not need to reach into the Interpret struct for array-LHS state. */
static void
_op_assign(Interpret *p_interpret, Token *p_token_store,
           Token *p_array_lhs, int i_lhs_idx) {

   /* Array element assignment: arr[i] = val */
   if (p_array_lhs != NULL) {
      array_set(p_array_lhs->p_array, i_lhs_idx, p_token_store);
      return;
   }

   /* Regular variable assignment */
   Token *p_token_assign = p_interpret->p_token_temp;
   TokenType tt_assign = token_get_tt(p_token_assign);

   if (tt_assign != TT_IDENT) {
      _FUNCTIONS_ERROR("Can only assign to symbols", p_token_store);
   }

   Symbol *p_symbol = scope_get(p_interpret->p_scope,
                                token_get_val(p_token_assign));

   if (p_symbol == NULL) {
      _FUNCTIONS_ERROR("No such symbol", p_token_assign);
   }

   symbol_set_val(p_symbol, p_token_store);
   symbol_set_sym(p_symbol, SYM_VARIABLE);
}

/* Addition: store = next + store (integers, doubles, strings, array append) */
static void
_op_add(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     token_get_ival(p_next) + token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_dval(p_store,
                     token_get_dval(p_next) + token_get_dval(p_store));
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) +
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      array_append(p_store->p_array, p_next->p_array);
      break;
      NO_DEFAULT;
   }
}

/* Subtraction: store = next - store */
static void
_op_sub(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     token_get_ival(p_next) - token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_dval(p_store,
                     token_get_dval(p_next) - token_get_dval(p_store));
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) -
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY - TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Multiplication: store = next * store */
static void
_op_mult(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     token_get_ival(p_next) * token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_dval(p_store,
                     token_get_dval(p_next) * token_get_dval(p_store));
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) *
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY * TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Division: store = next / store */
static void
_op_div(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) /
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_dval(p_store,
                     token_get_dval(p_next) / token_get_dval(p_store));
      break;
   case TT_STRING:
      token_set_dval(p_store,
                     atof(token_get_val(p_next)) /
                     atof(token_get_val(p_store)));
      token_set_tt(p_store, TT_DOUBLE);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY / TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Equality: store = (next == store) — result is always TT_INTEGER */
static void
_op_eq(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) ==
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) == token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) == 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY eq TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Inequality: store = (next != store) — result is always TT_INTEGER */
static void
_op_neq(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) !=
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) != token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) != 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY != TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Less-than: store = (next < store) — result is always TT_INTEGER */
static void
_op_lt(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) <
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) < token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) < 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY < TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Greater-than: store = (next > store) — result is always TT_INTEGER */
static void
_op_gt(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) >
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) > token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) > 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY > TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Less-or-equal: store = (next <= store) — result is always TT_INTEGER */
static void
_op_le(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) <=
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) <= token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) <= 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY <= TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Greater-or-equal: store = (next >= store) — result is always TT_INTEGER */
static void
_op_ge(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) >=
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     token_get_dval(p_next) >= token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     strcmp(token_get_val(p_next),
                            token_get_val(p_store)) >= 0);
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY >= TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Bitwise AND: store = next & store — doubles and strings coerced to int */
static void
_op_and(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) &
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     (int) token_get_dval(p_next) &
                     (int) token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) &
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY & TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Bitwise OR: store = next | store — doubles and strings coerced to int */
static void
_op_or(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) |
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     (int) token_get_dval(p_next) |
                     (int) token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) |
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY | TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Bitwise XOR: store = next ^ store — doubles and strings coerced to int */
static void
_op_xor(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) ^
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     (int) token_get_dval(p_next) ^
                     (int) token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) ^
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY ^ TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Left shift: store = next << store — doubles and strings coerced to int */
static void
_op_lshift(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) <<
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     (int) token_get_dval(p_next) <<
                     (int) token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) <<
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY << TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Right shift: store = next >> store — doubles and strings coerced to int */
static void
_op_rshift(Token *p_store, Token *p_next, TokenType tt_highest) {
   switch (tt_highest) {
   case TT_INTEGER:
      token_set_ival(p_store,
                     (int) token_get_ival(p_next) >>
                     token_get_ival(p_store));
      break;
   case TT_DOUBLE:
      token_set_ival(p_store,
                     (int) token_get_dval(p_next) >>
                     (int) token_get_dval(p_store));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_STRING:
      token_set_ival(p_store,
                     atoi(token_get_val(p_next)) >>
                     atoi(token_get_val(p_store)));
      token_set_tt(p_store, TT_INTEGER);
      break;
   case TT_ARRAY:
      ERROR("TT_ARRAY >> TT_ARRAY not yet implemented");
      break;
      NO_DEFAULT;
   }
}

/* Dispatch to the correct per-operator handler.
 * 1. Resolve composite two-token operators (!=, ==, <=, >=, <<, >>).
 * 2. Handle assignment early — it does not need type conversion.
 * 3. Coerce both operands to their highest shared type.
 * 4. Call the matching _op_*() function for the resolved operator. */
/* Dispatch to the correct per-operator handler.
 * p_array_lhs / i_lhs_idx carry array-element assignment context when
 * the LHS is arr[i]; both are NULL/0 for plain variable assignment or
 * any non-assignment operator.
 * 1. Resolve composite two-token operators (!=, ==, <=, >=, <<, >>).
 * 2. Handle assignment early — it does not need type conversion.
 * 3. Coerce both operands to their highest shared type.
 * 4. Call the matching _op_*() function for the resolved operator. */
void
_process(Interpret *p_interpret, Token *p_token_store, Token *p_token_op,
         Token *p_token_op2, Token *p_token_next,
         Token *p_array_lhs, int i_lhs_idx) {

   TokenType tt_op = token_get_tt(p_token_op);
   TokenType tt_op2 = p_token_op2 == NULL
                      ? TT_NONE
                      : token_get_tt(p_token_op2);

#ifdef DEBUG_FUNCTION_PROCESS
   if (p_token_op2 == NULL)
      printf("DEBUG::FUNCTION::PROCESS: Operator %s\n",
             tt_get_name(tt_op));
   else
      printf("DEBUG::FUNCTION::PROCESS: Operator %s %s\n",
             tt_get_name(tt_op), tt_get_name(tt_op2));

   token_print(p_token_next);
   printf("\n");
   token_print(p_token_store);
   printf("\n");
#endif /* DEBUG_FUNCTION_PROCESS */

   /* Resolve two-token operators (!=, ==, <=, >=, <<, >>).
    * When p_token_op2 is NULL and operator is TT_ASSIGN, handle assignment
    * directly — it does not need type conversion and must not fall through. */
   if (p_token_op2 != NULL)
      tt_op = _resolve_composite_op(tt_op, tt_op2);
   else if (tt_op == TT_ASSIGN) {
      _op_assign(p_interpret, p_token_store, p_array_lhs, i_lhs_idx);
      return;
   }

   p_token_next = token_new_copy(p_token_next);
   TokenType tt_highest = convert_to_highest(p_token_store, p_token_next);

#ifdef DEBUG_FUNCTION_PROCESS
   printf("DEBUG::FUNCTION::PROCESS: ===> %s %s %s\n",
          tt_get_name(tt_highest), tt_get_name(tt_op),
          tt_get_name(tt_highest));
#endif /* DEBUG_FUNCTION_PROCESS */

   switch (tt_op) {
   case TT_ADD:    _op_add(p_token_store, p_token_next, tt_highest);    break;
   case TT_SUB:    _op_sub(p_token_store, p_token_next, tt_highest);    break;
   case TT_MULT:   _op_mult(p_token_store, p_token_next, tt_highest);   break;
   case TT_DIV:    _op_div(p_token_store, p_token_next, tt_highest);    break;
   case TT_EQ:     _op_eq(p_token_store, p_token_next, tt_highest);     break;
   case TT_NEQ:    _op_neq(p_token_store, p_token_next, tt_highest);    break;
   case TT_LT:     _op_lt(p_token_store, p_token_next, tt_highest);     break;
   case TT_GT:     _op_gt(p_token_store, p_token_next, tt_highest);     break;
   case TT_LE:     _op_le(p_token_store, p_token_next, tt_highest);     break;
   case TT_GE:     _op_ge(p_token_store, p_token_next, tt_highest);     break;
   case TT_AND:    _op_and(p_token_store, p_token_next, tt_highest);    break;
   case TT_OR:     _op_or(p_token_store, p_token_next, tt_highest);     break;
   case TT_XOR:    _op_xor(p_token_store, p_token_next, tt_highest);    break;
   case TT_LSHIFT: _op_lshift(p_token_store, p_token_next, tt_highest); break;
   case TT_RSHIFT: _op_rshift(p_token_store, p_token_next, tt_highest); break;
   default:
      _FUNCTIONS_ERROR("No such function/operator", p_token_op);
   }

#ifdef DEBUG_FUNCTION_PROCESS
   token_print(p_token_store);
   printf("\n\n");
#endif /* DEBUG_FUNCTION_PROCESS */

   token_delete(p_token_next);
}

/* Drive p_stack_args through the given operator, consuming i_args operands.
 * p_array_lhs / i_lhs_idx are forwarded to _process() so that array-element
 * assignment (arr[i] = val) can be dispatched without touching Interpret
 * internals.  Pass NULL / 0 for normal (non-array-LHS) expressions. */
void
function_process(Interpret *p_interpret, Token *p_token_op,
                 Token *p_token_op2, Stack *p_stack_args, int i_args,
                 Token *p_array_lhs, int i_lhs_idx) {

   Token *p_token_store = token_new_copy(stack_pop(p_stack_args));

   for (int i = 0; i < i_args - 1 && !stack_empty(p_stack_args); ++i) {
      Token *p_token_next = stack_pop(p_stack_args);

      _process(p_interpret, p_token_store, p_token_op,
               p_token_op2, p_token_next, p_array_lhs, i_lhs_idx);
   }

   stack_push(p_stack_args, p_token_store);
}

/* ─── Built-in function public API ──────────────────────────────────── */

_Bool
function_is_buildin(Token *p_token_ident) {
   /* Look up the token's name in the dispatch table */
   const char *c_name = token_get_val(p_token_ident);
   for (int i = 0; g_builtins[i].c_name != NULL; ++i) {
      if (strcmp(g_builtins[i].c_name, c_name) == 0)
         return (true);
   }
   return (false);
}

void
function_process_buildin(Interpret *p_interpret, Token *p_token_ident,
                         Stack *p_stack_args) {

   /* Special array dispatch: apply len/ind to the whole array, or
    * iterate element-wise for all other built-ins. */
   Token *p_token = stack_top(p_stack_args);
   if (token_get_tt(p_token) == TT_ARRAY) {
      const char *c_name = token_get_val(p_token_ident);
      if (strcmp("len", c_name) == 0) {
         stack_pop(p_stack_args);
         stack_push(p_stack_args,
                    token_new_integer(array_get_used(p_token->p_array)));

      } else if (strcmp("ind", c_name) == 0) {
         stack_pop(p_stack_args);
         stack_push(p_stack_args,
                    token_new_integer(array_get_ind(p_token->p_array)));

      } else {
         /* Apply built-in element-wise over every array element */
         ArrayIterator *p_iter = arrayiterator_new(p_token->p_array);
         while (arrayiterator_has_next(p_iter)) {
            stack_push(p_stack_args, arrayiterator_next(p_iter));
            function_process_buildin(p_interpret, p_token_ident,
                                     p_stack_args);
            stack_pop(p_stack_args);
         }
         arrayiterator_delete(p_iter);
      }
      return;
   }

   /* Dispatch to the named built-in handler via the table */
   const char *c_name = token_get_val(p_token_ident);
   for (int i = 0; g_builtins[i].c_name != NULL; ++i) {
      if (strcmp(g_builtins[i].c_name, c_name) == 0) {
         g_builtins[i].fn(p_interpret, p_stack_args, p_token_ident);
         return;
      }
   }
}

_Bool
function_is_self_defined(Interpret *p_interpret) {
   Symbol *p_symbol = scope_get(p_interpret->p_scope,
                                token_get_val(p_interpret->p_token));

   if (p_symbol == NULL)
      return (false);

   switch (symbol_get_sym(p_symbol)) {
   case SYM_PROCEDURE:
   case SYM_FUNCTION:
      return (true);
      NO_DEFAULT;
   }

   return (false);
}

void
function_process_self_defined(Interpret *p_interpret, Token *p_token_ident) {
   Symbol *p_symbol = scope_get(p_interpret->p_scope,
                                token_get_val(p_token_ident));

   switch (symbol_get_sym(p_symbol)) {
   case SYM_PROCEDURE:
   {
      List *p_list_token = symbol_get_val(p_symbol);
      interpret_subprocess(p_interpret, p_list_token);
   }
   break;
   case SYM_FUNCTION:
   {
      FuncDef *p_funcdef = (FuncDef*) symbol_get_val(p_symbol);
      scope_up(p_interpret->p_scope);

      /* Bind each named parameter to its argument value.
       * Args were pushed left-to-right so the last arg is on top (LIFO).
       * Reverse-fill a temp array so index 0 = first param = first arg. */
      if (p_funcdef->i_nparams > 0) {
         Token **pp_args = malloc(
            sizeof(Token*) * p_funcdef->i_nparams);

         for (int i = p_funcdef->i_nparams - 1; i >= 0; --i)
            pp_args[i] = stack_pop(p_interpret->p_stack);

         ListIterator *p_iter = listiterator_new(p_funcdef->p_params);

         for (int i = 0; i < p_funcdef->i_nparams; ++i) {
            char *c_name = (char*) listiterator_next(p_iter);
            Symbol *p_sym = symbol_new(SYM_VARIABLE, pp_args[i]);
            scope_newset(p_interpret->p_scope, c_name, p_sym);
         }

         listiterator_delete(p_iter);
         free(pp_args);
      }

      interpret_subprocess(p_interpret, p_funcdef->p_body);

      /* Consume CONTROL_RET here; return value(s) are already on the
       * stack (merged by interpret_subprocess when CONTROL_RET was
       * active). */
      if (p_interpret->ct == CONTROL_RET)
         p_interpret->ct = CONTROL_NONE;

      scope_down(p_interpret->p_scope);
   }
   NO_DEFAULT;
   }
}

void
functions_init(Functions *p_functions) {
   (void) p_functions;
}
