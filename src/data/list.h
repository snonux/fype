/*:*
 *: File: ./src/data/list.h
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

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#include "../defines.h"

#define list_first(l) l->p_first->p_val
#define list_last(l) l->p_last->p_val
#define listiterator_set_callback(i,cb) i->func = cb

typedef struct ListElem_ {
   struct ListElem_ *p_next;
   struct ListElem_ *p_prev;
   void *p_val;
} ListElem;

typedef struct {
   ListElem *p_first;
   ListElem *p_last;
   unsigned i_size;
} List;

typedef struct {
   ListElem *p_cur;
   _Bool b_reverse;
   void* (*func)(void *);
} ListIterator;

typedef struct {
   ListElem *p_cur;
   _Bool b_reverse;
} ListIteratorState;

List *list_new();
List *list_copy(List *p_list);
List *list_copy2(List *p_list, void* (*func)(void *));
ListElem *listelem_new();
_Bool list_empty(List *p_list);
void list_concat_front(List *p_list1, List *p_list2);
void list_concat_back(List *p_list1, List *p_list2);
void list_add_front(List *p_list, void *p_val);
void list_add_back(List *p_list, void *p_val);
void *list_remove_front(List *p_list);
void *list_remove_back(List *p_list);
void list_clear(List *p_list);
void list_clear_and_free_vals(List *p_list);
void list_delete(List *p_list);
void list_delete_cb(void *p_list);
void list_delete_and_free_vals(List *p_list);
unsigned list_size(List *p_list);
void list_iterate(List *p_list, void (*func)(void *));
void list_iterate2_ptr(List *p_list,
                       void (*func)(void *, void *),
                       void *p_void);
void list_iterate2(List *p_list,
                   void (*func)(void *, void *),
                   void *p_void);
void list_iterate3(List *p_list,
                   void (*func)(void *, void *, void *),
                   void *p_void1, void *p_void2);
void list_iterate3_ptr(List *p_list,
                       void (*func)(void *, void *, void *),
                       void *p_void1, void *p_void2);
ListIterator *listiterator_new(List *p_list);
ListIterator *listiterator_new_reverse(List *p_list);
void listiterator_delete(ListIterator *p_iter);
void *listiterator_next(ListIterator *p_iter);
void *listiterator_prev(ListIterator *p_iter);
void *listiterator_current(ListIterator *p_iter);
void *listiterator_end(ListIterator *p_iter);
_Bool listiterator_has_next(ListIterator *p_iter);
ListElem* listiterator_next_elem(ListIterator *p_iter);
void list_remove_elem(List *p_list, ListElem *p_elem_remove);
ListIteratorState* listiterator_get_state(ListIterator *p_iter);
void listiterator_set_state(ListIterator *p_iter, ListIteratorState *p_state);
void listiteratorstate_delete(ListIteratorState *p_state);

#endif
