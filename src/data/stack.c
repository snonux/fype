/*:*
 *: File: ./src/data/stack.c
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

#include "stack.h"

#include "../defines.h"

Stack*
stack_new() {
   Stack *p_stack = (Stack *) malloc(sizeof(Stack));

   p_stack->p_first = p_stack->p_last = NULL;
   p_stack->i_size = 0;

   return (p_stack);
}

StackElem*
stackelem_new() {
   StackElem *p_elem = (StackElem *) malloc(sizeof(StackElem));

   p_elem->p_next = NULL;
   p_elem->p_val = NULL;

   return (p_elem);
}

_Bool
stack_empty(Stack *p_stack) {
   return (p_stack->i_size == 0);
}

void
stack_push(Stack *p_stack, void *p_val) {
   StackElem *p_elem = stackelem_new();

   p_elem->p_val = p_val;
   p_elem->p_next = p_stack->p_first;
   p_stack->p_first = p_elem;

   if (p_stack->p_last == NULL)
      p_stack->p_last = p_stack->p_first;

   ++p_stack->i_size;
}

void*
stack_pop(Stack *p_stack) {
   if (stack_empty(p_stack))
      return (NULL);

   StackElem *p_elem = p_stack->p_first;
   p_stack->p_first = p_elem->p_next;

   void *p_val = p_elem->p_val;
   free(p_elem);
   --p_stack->i_size;

   if (p_stack->i_size == 0)
      p_stack->p_last = NULL;

   return (p_val);
}

void*
stack_top(Stack *p_stack) {
   if (stack_empty(p_stack))
      return (NULL);

   return (p_stack->p_first->p_val);
}

void
stack_clear(Stack *p_stack) {
   for (;!stack_empty(p_stack); stack_pop(p_stack));
}

void
stack_delete(Stack *p_stack) {
   stack_clear(p_stack);
   free(p_stack);
}

void
stack_delete_and_free(Stack *p_stack) {
   for (;!stack_empty(p_stack); free(stack_pop(p_stack)));
   stack_delete(p_stack);
}

unsigned
stack_size(Stack *p_stack) {
   if (!p_stack)
      return (0);

   return (p_stack->i_size);
}

void
stack_merge(Stack *p_stack, Stack *p_stack_merge) {
   if (stack_empty(p_stack_merge))
      return;

   if (stack_empty(p_stack)) {
      p_stack->p_first = p_stack_merge->p_first;
      p_stack->p_last = p_stack_merge->p_last;
      p_stack->i_size = p_stack_merge->i_size;

   } else {
      StackElem *p_old_first = p_stack->p_first;

      p_stack->p_first = p_stack_merge->p_first;
      p_stack_merge->p_last->p_next = p_old_first;
      p_stack->i_size += p_stack_merge->i_size;
   }

   p_stack_merge->p_first = p_stack_merge->p_last = NULL;
   p_stack_merge->i_size = 0;
}

void
stack_concat(Stack *p_stack, Stack *p_stack_concat) {
   if (stack_empty(p_stack_concat))
      return;

   Stack *p_stack_tmp = stack_new();

   StackIterator *p_iter = stackiterator_new(p_stack_concat);

   while (stackiterator_has_next(p_iter))
      stack_push(p_stack_tmp, stackiterator_next(p_iter));

   stackiterator_delete(p_iter);

   while (!stack_empty(p_stack_tmp))
      stack_push(p_stack, stack_pop(p_stack_tmp));

   stack_delete(p_stack_tmp);
}

void
stack_iterate(Stack *p_stack, void (*func)(void *p_void)) {
   if (!p_stack)
      return;

   StackElem *p_elem = p_stack->p_first;

   while (p_elem) {
      (*func)(p_elem->p_val);
      p_elem = p_elem->p_next;
   }
}

void
stack_iterate2(Stack *p_stack, void (*func)(void *p_void, void *p_void2),
               void *p_void_arg) {
   if (!p_stack)
      return;

   StackElem *p_elem = p_stack->p_first;

   while (p_elem) {
      (*func)(p_elem->p_val, p_void_arg);
      p_elem = p_elem->p_next;
   }
}

void
stack_iterate_level(Stack *p_stack, void (*func)(void *p_void,
                    int i_level)) {
   if (!p_stack)
      return;

   StackElem *p_elem = p_stack->p_first;

   int i_level = 0;
   while (p_elem) {
      (*func)(p_elem->p_val, i_level++);
      p_elem = p_elem->p_next;
   }
}

StackIterator*
stackiterator_new(Stack *p_stack) {
   StackIterator *p_iter = malloc(sizeof(StackIterator));

   p_iter->p_current = p_stack->p_first;
   p_iter->p_prev = NULL;

   return (p_iter);
}

void
stackiterator_delete(StackIterator *p_iter) {
   free(p_iter);
}

void*
stackiterator_next(StackIterator *p_iter) {
   if (!p_iter)
      return (NULL);

   StackElem *p_elem = p_iter->p_current;

   if (!p_elem)
      return (NULL);

   p_iter->p_prev = p_iter->p_current;
   p_iter->p_current = p_elem->p_next;

   return (p_elem->p_val);
}

_Bool
stackiterator_has_next(StackIterator *p_iter) {
   return (p_iter->p_current ? true : false);
}

_Bool
stackiterator_remove_prev(StackIterator *p_iter) {
   StackElem *p_prev = p_iter->p_prev;

   if (p_prev == NULL)
      return (false);

   StackElem *p_next = p_prev->p_next;

   if (p_next == NULL)
      return (false);

   p_prev->p_val = p_next->p_val;
   p_prev->p_next = p_next->p_next;


   free(p_next);

   p_iter->p_current = p_prev;
   p_iter->p_prev = NULL;

   return (true);
}
