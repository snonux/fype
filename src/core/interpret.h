/*:*
 *: File: ./src/core/interpret.h
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

#ifndef INTERPRET_H
#define INTERPRET_H

#include "../data/list.h"
#include "../data/stack.h"
#include "../data/hash.h"

#include "../fype.h"

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
void interpret_run(Fype *p_type);
int interpret_process(Interpret *p_interpret);
int interpret_subprocess(Interpret *p_interpret, List *p_list_token);

#endif /* INTERPRET_H */
