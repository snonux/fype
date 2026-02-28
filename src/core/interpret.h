#ifndef INTERPRET_H
#define INTERPRET_H

#include "../data/list.h"
#include "../data/stack.h"
#include "../data/hash.h"

#include "garbage.h"
#include "scope.h"
#include "token.h"

typedef enum {
   CONTROL_NONE,
   CONTROL_NEXT,
   CONTROL_BREAK,
   CONTROL_RET,   /* explicit return from a function */
} ControlType;

typedef struct {
   List *p_list_token;
   Scope *p_scope;
   _Bool b_scope_delete;
   Stack *p_stack;
   ControlType ct;

   ListIterator *p_iter;
   Token *p_token;
   TokenType tt;
   Token *p_token_prev;
   TokenType tt_prev;
   Token *p_token_temp;
   /* LHS array token set by _term_array_access for arr[i] = val */
   Token *p_token_array_lhs;
   /* Array index set alongside p_token_array_lhs */
   int    i_array_lhs_index;
} Interpret;

Interpret* interpret_new(List *p_list_token, Hash *p_hash_syms);
void interpret_delete(Interpret *p_interpret);
void interpret_run(List *p_list_token, Hash *p_hash_syms);
int interpret_process(Interpret *p_interpret);
int interpret_subprocess(Interpret *p_interpret, List *p_list_token);

#endif /* INTERPRET_H */
