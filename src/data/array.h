/*:*
 *: File: ./src/data/array.h
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

#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>

#include "../defines.h"

#define array_get_size(a) a->i_size
#define array_get_used(a) a->i_used
#define array_get_ind(a) (a->i_used - 1)
#define array_empty(a) a->i_size == 0
#define array_clear(a) array_resize(a, 0)
#define array_get_first(a) array_get(a, 0)
#define array_get_last(a) array_get(a, array_get_size(a)-1)

typedef struct {
   void *p_val;
} ArrayElement;

typedef struct {
   ArrayElement **pp_ae;
   int i_used;
   int i_size;
} Array;

typedef struct {
   Array *p_array;
   int i_cur_pos;
} ArrayIterator;

Array *array_new();
Array *array_new_size(int i_size);
Array *array_new_copy(Array *p_array);
void array_delete(Array *p_array);
void array_delete_iterate(Array *p_array, void (*func)(void*));
void array_set(Array *p_array, int i_index, void *p_val);
void array_insert(Array *p_array, int i_index, void *p_val);
void *array_remove(Array *p_array, int i_index);
void *array_get(Array *p_array, int i_index);
void array_resize(Array *p_array, int i_size);
_Bool array_defined(Array *p_array, int i_index);
void array_print_int(Array *p_array);
void array_splice(Array *p_array, int i_index, Array *p_array2);
void array_push(Array *p_array, void *p_void);
void array_append(Array *p_array, Array *p_array_append);
void array_unshift(Array *p_array, void *p_void);
void array_iterate(Array *p_array, void (*func)(void *));
void array_iterate2(Array *p_array, void (*func)(void *, void *),
                    void *p_void);
void array_set_used(Array *p_array, int i_used);

ArrayElement *arrayelement_new(void *p_val);
void arrayelement_delete(ArrayElement *p_ae);

ArrayIterator *arrayiterator_new(Array *p_array);
void arrayiterator_delete(ArrayIterator *p_arrayiterator);
_Bool arrayiterator_has_next(ArrayIterator *p_arrayiterator);
void *arrayiterator_next(ArrayIterator *p_arrayiterator);
#endif
