/*:*
 *: File: ./src/core/symbol.h
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

#ifndef SYMBOL_H
#define SYMBOL_H

#include "../defines.h"
#include "../data/list.h"

#define symbol_set_val(s,v) s->p_val = v
#define symbol_set_sym(s,st) s->sym = st
#define symbol_get_val(s) s->p_val
#define symbol_get_sym(s) s->sym
#define symbol_ref_up(s) ++s->i_refs
#define IS_A_FUNCTION(s) (s == SYM_INLINEFUNCTION || s == SYM_FUNCTION)
#define IS_NOT_A_FUNCTION(s) !IS_A_FUNCTION(s)

typedef enum {
   SYM_BUILDIN,
   SYM_CONSTANT,
   SYM_FUNCTION,
   SYM_PROCEDURE,
   SYM_ARRAY,
   SYM_VARIABLE,
} SymbolType;

/* Wraps a function's body token list and its named parameter names.
 * Every SYM_FUNCTION symbol stores a FuncDef* as its p_val. */
typedef struct {
   List *p_body;    /* function body token list */
   List *p_params;  /* list of strdup'd char* param names */
   int   i_nparams; /* cached count of p_params */
} FuncDef;

FuncDef* funcdef_new(List *p_body, List *p_params, int i_nparams);
void     funcdef_delete(FuncDef *p_funcdef);

typedef struct {
   SymbolType sym;
   void *p_val;
   unsigned i_refs;
} Symbol;

Symbol* symbol_new(SymbolType sym, void *p_val);
void symbol_delete(Symbol *p_symbol);
void symbol_cleanup_hash_syms_cb(void *p_void);
void symbol_print(Symbol *p_symbol, char *c_key);
void symbol_print_cb(void *p_void, char *c_key);
char* sym_get_name(SymbolType sym);

#endif
