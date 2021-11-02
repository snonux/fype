/*:*
 *: File: ./src/core/scope.c
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

#include "garbage.h"
#include "scope.h"
#include "symbol.h"

Scope*
scope_new(Hash *p_hash_syms) {
   Scope *p_scope = malloc(sizeof(Scope));

   p_scope->p_hash_global = p_hash_syms;
   p_scope->p_stack_scopes = stack_new();

   return (p_scope);
}

void
scope_delete(Scope *p_scope) {
   while (!stack_empty(p_scope->p_stack_scopes))
      scope_down(p_scope);

   stack_delete(p_scope->p_stack_scopes);

   free(p_scope);
}

void
scope_up(Scope *p_scope) {
   stack_push(p_scope->p_stack_scopes, hash_new(1024));
}

void
scope_down(Scope *p_scope) {
   Hash *p_hash_syms = stack_pop(p_scope->p_stack_scopes);
   hash_iterate(p_hash_syms, symbol_cleanup_hash_syms_cb);
   hash_delete(p_hash_syms);
}

static Hash*
_scope_get_hash(Scope *p_scope, char *c_key) {
   StackIterator *p_iter = stackiterator_new(p_scope->p_stack_scopes);

   while (stackiterator_has_next(p_iter)) {
      Hash *p_hash_syms = stackiterator_next(p_iter);
      Symbol *p_symbol = hash_get(p_hash_syms, c_key);

      if (p_symbol != NULL) {
         stackiterator_delete(p_iter);
         return (p_hash_syms);
      }
   }

   stackiterator_delete(p_iter);

   if (hash_get(p_scope->p_hash_global, c_key))
      return (p_scope->p_hash_global);

   return (NULL);
}

Symbol*
scope_get(Scope *p_scope, char *c_key) {
   Hash *p_hash_syms = _scope_get_hash(p_scope, c_key);

   if (p_hash_syms == NULL)
      return (NULL);

   return (hash_get(p_hash_syms, c_key));
}

Symbol*
scope_remove(Scope *p_scope, char *c_key) {
   Hash *p_hash_syms = _scope_get_hash(p_scope, c_key);

   if (p_hash_syms == NULL)
      return (NULL);

   Symbol *p_symbol =  hash_remove(p_hash_syms, c_key);

   return (p_symbol);
}

_Bool
scope_exists(Scope *p_scope, char *c_key) {
   return (scope_get(p_scope, c_key) != NULL);
}

_Bool
scope_reset(Scope *p_scope, char *c_key, Symbol *p_symbol) {
   Hash *p_hash_syms = _scope_get_hash(p_scope, c_key);

   if (p_hash_syms == NULL)
      return (false);

   hash_remove(p_hash_syms, c_key);
   hash_insert(p_hash_syms, c_key, p_symbol);

   return (true);
}

_Bool
scope_newset(Scope *p_scope, char *c_key, Symbol *p_symbol) {
   Hash *p_hash_syms = NULL;

   if (stack_empty(p_scope->p_stack_scopes)) {
      if (hash_get(p_scope->p_hash_global, c_key) != NULL)
         return (false);

      p_hash_syms = p_scope->p_hash_global;

   } else {
      p_hash_syms = stack_top(p_scope->p_stack_scopes);
   }

   hash_insert(p_hash_syms, c_key, p_symbol);

   return (true);
}

void
_scope_print_cb(void *p_void, int i_level) {
   Hash *p_hash_syms = p_void;
   if (i_level > 0)
      printf("%d level(s) up:\n", i_level);
   hash_iterate_key(p_hash_syms, symbol_print_cb);
}

void
scope_print(Scope *p_scope) {
   printf("Scopes:\n");
   printf("Scope stack size: %d\n", stack_size(p_scope->p_stack_scopes));
   printf("Global symbols:\n");
   hash_iterate_key(p_scope->p_hash_global, symbol_print_cb);
   printf("Local symbols:\n");
   stack_iterate_level(p_scope->p_stack_scopes, _scope_print_cb);
}
