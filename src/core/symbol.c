/*:*
 *: File: ./src/core/symbol.c
 *: A simple interpreter
 *: 
 *: WWW		: https://github.com/snonux/fype
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

#include "symbol.h"

#include "../data/list.h"
#include "token.h"

Symbol*
symbol_new(SymbolType sym, void *p_val) {
   Symbol *p_symbol = malloc(sizeof(Symbol));

   p_symbol->sym = sym;
   p_symbol->p_val = p_val;
   p_symbol->i_refs = 1;

   return p_symbol;
}

void
symbol_delete(Symbol *p_symbol) {
   if (--p_symbol->i_refs == 0) {
      switch (symbol_get_sym(p_symbol)) {
      case SYM_PROCEDURE:
      {
         List *p_list_token = symbol_get_val(p_symbol);
         list_delete(p_list_token);
      }
      case SYM_ARRAY:
         symbol_delete(symbol_get_val(p_symbol));
         break;
         NO_DEFAULT;
      }
      free(p_symbol);
   }
}

void
symbol_cleanup_hash_syms_cb(void *p_void) {
   symbol_delete(p_void);
}

void
symbol_print(Symbol *p_symbol, char *c_key) {
   printf("%s: %s", sym_get_name(p_symbol->sym), c_key);

   switch (p_symbol->sym) {
   case SYM_BUILDIN:
   case SYM_CONSTANT:
      break;
   case SYM_PROCEDURE:
   case SYM_FUNCTION:
      //list_iterate(symbol_get_val(p_symbol), token_print_cb);
      break;
   case SYM_ARRAY:
      break;
   case SYM_VARIABLE:
      printf(" ");
      token_print(symbol_get_val(p_symbol));
      break;
   }

   printf("\n");
}

void
symbol_print_cb(void *p_void, char *c_key) {
   symbol_print(p_void, c_key);
}

char*
sym_get_name(SymbolType sym) {
   switch (sym) {
   case SYM_CONSTANT:
      return ("SYM_CONSTANT");
   case SYM_ARRAY:
      return ("SYM_ARRAY");
   case SYM_VARIABLE:
      return ("SYM_VARIABLE");
   case SYM_BUILDIN:
      return ("SYM_BUILDIN");
   case SYM_PROCEDURE:
      return ("SYM_PROCEDURE");
   case SYM_FUNCTION:
      return ("SYM_FUNCTION");
   }

   // Never reach this point
   return ("NONE");
}
