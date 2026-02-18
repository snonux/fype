/*:*
 *: File: ./src/core/interpret.c
 *: A simple interpreter
 *: 
 *: WWW		: https://codeberg.org/snonux/fype
 *: AUTHOR	: http://buetow.org
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

#include "interpret.h"

#include "../defines.h"
#include "convert.h"
#include "functions.h"
#include "symbol.h"

#define _INTERPRET_ERROR(m,t) \
	 ERROR(\
		"%s: Interpret error in %s line %d pos %d near '%s'", m, \
		token_get_filename(t), \
		token_get_line_nr(t), \
		token_get_pos_nr(t), \
		token_get_val(t) \
		)

#define _CHECK if (p_interpret->p_token == NULL) return (0);
#define _HAS_NEXT listiterator_has_next(p_interpret->p_iter)
#define _NEXT_ORG _next(p_interpret);
#define _NEXT if (!_next(p_interpret)) { return (2); }
#define _NEXT2 _NEXT _NEXT
#define _NEXT_TT _next_tt(p_interpret)
#define _SKIP _next(p_interpret);

int _block(Interpret *p_interpret);
int _block_get(Interpret *p_interpret, List *p_list_block);
int _block_skip(Interpret *p_interpret);
int _compare(Interpret *p_interpret);
int _control(Interpret *p_interpret);
int _expression(Interpret *p_interpret);
int _expression_(Interpret *p_interpret);
int _expression_get(Interpret *p_interpret, List *p_list_block);
int _func_decl(Interpret *p_interpret);
int _next(Interpret *p_interpret);
int _proc_decl(Interpret *p_interpret);
int _product(Interpret *p_interpret);
int _product2(Interpret *p_interpret);
int _program(Interpret *p_interpret);
int _statement(Interpret *p_interpret);
int _sum(Interpret *p_interpret);
int _term(Interpret *p_interpret);
int _var_assign(Interpret *p_interpret);
int _var_decl(Interpret *p_interpret);
int _var_list(Interpret *p_interpret);
void _print_lookahead(Interpret *p_interpret);

Interpret*
interpret_new(List *p_list_token, Hash *p_hash_syms) {
   Interpret *p_interpret = malloc(sizeof(Interpret));

   if (p_hash_syms != NULL) {
      p_interpret->p_scope = scope_new(p_hash_syms);
      p_interpret->b_scope_delete = true;

   } else {
      p_interpret->p_scope = NULL;
      p_interpret->b_scope_delete = false;
   }

   p_interpret->p_list_token = p_list_token;
   p_interpret->p_stack = stack_new();
   p_interpret->tt = TT_NONE;
   p_interpret->p_token = NULL;
   p_interpret->tt_prev = TT_NONE;
   p_interpret->p_token_prev = NULL;
   p_interpret->p_token_temp = NULL;
   p_interpret->ct = CONTROL_NONE;

   return (p_interpret);
}

void
interpret_delete(Interpret *p_interpret) {
   if (!p_interpret)
      return;

   if (p_interpret->b_scope_delete)
      scope_delete(p_interpret->p_scope);

   stack_delete(p_interpret->p_stack);
   free(p_interpret);
}

void
_print_lookahead(Interpret *p_interpret) {
   ListIterator *p_iter = p_interpret->p_iter;
   ListIteratorState *p_state = listiterator_get_state(p_iter);

   printf("LOOLAHEAD:\n");

   token_print(p_interpret->p_token);
   printf("\n");

   while (listiterator_has_next(p_iter)) {
      Token *p_token = listiterator_next(p_iter);
      token_print(p_token);
      printf("\n");
   }

   listiterator_set_state(p_iter, p_state);
   listiteratorstate_delete(p_state);
}

int
_next(Interpret *p_interpret) {
   if (listiterator_has_next(p_interpret->p_iter)) {
      p_interpret->p_token_prev = p_interpret->p_token;
      p_interpret->tt_prev = p_interpret->tt;

      p_interpret->p_token = listiterator_next(p_interpret->p_iter);
      p_interpret->tt = token_get_tt(p_interpret->p_token);
      return (1);
   }

   p_interpret->p_token = NULL;
   p_interpret->tt = TT_NONE;
   //printf("==>\n");

   return (0);
}

TokenType
_next_tt(Interpret *p_interpret) {
   if (listiterator_has_next(p_interpret->p_iter)) {
      Token *p_token = listiterator_current(p_interpret->p_iter);
      return (token_get_tt(p_token));
   }

   return (TT_NONE);
}

int
_program(Interpret *p_interpret) {
   _CHECK TRACK

   /* Stop executing statements as soon as break/next is active so the
    * control flag propagates cleanly up to the enclosing loop. */
   while (_statement(p_interpret) == 1 && p_interpret->ct == CONTROL_NONE)
      garbage_collect();

   return (1);
}

int
_var_decl(Interpret *p_interpret) {
   _CHECK TRACK

   switch (p_interpret->tt) {
   case TT_MY:
   {
      if (_NEXT_TT != TT_IDENT)
         _INTERPRET_ERROR("'my' expects identifier", p_interpret->p_token);

      _NEXT

      Token *p_token_ident = p_interpret->p_token;

      _var_assign(p_interpret);
      _var_list(p_interpret);

      if (p_interpret->tt == TT_SEMICOLON) {
         _NEXT
         return (1);

      } else if (p_interpret->p_token != NULL) {
         _INTERPRET_ERROR("Expected ';'", p_interpret->p_token);

      } else {
         _INTERPRET_ERROR("Expected ';' after", p_token_ident);
      }
   }
   default:
      break;
   }

   return (0);
}

int
_var_assign(Interpret *p_interpret) {
   _CHECK TRACK

   if (p_interpret->tt == TT_IDENT) {
      Token *p_token = p_interpret->p_token;
      _NEXT

      char *c_name = token_get_val(p_token);
      if (scope_exists(p_interpret->p_scope, c_name)) {
         _INTERPRET_ERROR("Symbol already defined", p_token);
      }

      if (p_interpret->tt == TT_ASSIGN) {
         _NEXT

         if (p_interpret->tt == TT_VID) {
            _NEXT
            if (p_interpret->tt != TT_IDENT)
               _INTERPRET_ERROR("Expected identifier", p_interpret->p_token);

            char *c_name_ = token_get_val(p_interpret->p_token);
            Symbol *p_symbol = scope_get(p_interpret->p_scope, c_name_);

            if (p_symbol == NULL)
               _INTERPRET_ERROR("No such symbol", p_interpret->p_token);

            symbol_ref_up(p_symbol);
            scope_newset(p_interpret->p_scope, c_name, p_symbol);
            _NEXT

         } else {
            Stack *p_stack = p_interpret->p_stack;
            p_interpret->p_stack = stack_new();

            if (_expression_(p_interpret)) {
               function_process_buildin(p_interpret, p_token,
                                        p_interpret->p_stack);

               stack_merge(p_stack, p_interpret->p_stack);
               stack_delete(p_interpret->p_stack);
               p_interpret->p_stack = p_stack;

               p_token = stack_top(p_interpret->p_stack);
               Symbol *p_symbol = symbol_new(SYM_VARIABLE, p_token);
               scope_newset(p_interpret->p_scope, c_name, p_symbol);
            } else {
               return (0);
            }
         }
      } else {
         Token *p_token = token_new_integer(0);
         Symbol *p_symbol = symbol_new(SYM_VARIABLE, p_token);
         scope_newset(p_interpret->p_scope, c_name, p_symbol);
      }
   }

   return (1);
}

int
_var_list(Interpret *p_interpret) {
   _CHECK TRACK

   if (p_interpret->tt == TT_COMMA) {
      _NEXT
      _var_assign(p_interpret);
      _var_list(p_interpret);
   }

   return (1);
}

int
_block_get(Interpret *p_interpret, List *p_list_block) {
   if (p_interpret->tt != TT_PARANT_CL)
      _INTERPRET_ERROR("Expected '{'", p_interpret->p_token);
   _NEXT

   int i_num_parant = 0;

   for (;;) {
      if (p_interpret->tt == TT_PARANT_CL) {
         ++i_num_parant;

      } else if (p_interpret->tt == TT_PARANT_CR) {
         if (--i_num_parant == -1) {
            _NEXT
            break; /* for */
         }
      }

      list_add_back(p_list_block, p_interpret->p_token);

      _NEXT
   }

#ifdef DEBUG_BLOCK_GET
   printf("DEBUG::BLOCK::GET: ====>\n");
   list_iterate(p_list_block, token_print_cb);
   printf("DEBUG::BLOCK::GET: <====\n");
#endif /* DEBUG_BLOCK_GET */

   return (1);
}

int
_expression_get(Interpret *p_interpret, List *p_list_expression) {
   for (;;) {
      if (p_interpret->tt == TT_PARANT_CL) {
         break; /* for */
      }

      list_add_back(p_list_expression, p_interpret->p_token);

      _NEXT
   }

#ifdef DEBUG_EXPRESSION_GET
   printf("DEBUG::EXPRESSION::GET: ====>\n");
   list_iterate(p_list_expression, token_print_cb);
   printf("DEBUG::EXPRESSION::GET: <====\n");
#endif /* DEBUG_EXPRESSION_GET */

   return (1);
}

int
_block_skip(Interpret *p_interpret) {
   if (p_interpret->tt != TT_PARANT_CL)
      _INTERPRET_ERROR("Expected '{'", p_interpret->p_token);
   _NEXT

   int i_num_parant = 0;

   for (;;) {
      if (p_interpret->tt == TT_PARANT_CL) {
         ++i_num_parant;

      } else if (p_interpret->tt == TT_PARANT_CR) {
         if (--i_num_parant == -1) {
            _NEXT
            break; /* for */
         }
      }

      _NEXT
   }

   return (1);
}

int
_proc_decl(Interpret *p_interpret) {
   _CHECK TRACK

   if (p_interpret->tt == TT_PROC) {
      _NEXT

      if (p_interpret->tt != TT_IDENT)
         _INTERPRET_ERROR("Expected identifier", p_interpret->p_token);

      Token *p_token_ident = p_interpret->p_token;
      _NEXT

      if (scope_exists(p_interpret->p_scope, token_get_val(p_token_ident))) {
         _INTERPRET_ERROR("Symbol already defined", p_token_ident);
      }

      List *p_list_proc = list_new();

      if (_block_get(p_interpret, p_list_proc)) {

         Symbol *p_symbol = symbol_new(SYM_PROCEDURE, p_list_proc);
         scope_newset(p_interpret->p_scope, token_get_val(p_token_ident),
                      p_symbol);

         return (1);
      }

      list_delete(p_list_proc);
   }

   return (0);
}

int
_func_decl(Interpret *p_interpret) {
   _CHECK TRACK

   if (p_interpret->tt == TT_FUNC) {
      _NEXT

      if (p_interpret->tt != TT_IDENT)
         _INTERPRET_ERROR("Expected identifier", p_interpret->p_token);

      Token *p_token_ident = p_interpret->p_token;
      _NEXT

      if (scope_exists(p_interpret->p_scope, token_get_val(p_token_ident))) {
         _INTERPRET_ERROR("Symbol already defined", p_token_ident);
      }

      List *p_list_proc = list_new();

      if (_block_get(p_interpret, p_list_proc)) {

         Symbol *p_symbol = symbol_new(SYM_FUNCTION, p_list_proc);
         scope_newset(p_interpret->p_scope, token_get_val(p_token_ident),
                      p_symbol);

         return (1);
      }

      list_delete(p_list_proc);
   }

   return (0);
}

int
_statement(Interpret *p_interpret) {
   _CHECK TRACK

   for (int i = 0; i < 2; ++i) {
      if (_proc_decl(p_interpret)) return (1);
      if (_func_decl(p_interpret)) return (1);
      if (_var_decl(p_interpret)) return (1);
      if (_control(p_interpret)) return (1);
      if (_expression(p_interpret)) return (1);
      if (_block(p_interpret)) return (1);
   }

   return (0);
}

int
_block(Interpret *p_interpret) {
   if (p_interpret->tt == TT_PARANT_CL) {
      List *p_list_block = list_new();

      if (_block_get(p_interpret, p_list_block)) {
         scope_up(p_interpret->p_scope);
         interpret_subprocess(p_interpret, p_list_block);
         scope_down(p_interpret->p_scope);
         list_delete(p_list_block);
         return (1);
      }

      list_delete(p_list_block);
   }

   return (0);
}

int
_expression(Interpret *p_interpret) {
   _CHECK TRACK

   if (_expression_(p_interpret)) {
      TokenType tt = p_interpret->tt;
      if (tt == TT_SEMICOLON || tt == TT_NONE) {
         _NEXT

      } else {
         _INTERPRET_ERROR("Expected ';'", p_interpret->p_token);
      }

      stack_clear(p_interpret->p_stack);
      return (1);
   }

   return (0);
}

int
_expression_(Interpret *p_interpret) {
   return (_compare(p_interpret));
}

int
_control(Interpret *p_interpret) {
   _CHECK TRACK

   Token *p_token = p_interpret->p_token;

   switch (p_interpret->tt) {
   /* break; — set the break flag; the statement loop in _program() will
    * stop and the flag propagates up to the enclosing while/until. */
   case TT_BREAK:
      p_interpret->ct = CONTROL_BREAK;
      _NEXT
      return (1);
   /* next; — set the next flag to skip the rest of the loop body and
    * let the loop re-evaluate its condition on the next iteration. */
   case TT_NEXT:
      p_interpret->ct = CONTROL_NEXT;
      _NEXT
      return (1);
   case TT_IF:
   case TT_IFNOT:
   {
      TokenType tt = p_interpret->tt;
      _NEXT
      if (_expression_(p_interpret)) {
         Token *p_token_top = stack_pop(p_interpret->p_stack);
         List *p_list_block = list_new();
         _block_get(p_interpret, p_list_block);
         int ret = 0;

         switch (tt) {
         case TT_IF:
            if (convert_to_integer_get(p_token_top)) {
               scope_up(p_interpret->p_scope);
               ret = interpret_subprocess(p_interpret, p_list_block);
               scope_down(p_interpret->p_scope);
            }
            break;
         case TT_IFNOT:
            if (!convert_to_integer_get(p_token_top)) {
               scope_up(p_interpret->p_scope);
               ret = interpret_subprocess(p_interpret, p_list_block);
               scope_down(p_interpret->p_scope);
            }
            break;
            NO_DEFAULT;
         }

         list_delete(p_list_block);
         return (1);

      } else {
         _INTERPRET_ERROR("Expected expression after control keyword", p_token);
      }
   }
   break;
   case TT_WHILE:
   case TT_UNTIL:
   {
      TokenType tt = p_interpret->tt;
      List *p_list_expr = list_new(), *p_list_block = list_new();
      _Bool b_flag = true;

      _NEXT

      _expression_get(p_interpret, p_list_expr);
      _block_get(p_interpret, p_list_block);
      Token *p_token_backup = p_interpret->p_token;

      do {
         Stack *p_stack_backup = p_interpret->p_stack;
         p_interpret->p_stack = stack_new();

         ListIterator *p_iter_backup = p_interpret->p_iter;
         p_interpret->p_iter = listiterator_new(p_list_expr);

         _NEXT

         /* Dont use if here, because we want to check the p_itnerpret->ct */
         if (_expression_(p_interpret)) {
            Token *p_token_top = stack_pop(p_interpret->p_stack);

            if (p_token_top == NULL) {
               printf("FOO\n");
               exit(0);
            }
            if (tt ==  TT_WHILE) {
               if (convert_to_integer_get(p_token_top)) {
                  scope_up(p_interpret->p_scope);
                  interpret_subprocess(p_interpret, p_list_block);
                  scope_down(p_interpret->p_scope);

               } else {
                  b_flag = false;
               }

            } else if (tt == TT_UNTIL) {
               if (!convert_to_integer_get(p_token_top)) {
                  scope_up(p_interpret->p_scope);
                  interpret_subprocess(p_interpret, p_list_block);
                  scope_down(p_interpret->p_scope);

               } else {
                  b_flag = false;
               }
            }

            /* Act on any break/next flag set during loop body execution.
             * break clears the flag and stops iteration; next clears it
             * and lets the loop re-evaluate the condition naturally. */
            if (p_interpret->ct == CONTROL_BREAK) {
               p_interpret->ct = CONTROL_NONE;
               b_flag = false;
            } else if (p_interpret->ct == CONTROL_NEXT) {
               p_interpret->ct = CONTROL_NONE;
               /* b_flag stays true; condition re-evaluated next iteration */
            }

         } else {
            _INTERPRET_ERROR("Expected expression after control keyword",
                             p_token);
         }

         listiterator_delete(p_interpret->p_iter);
         p_interpret->p_iter = p_iter_backup;

         stack_delete(p_interpret->p_stack);
         p_interpret->p_stack = p_stack_backup;

      } while (b_flag);

      list_delete(p_list_expr);
      list_delete(p_list_block);
      p_interpret->p_token = p_token_backup;
      p_interpret->tt = token_get_tt(p_token_backup);

      return (1);
   }
   break;
   case TT_LOOP:
   {
      List *p_list_block = list_new();
      _NEXT
      _block_get(p_interpret, p_list_block);
      Token *p_token_backup = p_interpret->p_token;

      /* Run forever; break is the only exit, next restarts the body */
      for (;;) {
         scope_up(p_interpret->p_scope);
         interpret_subprocess(p_interpret, p_list_block);
         scope_down(p_interpret->p_scope);

         if (p_interpret->ct == CONTROL_BREAK) {
            p_interpret->ct = CONTROL_NONE;
            break;
         } else if (p_interpret->ct == CONTROL_NEXT) {
            p_interpret->ct = CONTROL_NONE;
            /* skip the rest of the body; re-run from the top */
         }
      }

      list_delete(p_list_block);
      p_interpret->p_token = p_token_backup;
      p_interpret->tt = token_get_tt(p_token_backup);
      return (1);
   }
   break;
   case TT_DO:
   {
      /* do { body } while expr;  or  do { body } until expr;
       * The body always executes at least once; the condition is evaluated
       * at the bottom of each iteration (post-condition loop). */
      List *p_list_block = list_new();
      List *p_list_expr  = list_new();

      _NEXT
      _block_get(p_interpret, p_list_block);  /* leaves at 'while'/'until' */

      Token *p_token = p_interpret->p_token;
      TokenType tt = p_interpret->tt;
      if (tt != TT_WHILE && tt != TT_UNTIL)
         _INTERPRET_ERROR(
            "Expected 'while' or 'until' after 'do' block", p_token);

      _NEXT   /* past 'while' or 'until' */

      /* Collect condition tokens until ';' — mirrors _expression_get but
       * stops at semicolon instead of '{' since there is no block here */
      while (p_interpret->tt != TT_SEMICOLON
             && p_interpret->tt != TT_NONE) {
         list_add_back(p_list_expr, p_interpret->p_token);
         _NEXT
      }
      _NEXT   /* past ';' */
      Token *p_token_backup = p_interpret->p_token;  /* after ';' */

      _Bool b_flag = true;
      do {
         scope_up(p_interpret->p_scope);
         interpret_subprocess(p_interpret, p_list_block);
         scope_down(p_interpret->p_scope);

         /* Handle break/next before re-evaluating the condition */
         if (p_interpret->ct == CONTROL_BREAK) {
            p_interpret->ct = CONTROL_NONE;
            b_flag = false;
            continue;
         } else if (p_interpret->ct == CONTROL_NEXT) {
            p_interpret->ct = CONTROL_NONE;
            /* fall through to condition check */
         }

         /* Re-evaluate condition using a temp stack/iterator over
          * p_list_expr, exactly as the while/until loop does */
         Stack *p_stack_backup = p_interpret->p_stack;
         p_interpret->p_stack = stack_new();
         ListIterator *p_iter_backup = p_interpret->p_iter;
         p_interpret->p_iter = listiterator_new(p_list_expr);
         _NEXT

         if (_expression_(p_interpret)) {
            Token *p_token_top = stack_pop(p_interpret->p_stack);
            int i_val = convert_to_integer_get(p_token_top);
            b_flag = (tt == TT_WHILE) ? (i_val != 0) : (i_val == 0);
         }

         listiterator_delete(p_interpret->p_iter);
         p_interpret->p_iter = p_iter_backup;
         stack_delete(p_interpret->p_stack);
         p_interpret->p_stack = p_stack_backup;

      } while (b_flag);

      list_delete(p_list_block);
      list_delete(p_list_expr);
      p_interpret->p_token = p_token_backup;
      p_interpret->tt = token_get_tt(p_token_backup);
      return (1);
   }
   break;
   NO_DEFAULT;
   }

   return (0);
}

int
_compare(Interpret *p_interpret) {
   _CHECK TRACK

   if (_sum(p_interpret)) {
      _Bool b_flag = true;

      do {
         switch (p_interpret->tt) {
         case TT_NOT:
         case TT_ASSIGN:
         case TT_LT:
         case TT_GT:
         {
            Token *p_token_op = p_interpret->p_token;
            Token *p_token_op2 = NULL;
            _NEXT

            switch (p_interpret->tt) {
            case TT_NOT:
            case TT_ASSIGN:
            case TT_LT:
            case TT_GT:
               p_token_op2 = p_interpret->p_token;
               _NEXT
            default:
               break;
            }

            if (!_sum(p_interpret))
               _INTERPRET_ERROR("Expected sum", p_interpret->p_token);

            function_process(p_interpret, p_token_op, p_token_op2,
                             p_interpret->p_stack, 2);
         }
         break; /* case */

         default:
            b_flag = false;
            break;

         } /* switch */

      } while (b_flag);

      return (1);
   }

   return (0);
}

int
_sum(Interpret *p_interpret) {
   _CHECK TRACK

   if (_product(p_interpret)) {
      _Bool b_flag = true;

      do {
         Token *p_token_op2 = NULL, *p_token_tmp = NULL;

         switch (p_interpret->tt) {
         case TT_DDOT:
            p_token_tmp = p_interpret->p_token;
            _NEXT
         case TT_ADD:
         case TT_SUB:
         case TT_AND:
         case TT_OR:
         case TT_XOR:
         {
            Token *p_token_op = p_interpret->p_token;
            _NEXT

            if (p_token_tmp != NULL) {
               p_token_op2 = p_token_op;
               p_token_op = p_token_tmp;;
            }

            if (!_product(p_interpret))
               _INTERPRET_ERROR("Expected product", p_token_op);

            function_process(p_interpret, p_token_op, p_token_op2,
                             p_interpret->p_stack, 2);

         }
         break; /* case */

         default:
            b_flag = false;
            break;

         } /* switch */

      } while (b_flag);

      return (1);
   }

   return (0);
}

int
_product(Interpret *p_interpret) {
   _CHECK TRACK

   if (_product2(p_interpret)) {
      _Bool b_flag = true;

      do {
         switch (p_interpret->tt) {
         case TT_MULT:
         case TT_DIV:
         {
            Token *p_token = p_interpret->p_token;
            _NEXT

            if (!_product2(p_interpret))
               _INTERPRET_ERROR("Expected product2", p_token);

            function_process(p_interpret, p_token, NULL,
                             p_interpret->p_stack, 2);

         }
         break; /* case */

         default:
            b_flag = false;
            break;

         } /* switch */

      } while (b_flag);

      return (1);
   }

   return (0);
}

int
_product2(Interpret *p_interpret) {
   _CHECK TRACK

   if (_term(p_interpret)) {
      _Bool b_flag = true;

      do {
         if (p_interpret->tt == TT_ASSIGN
               && IS_NOT_OPERATOR(_NEXT_TT)) {
            Token *p_token = p_interpret->p_token;
            Token *p_token_temp = p_interpret->p_token_prev;
            _NEXT

            if (!_expression_(p_interpret))
               _INTERPRET_ERROR("Expected expression", p_token);

            p_interpret->p_token_temp = p_token_temp;
            function_process(p_interpret, p_token, NULL,
                             p_interpret->p_stack, 2);
            p_interpret->p_token_temp = NULL;

         } else {
            b_flag = false;
            break;
         } /* if */
      } while (b_flag);

      return (1);
   }

   return (0);
}

int
_term(Interpret *p_interpret) {
   _CHECK TRACK

   switch (p_interpret->tt) {
   case TT_ARRAY:
   case TT_STRING:
   case TT_INTEGER:
   case TT_DOUBLE:
      stack_push(p_interpret->p_stack, p_interpret->p_token);
      // Checks if the term is the last element of an array
      //	say ["element"] # The "element"
      // or of a function
      // 	func foo { say 1 } # The 1
      if (_NEXT_TT != TT_PARANT_AR && _NEXT_TT != TT_PARANT_CR)
         _NEXT
         return (1);

   case TT_IDENT:
   {
      if (_NEXT_TT != TT_ASSIGN) {
         if (_NEXT_TT == TT_PARANT_AL) {
            Token *p_token_var = p_interpret->p_token;
            char *c_name = token_get_val(p_token_var);
            Symbol *p_symbol = scope_get(p_interpret->p_scope, c_name);

            if (p_symbol == NULL)
               _INTERPRET_ERROR("No such symbol", p_token_var);
            Token *p_token_array = symbol_get_val(p_symbol);
            Array *p_array = TOKEN_GET_ARRAY(p_token_array);
            if (p_array == NULL)
               _INTERPRET_ERROR("Expected an array", p_interpret->p_token);

            _NEXT2
            Token *p_token_val = array_get(p_array,
                                           convert_to_integer_get(p_interpret->p_token));
            stack_push(p_interpret->p_stack, p_token_val);
            _NEXT

            return (1);

         } else if (function_is_buildin(p_interpret->p_token)) {
            Token *p_token = p_interpret->p_token;
            Stack *p_stack = p_interpret->p_stack;
            p_interpret->p_stack = stack_new();

            if (_HAS_NEXT) {
               _NEXT
               //if (_expression_(p_interpret));
               _expression_(p_interpret);

            } else {
               _SKIP
            }

            function_process_buildin(p_interpret, p_token,
                                     p_interpret->p_stack);

            stack_merge(p_stack, p_interpret->p_stack);
            stack_delete(p_interpret->p_stack);
            p_interpret->p_stack = p_stack;

            return (1);

         } else if (function_is_self_defined(p_interpret)) {
            Token *p_token = p_interpret->p_token;
            Stack *p_stack = p_interpret->p_stack;
            p_interpret->p_stack = stack_new();

            _NEXT
            if (_expression_(p_interpret));

            function_process_self_defined(p_interpret, p_token);

            if (stack_empty(p_interpret->p_stack)) {
               Token *p_token = token_new_dummy();
               token_set_tt(p_token, TT_INTEGER);
               token_set_ival(p_token, 0);
               stack_push(p_interpret->p_stack, p_token);
            }

            stack_merge(p_stack, p_interpret->p_stack);
            stack_delete(p_interpret->p_stack);
            p_interpret->p_stack = p_stack;

            return (1);
         }
      }

      /* It is not a function, it is a variable or some sort of */

      char *c_name = token_get_val(p_interpret->p_token);
      Symbol *p_symbol = scope_get(p_interpret->p_scope, c_name);

      if (p_symbol == NULL)
         _INTERPRET_ERROR("No such symbol", p_interpret->p_token);

      SymbolType st = symbol_get_sym(p_symbol);

      switch (st) {
      case SYM_VARIABLE:
         stack_push(p_interpret->p_stack, symbol_get_val(p_symbol));
         _NEXT
         return (1);

         /* Example: proc foo { foo = "Hello"; } foo; say foo; */
      case SYM_PROCEDURE:
         stack_push(p_interpret->p_stack, symbol_get_val(p_symbol));
         _NEXT
         return (1);

         NO_DEFAULT;
      }
   }
   break;
   case TT_DEFINED:
   {
      _NEXT
      if (p_interpret->tt != TT_IDENT)
         _INTERPRET_ERROR("Expexted identifier for 'defined'",
                          p_interpret->p_token);

      char *c_name = token_get_val(p_interpret->p_token);
      Token *p_token = token_new_integer(0);

      if (scope_exists(p_interpret->p_scope, c_name))
         token_set_ival(p_token, 1);

      stack_push(p_interpret->p_stack, p_token);

      _NEXT;
      return (1);
   }
   break;

   case TT_UNDEF:
   {
      _NEXT
      if (p_interpret->tt != TT_IDENT)
         _INTERPRET_ERROR("Expexted identifier for 'defined'",
                          p_interpret->p_token);

      char *c_name = token_get_val(p_interpret->p_token);
      Token *p_token = NULL;
      Symbol *p_symbol = NULL;

      if ((p_symbol = scope_remove(p_interpret->p_scope, c_name)))  {
         symbol_delete(p_symbol);
         p_token = token_new_integer(1);

      } else {
         p_token = token_new_integer(0);
      }

      stack_push(p_interpret->p_stack, p_token);

      _NEXT;
      return (1);
   }
   break;

   case TT_SYMS:
   {
      _NEXT
      if (p_interpret->tt != TT_IDENT)
         _INTERPRET_ERROR("Expexted identifier for 'syms'",
                          p_interpret->p_token);

      char *c_name = token_get_val(p_interpret->p_token);
      Symbol *p_symbol = scope_get(p_interpret->p_scope, c_name);
      if (p_symbol == NULL)
         _INTERPRET_ERROR("No such symbol", p_interpret->p_token);


      Token *p_token_num_refs = token_new_integer(p_symbol->i_refs);
      stack_push(p_interpret->p_stack, p_token_num_refs);

      _NEXT;
      return (1);
   }
   break;

   case TT_PARANT_AL:
   {
      Token *p_token_arr = token_new_array(ARRAY_SIZE);
      Array *p_array = p_token_arr->p_array;

      _NEXT
      // Get the array elements
      while (p_interpret->tt != TT_PARANT_AR) {
         TokenType tt = tt = p_interpret->tt;
         if (tt != TT_COMMA && tt != TT_SEMICOLON) {
            UNLESS (_expression_(p_interpret)) {
               Token *p_token = p_interpret->p_token;
               _INTERPRET_ERROR("Expected expression", p_token);
            }

            array_unshift(p_array, stack_pop(p_interpret->p_stack));
         }

         _NEXT
      }

      stack_push(p_interpret->p_stack, p_token_arr);
      _NEXT
      return (1);
   }
   break;

   case TT_PARANT_L:
   {
      Token *p_token = p_interpret->p_token;
      _NEXT

      if (_expression_(p_interpret)) {
         if (p_interpret->tt != TT_PARANT_R)
            _INTERPRET_ERROR("Expected ')'", p_token);

      } else {
         _INTERPRET_ERROR("Expected expression", p_token);
      }
   }
   _NEXT
   return (1);

   default:
      break;
   }

   return (0);
}

int
interpret_process(Interpret *p_interpret) {
   p_interpret->p_iter = listiterator_new(p_interpret->p_list_token);

   _NEXT
   _program(p_interpret);

   listiterator_delete(p_interpret->p_iter);

   return (1);
}


int
interpret_subprocess(Interpret *p_interpret, List *p_list_token) {
   Interpret *p_interpret_sub = interpret_new(p_list_token,
                                NULL);

   p_interpret_sub->p_scope = p_interpret->p_scope;

   int i_ret = interpret_process(p_interpret_sub);
   p_interpret->ct = p_interpret_sub->ct;

   interpret_delete(p_interpret_sub);

   return (i_ret);
}

void
interpret_run(Fype *p_fype) {
   Interpret *p_interpret =
      interpret_new(p_fype->p_list_token, p_fype->p_hash_syms);

   interpret_process(p_interpret);

   interpret_delete(p_interpret);
}

