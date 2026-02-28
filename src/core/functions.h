#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "token.h"

#include "interpret.h"
#include "../data/stack.h"
#include "../data/hash.h"

typedef struct {
   Hash *p_hash_functions;
} Functions;

Functions* functions_new();
void functions_delete(Functions *p_functions);
void functions_init(Functions *p_functions);

/* p_array_lhs / i_lhs_idx carry array-element LHS context for arr[i] = val;
 * pass NULL / 0 for plain assignment or non-assignment operators. */
void function_process(Interpret *p_interp, Token *p_token_op,
                      Token *p_token_op2, Stack *p_stack_args,
                      int i_args, Token *p_array_lhs, int i_lhs_idx);
_Bool function_is_buildin(Token *p_token_ident);
void function_process_buildin(Interpret *p_interpret,
                              Token *p_token_ident,
                              Stack *p_stack_args);
_Bool function_is_self_defined(Interpret *p_interpret);
void function_process_self_defined(Interpret *p_interpret,
                                   Token *p_token_ident);

#endif /* FUNCTIONS_H */
