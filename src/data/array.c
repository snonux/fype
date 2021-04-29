/*:*
 *: File: ./src/data/array.c
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

#include "array.h"

Array*
array_new() {
   Array *p_array = malloc(sizeof(Array));

   p_array->i_size = 0;
   p_array->pp_ae = NULL;
   array_set_used(p_array, 0);

   return (p_array);
}

Array*
array_new_size(int i_size) {
   Array *p_array = array_new();

   array_resize(p_array, i_size);

   return (p_array);
}

void
_unshift_cb(void *p_array, void *p_void) {
   array_unshift(p_array, p_void);
}

Array*
array_new_copy(Array *p_array) {
   Array *p_array_cpy = array_new_size(array_get_size(p_array));
   array_iterate2(p_array, _unshift_cb, p_array_cpy);

   return (p_array_cpy);
}

void
array_delete(Array *p_array) {
   if (!p_array)
      return;

   if (p_array->i_size)
      for (int i = p_array->i_size - 1; i >= 0; --i)
         arrayelement_delete(p_array->pp_ae[i]);

   if (p_array->pp_ae)
      free(p_array->pp_ae);

   free(p_array);
}

void
array_delete_iterate(Array *p_array, void (*func)(void *)) {
   if (!p_array)
      return;

   array_iterate(p_array, func);

   if (p_array->i_size)
      for (int i = p_array->i_size - 1; i >= 0; --i)
         arrayelement_delete(p_array->pp_ae[i]);

   if (p_array->pp_ae)
      free(p_array->pp_ae);

   free(p_array);
}

void
array_set(Array *p_array, int i_index, void *p_val) {
   if (p_array->i_size > i_index) {
      p_array->pp_ae[i_index]->p_val = p_val;

   } else {
      array_resize(p_array, i_index + 1);
      p_array->pp_ae[i_index]->p_val = p_val;
   }

   if (p_array->i_used < i_index)
      array_set_used(p_array, i_index);
}

void
array_set_used(Array *p_array, int i_used) {
   p_array->i_used = i_used;
}


void
array_insert(Array *p_array, int i_index, void *p_val) {
   if (p_array->i_size <= i_index) {
      array_set(p_array, i_index, p_val);

   } else {
      array_resize(p_array, p_array->i_size + 1);

      ArrayElement *p_ae = p_array->pp_ae[p_array->i_size-1];
      int i;
      for (i = p_array->i_size - 1; i > i_index; --i)
         p_array->pp_ae[i] = p_array->pp_ae[i-1];

      p_array->pp_ae[i] = p_ae;
      p_ae->p_val = p_val;
   }

   if (p_array->i_used < i_index)
      array_set_used(p_array, i_index);
}

void*
array_remove(Array *p_array, int i_index) {
   if (p_array->i_size <= i_index)
      return (NULL);

   ArrayElement *p_ae = p_array->pp_ae[i_index];
   void *p_ret = p_ae->p_val;
   int i;

   for (i = i_index+1; i < p_array->i_size; ++i)
      p_array->pp_ae[i-1] = p_array->pp_ae[i];

   p_array->pp_ae[i-1] = p_ae;

   array_resize(p_array, p_array->i_size - 1);
   return (p_ret);
}

void
array_print_int(Array *p_array) {
   printf("Array:");
   for (int i = 0; i < p_array->i_size; ++i)
      printf(" (%d,%d)", i, (int) array_get(p_array, i));
   printf("\n");
}

void
array_resize(Array *p_array, int i_size) {
   if (i_size == p_array->i_size)
      return;

   if (i_size < p_array->i_size)
      for (int i = p_array->i_size - 1; i >= i_size; --i)
         arrayelement_delete(p_array->pp_ae[i]);

   if (i_size == 0) {
      free(p_array->pp_ae);
      p_array->pp_ae = NULL;

   } else if (p_array->pp_ae != NULL) {
      p_array->pp_ae = realloc(p_array->pp_ae,
                               sizeof(ArrayElement) * i_size);

   } else {
      p_array->pp_ae = malloc(sizeof(ArrayElement) * i_size);
   }

   if (i_size > p_array->i_size)
      for (int i = p_array->i_size; i < i_size; ++i)
         p_array->pp_ae[i] = arrayelement_new(NULL);

   p_array->i_size = i_size;
   if (p_array->i_used > i_size)
      array_set_used(p_array, i_size);
}

void*
array_get(Array *p_array, int i_index) {
   if (p_array->i_size > i_index)
      return (p_array->pp_ae[i_index]->p_val);

   return (NULL);
}

_Bool
array_defined(Array *p_array, int i_index) {
   if (i_index >= p_array->i_size)
      return (false);

   return (p_array->pp_ae[i_index]->p_val != NULL);
}

void
array_splice(Array *p_array, int i_index, Array *p_array2) {
   if (i_index >= array_get_size(p_array))
      return;

   array_remove(p_array, i_index);

   int i_size1= array_get_size(p_array);
   int i_size2 = array_get_size(p_array2);
   int i_size = i_size1 + i_size2;

   array_resize(p_array, i_size);

   for (int i = i_size1 - 1; i >= i_index; --i)
      p_array->pp_ae[i+i_size2]->p_val = p_array->pp_ae[i]->p_val;

   for (int i = 0; i < i_size2; ++i)
      p_array->pp_ae[i+i_index]->p_val = p_array2->pp_ae[i]->p_val;

}

void
array_unshift(Array *p_array, void *p_void) {
   int i_used = array_get_used(p_array);
   array_set(p_array, i_used, p_void);
   array_set_used(p_array, 1+i_used);
}

void
array_push(Array *p_array, void *p_void) {
   int i_size = array_get_size(p_array);
   array_resize(p_array, ++i_size);

   for (int i = i_size - 1; i > 0; --i)
      p_array->pp_ae[i]->p_val = p_array->pp_ae[i-1]->p_val;

   array_set(p_array, 0, p_void);
}

void
array_append(Array *p_array, Array *p_array_append) {
   int i_size = array_get_size(p_array) + array_get_size(p_array_append);
   array_resize(p_array, i_size);
   array_iterate2(p_array_append, _unshift_cb, p_array);
}

void
array_iterate(Array *p_array, void (*func)(void *)) {
   if (!p_array)
      return;

   for (int i = 0; i < array_get_used(p_array); ++i)
      (*func) (array_get(p_array, i));
}

void
array_iterate2(Array *p_array, void (*func)(void *, void *), void *p_void) {
   if (!p_array)
      return;

   for (int i = 0; i < array_get_used(p_array); ++i)
      (*func) (array_get(p_array, i), p_void);
}

ArrayElement*
arrayelement_new(void *p_val) {
   ArrayElement *p_ae = malloc(sizeof(ArrayElement));

   p_ae->p_val = p_val;

   return (p_ae);
}

void
arrayelement_delete(ArrayElement *p_ae) {
   if (!p_ae)
      return;

   free(p_ae);
}

ArrayIterator*
arrayiterator_new(Array *p_array) {
   if (!p_array)
      return (NULL);

   ArrayIterator *p_arrayiterator = malloc(sizeof(ArrayIterator));
   p_arrayiterator->p_array = p_array;
   p_arrayiterator->i_cur_pos = 0;

   return (p_arrayiterator);
}

void
arrayiterator_delete(ArrayIterator *p_arrayiterator) {
   if (p_arrayiterator)
      free(p_arrayiterator);
}

_Bool
arrayiterator_has_next(ArrayIterator *p_arrayiterator) {
   //printf("[%d]", p_arrayiterator->p_array->i_used);
   return (p_arrayiterator->i_cur_pos <
           array_get_used(p_arrayiterator->p_array));
}

void*
arrayiterator_next(ArrayIterator *p_arrayiterator) {
   if (!arrayiterator_has_next(p_arrayiterator))
      return (NULL);

   return (array_get(p_arrayiterator->p_array, p_arrayiterator->i_cur_pos++));
}
