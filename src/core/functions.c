/*:*
 *: File: ./src/core/functions.c
 *: A simple interpreter
 *: 
 *: AUTHOR	: Paul Buetow http://buetow.org
 *: E-Mail	: fype at dev.buetow.org
 *: 
 *: Copyright (c) 2005 - 2008, Paul Buetow 
 *: All rights reserved.
 *: 
 *: Redistribution and use in source and binary forms, with or without modi-
 *: fication, are permitted provided that the following conditions are met:
 *:  * Redistributions of source code must retain the above copyright
 *:    notice, this list of conditions and the following disclaimer.
 *:  * Redistributions in binary form must reproduce the above copyright
 *:    notice, this list of conditions and the following disclaimer in the
 *:    documentation and/or other materials provided with the distribution.
 *:  * Neither the name of P. B. Labs nor the names of its contributors may 
 *:    be used to endorse or promote products derived from this software 
 *:    without specific prior written permission.
 *: 
 *: THIS SOFTWARE IS PROVIDED BY PAUL C. BUETOW AS IS'' AND ANY EXPRESS OR 
 *: IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *: WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *: DISCLAIMED. IN NO EVENT SHALL PAUL C. BUETOW BE LIABLE FOR ANY DIRECT, 
 *: INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *: (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *:  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *: HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *: STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 *: IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *: POSSIBILITY OF SUCH DAMAGE.
 *:*/

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


void
_process(Interpret *p_interpret, Token *p_token_store, Token *p_token_op,
         Token *p_token_op2, Token *p_token_next) {

   TokenType tt_op = token_get_tt(p_token_op);
   TokenType tt_op2 = p_token_op2 == NULL
                      ? TT_NONE
                      : token_get_tt(p_token_op2);

#ifdef DEBUG_FUNCTION_PROCESS
   if (p_token_op2 == NULL)
      printf("DEBUG::FUNCTION::PROCESS: Operator %s\n", tt_get_name(tt_op));
   else
      printf("DEBUG::FUNCTION::PROCESS: Operator %s %s\n", tt_get_name(tt_op),
             tt_get_name(tt_op2));

   token_print(p_token_next);
   printf("\n");
   token_print(p_token_store);
   printf("\n");
#endif /* DEBUG_FUNCTION_PROCESS */

   if (p_token_op2 != NULL) {
      switch (tt_op) {
      case TT_NOT:
         switch (tt_op2) {
         case TT_ASSIGN:
            tt_op = TT_NEQ;
            break;
         default:
            break;
         }
         break;
      case TT_ASSIGN:
         switch (tt_op2) {
         case TT_ASSIGN:
            tt_op = TT_EQ;
            break;
         default:
            break;
         }
         break;
      case TT_LT:
         switch (tt_op2) {
         case TT_ASSIGN:
            tt_op = TT_LE;
            break;
         default:
            break;
         }
         break;
      case TT_GT:
         switch (tt_op2) {
         case TT_ASSIGN:
            tt_op = TT_GE;
            break;
         default:
            break;
         }
         break;
      case TT_DDOT:
         switch (tt_op2) {
         case TT_LT:
            tt_op = TT_LSHIFT;
            break;
         case TT_GT:
            tt_op = TT_RSHIFT;
            break;
         default:
            break;
         }
         break;
      default:
         break;
      }
   } else {
      switch (tt_op) {
      case TT_ASSIGN:
      {
         Token *p_token_assign = p_interpret->p_token_temp;
         TokenType tt_assign = token_get_tt(p_token_assign);

         if (tt_assign != TT_IDENT) {
            _FUNCTIONS_ERROR("Can only assign to symbols",
                             p_token_store);
         }

         Symbol *p_symbol = scope_get(p_interpret->p_scope,
                                      token_get_val(p_token_assign));

         if (p_symbol == NULL) {
            _FUNCTIONS_ERROR("No such symbol",
                             p_token_assign);
         }

         symbol_set_val(p_symbol, p_token_store);
         symbol_set_sym(p_symbol, SYM_VARIABLE);

         return;
      }

      break;
      NO_DEFAULT;
      }
   }

   p_token_next = token_new_copy(p_token_next);
   TokenType tt_highest = convert_to_highest(p_token_store, p_token_next);

#ifdef DEBUG_FUNCTION_PROCESS
   printf("DEBUG::FUNCTION::PROCESS: ===> %s %s %s\n",
          tt_get_name(tt_highest),
          tt_get_name(tt_op),
          tt_get_name(tt_highest));
#endif /* DEBUG_FUNCTION_PROCESS */

   switch (tt_op) {
   case TT_ADD:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        token_get_ival(p_token_next) +
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_dval(p_token_store,
                        token_get_dval(p_token_next) +
                        token_get_dval(p_token_store));
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) +
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         array_append(p_token_store->p_array,
                      p_token_next->p_array);
         break;
         NO_DEFAULT;
      }
      break;
   case TT_SUB:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        token_get_ival(p_token_next) -
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_dval(p_token_store,
                        token_get_dval(p_token_next) -
                        token_get_dval(p_token_store));
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) -
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("TT_ARRAY - TT_ARRAY not yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_MULT:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        token_get_ival(p_token_next) *
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_dval(p_token_store,
                        token_get_dval(p_token_next) *
                        token_get_dval(p_token_store));
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) *
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("TT_ARRAY * TT_ARRAY not yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_DIV:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) /
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_dval(p_token_store,
                        token_get_dval(p_token_next) /
                        token_get_dval(p_token_store));
         break;
      case TT_STRING:
         token_set_dval(p_token_store,
                        atof(token_get_val(p_token_next)) /
                        atof(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_DOUBLE);
         break;
      case TT_ARRAY:
         ERROR("TT_ARRAY / TT_ARRAY not yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_EQ:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) ==
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) ==
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) == 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("TT_ARRAY eq TT_ARRAY not yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_NEQ:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) !=
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) !=
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) != 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_LE:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) <=
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) <=
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) <= 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_GE:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) >=
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) >=
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) >= 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_LT:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) <
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) <
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) < 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_GT:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) >
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        token_get_dval(p_token_next) >
                        token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        strcmp(token_get_val(p_token_next),
                               token_get_val(p_token_store)) > 0);
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_AND:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) &
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        (int) token_get_dval(p_token_next) &
                        (int) token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) &
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_OR:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) |
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        (int) token_get_dval(p_token_next) |
                        (int) token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) |
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_XOR:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) ^
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        (int) token_get_dval(p_token_next) ^
                        (int) token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) ^
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_LSHIFT:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) <<
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        (int) token_get_dval(p_token_next) <<
                        (int) token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) <<
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;
   case TT_RSHIFT:
      switch (tt_highest) {
      case TT_INTEGER:
         token_set_ival(p_token_store,
                        (int) token_get_ival(p_token_next) >>
                        token_get_ival(p_token_store));
         break;
      case TT_DOUBLE:
         token_set_ival(p_token_store,
                        (int) token_get_dval(p_token_next) >>
                        (int) token_get_dval(p_token_store));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_STRING:
         token_set_ival(p_token_store,
                        atoi(token_get_val(p_token_next)) >>
                        atoi(token_get_val(p_token_store)));
         token_set_tt(p_token_store, TT_INTEGER);
         break;
      case TT_ARRAY:
         ERROR("ARRAY bla yet implemented");
         break;
         NO_DEFAULT;
      }
      break;

   default:
      _FUNCTIONS_ERROR("No such function/operator", p_token_op);
   }

#ifdef DEBUG_FUNCTION_PROCESS
   token_print(p_token_store);
   printf("\n\n");
#endif /* DEBUG_FUNCTION_PROCESS */

   token_delete(p_token_next);
}

void
function_process(Interpret *p_interpret, Token *p_token_op,
                 Token *p_token_op2, Stack *p_stack_args, int i_args) {

   Token *p_token_store = token_new_copy(stack_pop(p_stack_args));

   for (int i = 0; i < i_args -1 && !stack_empty(p_stack_args); ++i) {
      Token *p_token_next = stack_pop(p_stack_args);

      _process(p_interpret, p_token_store, p_token_op,
               p_token_op2, p_token_next);
   }

   stack_push(p_stack_args, p_token_store);
}

_Bool
function_is_buildin(Token *p_token_ident) {
   /* TODO: optimize this function */
   if (strcmp("assert", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("decr", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("double", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("end", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("exit", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("fork", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("gc", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("incr", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("ind", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("integer", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("len", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("ln", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("neg", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("no", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("put", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("scope", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("say", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("string", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("yes", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("not", token_get_val(p_token_ident)) == 0)
      return (true);

   if (strcmp("refs", token_get_val(p_token_ident)) == 0)
      return (true);

   return (false);
}

void
function_process_buildin(Interpret *p_interpret, Token *p_token_ident,
                         Stack *p_stack_args) {

   Token *p_token = stack_top(p_stack_args);

   if (token_get_tt(p_token) == TT_ARRAY) {
      if (strcmp("len", token_get_val(p_token_ident)) == 0) {
         stack_pop(p_stack_args);
         stack_push(p_stack_args,
                    token_new_integer(array_get_used(p_token->p_array)));

      } else if (strcmp("ind", token_get_val(p_token_ident)) == 0) {
         stack_pop(p_stack_args);
         stack_push(p_stack_args,
                    token_new_integer(array_get_ind(p_token->p_array)));

      } else {
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

   if (strcmp("assert", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

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

   } else if (strcmp("decr", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

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

   } else if (strcmp("double", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      Token *p_token = token_new_copy(stack_pop(p_stack_args));
      convert_to_double(p_token);
      stack_push(p_stack_args, p_token);

   } else if (strcmp("end", token_get_val(p_token_ident)) == 0) {
      exit(0);

   } else if (strcmp("fork", token_get_val(p_token_ident)) == 0) {
      Token *p_token = token_new_integer((int) fork());
      stack_push(p_stack_args, p_token);

   } else if (strcmp("gc", token_get_val(p_token_ident)) == 0) {
      int i_count = garbage_collect();
      Token *p_token = token_new_integer(i_count);
      stack_push(p_stack_args, p_token);

   } else if (strcmp("exit", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      p_token = token_new_copy(p_token);
      convert_to_integer(p_token);
      exit(token_get_ival(p_token));

   } else if (strcmp("incr", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

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

   } else if (strcmp("ind", token_get_val(p_token_ident)) == 0) {
      _FUNCTIONS_ERROR("Expected array", p_token_ident);

   } else if (strcmp("integer", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      Token *p_token = token_new_copy(stack_pop(p_stack_args));
      convert_to_integer(p_token);
      stack_push(p_stack_args, p_token);

   } else if (strcmp("len", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      Token *p_token = token_new_copy(stack_pop(p_stack_args));
      convert_to_string(p_token);
      token_set_tt(p_token, TT_INTEGER);
      token_set_ival(p_token, strlen(token_get_val(p_token)));
      stack_push(p_stack_args, p_token);

   } else if (strcmp("ln", token_get_val(p_token_ident)) == 0) {
      printf("\n");

   } else if (strcmp("neg", token_get_val(p_token_ident)) == 0) {
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

   } else if (strcmp("no", token_get_val(p_token_ident)) == 0) {
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

   } else if (strcmp("put", token_get_val(p_token_ident)) == 0) {
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

   } else if (strcmp("scope", token_get_val(p_token_ident)) == 0) {
      scope_print(p_interpret->p_scope);

   } else if (strcmp("say", token_get_val(p_token_ident)) == 0) {
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
         }
      }
      stackiterator_delete(p_iter);
      printf("\n");

   } else if (strcmp("string", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      Token *p_token = token_new_copy(stack_pop(p_stack_args));
      convert_to_string(p_token);
      stack_push(p_stack_args, p_token);

   } else if (strcmp("yes", token_get_val(p_token_ident)) == 0) {
      Token *p_token = NULL;

      if (0 == stack_size(p_stack_args)) {
         p_token = token_new_integer(1);

      } else {
         p_token = token_new_copy(stack_pop(p_stack_args));
         token_set_ival(p_token, 1);
         token_set_tt(p_token, TT_INTEGER);

      }

      stack_push(p_stack_args, p_token);

   } else if (strcmp("not", token_get_val(p_token_ident)) == 0) {
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

   } else if (strcmp("refs", token_get_val(p_token_ident)) == 0) {
      if (0 == stack_size(p_stack_args))
         _FUNCTIONS_ERROR("No argument given", p_token_ident);

      Token *p_token_top = stack_pop(p_stack_args);
      Token *p_token = token_new_integer(p_token_top->i_ref_count);
      stack_push(p_stack_args, p_token);
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
      List *p_list_token = symbol_get_val(p_symbol);
      scope_up(p_interpret->p_scope);
      interpret_subprocess(p_interpret, p_list_token);
      scope_down(p_interpret->p_scope);
   }
   NO_DEFAULT;
   }
}

void
functions_init(Functions *p_functions) {
}
