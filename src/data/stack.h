/*:*
 *: File: ./src/data/stack.h
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

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef struct StackElem_ {
   struct StackElem_ *p_next;
   void *p_val;
} StackElem;

typedef struct {
   StackElem *p_first;
   StackElem *p_last; // Only needed for stack_merge
   unsigned i_size;
} Stack;

typedef struct {
   StackElem *p_current;
   StackElem *p_prev;
} StackIterator;

Stack *stack_new();
StackElem *stackelem_new();
_Bool stack_empty(Stack *p_stack);
void stack_iterate(Stack *p_stack, void (*func)(void *p_void));
void stack_iterate2(Stack *p_stack, void (*func)(void *p_void, void *p_void2),
                    void *p_void_arg);
void stack_iterate_level(Stack *p_stack, void (*func)(void *p_void,
                         int i_level));
void stack_push(Stack *p_stack, void *p_val);
void *stack_pop(Stack *p_stack);
void *stack_top(Stack *p_stack);
void stack_clear(Stack *p_stack);
void stack_delete(Stack *p_stack);
void stack_delete_and_free(Stack *p_stack);
unsigned stack_size(Stack *p_stack);
void stack_merge(Stack *p_stack, Stack *p_stack_merge);
void stack_concat(Stack *p_stack, Stack *p_stack_concat);
StackIterator* stackiterator_new(Stack *p_stack);
void stackiterator_delete(StackIterator *p_iter);
_Bool stackiterator_has_next(StackIterator *p_iter);
void* stackiterator_next(StackIterator *p_iter);
_Bool stackiterator_remove_prev(StackIterator *p_iter);

#endif
