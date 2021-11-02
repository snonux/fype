/*:*
 *: File: ./src/data/list.c
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

#include "list.h"

List*
list_new() {
   List *p_list = malloc(sizeof(List));

   p_list->p_first = NULL;
   p_list->p_last = NULL;
   p_list->i_size = 0;

   return (p_list);
}

void
_list_copy_cb(void *p_void1, void *p_cpy) {
   ListElem *p_elem = p_void1;
   list_add_back(p_cpy, p_elem->p_val);
}

List*
list_copy(List *p_list) {
   List *p_list_cpy = list_new();
   list_iterate2(p_list, _list_copy_cb, p_list_cpy);
   return (p_list_cpy);
}

List*
list_copy2(List *p_list, void* (*func)(void *)) {
   List *p_list_cpy = list_new();
   ListIterator *p_iter = listiterator_new(p_list);

   while (listiterator_has_next(p_iter))
      list_add_back(p_list_cpy, (*func) (listiterator_next(p_iter)));

   listiterator_delete(p_iter);

   return (p_list_cpy);
}

ListElem*
listelem_new() {
   ListElem *p_elem = malloc(sizeof(ListElem));

   p_elem->p_next = NULL;
   p_elem->p_prev = NULL;
   p_elem->p_val = NULL;

   return (p_elem);
}

_Bool
list_empty(List *p_list) {
   return (p_list->i_size == 0);
}

void
list_concat_front(List *p_list1, List *p_list2) {
   if (!p_list1 || !p_list2 || !p_list2->p_last)
      return;

   ListElem *p_first = p_list1->p_first;

   if (!p_first) {
      p_list1->p_first = p_list2->p_first;
      p_list1->p_last = p_list2->p_last;

   } else {
      p_list2->p_last->p_next = p_list1->p_first;
      p_list1->p_first->p_prev = p_list2->p_last;
      p_list1->p_first = p_list2->p_first;
   }

   p_list1->i_size += p_list2->i_size;
   p_list2->i_size = 0;
   p_list2->p_first = NULL;
   p_list2->p_last = NULL;
}

void
list_concat_back(List *p_list1, List *p_list2) {
   if (!p_list1 || !p_list2 || !p_list2->p_first)
      return;

   ListElem *p_last = p_list1->p_last;

   if (!p_last) {
      p_list1->p_first = p_list2->p_first;
      p_list1->p_last = p_list2->p_last;

   } else {
      p_last->p_next = p_list2->p_first;
      p_list2->p_first->p_prev = p_last;
      p_list1->p_last = p_list2->p_last;
   }

   p_list1->i_size += p_list2->i_size;
   p_list2->i_size = 0;
   p_list2->p_first = NULL;
   p_list2->p_last = NULL;

}

void
list_add_front(List *p_list, void *p_val) {
   ListElem *p_elem = listelem_new();

   p_elem->p_val = p_val;
   if (p_list->p_first == NULL) {
      p_list->p_first = p_elem;
      p_list->p_last = p_elem;

   } else {
      p_elem->p_next = p_list->p_first;
      p_list->p_first->p_prev = p_elem;
      p_list->p_first = p_elem;
   }

   ++p_list->i_size;
}

void
list_add_back(List *p_list, void *p_val) {
   ListElem *p_elem = listelem_new();

   p_elem->p_val = p_val;
   if (p_list->p_last == NULL) {
      p_list->p_first = p_elem;
      p_list->p_last = p_elem;

   } else {
      p_elem->p_prev = p_list->p_last;
      p_list->p_last->p_next = p_elem;
      p_list->p_last = p_elem;
   }

   ++p_list->i_size;
}

void*
list_remove_front(List *p_list) {
   if (list_empty(p_list))
      return (NULL);

   ListElem *p_elem = p_list->p_first;
   p_list->p_first = p_elem->p_next;

   if (p_list->p_first)
      p_list->p_first->p_prev = NULL;

   void *p_val = p_elem->p_val;
   free(p_elem);

   --p_list->i_size;

   return (p_val);
}

void*
list_remove_back(List *p_list) {
   if (list_empty(p_list))
      return (NULL);

   ListElem *p_elem = p_list->p_last;
   p_list->p_last = p_elem->p_prev;
   p_list->p_last->p_next = NULL;

   void *p_val = p_elem->p_val;
   free(p_elem);

   --p_list->i_size;

   return (p_val);
}

void
list_clear(List *p_list) {
   for (;!list_empty(p_list); list_remove_front(p_list));
}

void
list_clear_and_free_vals(List *p_list) {
   void *p_void = NULL;
   for (;!list_empty(p_list); p_void = list_remove_front(p_list))
      if (p_void)
         free(p_void);
}

void
list_delete(List *p_list) {
   list_clear(p_list);
   free(p_list);
}

void
list_delete_cb(void *p_list) {
   list_delete(p_list);
}

void
list_delete_and_free_vals(List *p_list) {
   list_clear_and_free_vals(p_list);
   free(p_list);
}

unsigned
list_size(List *p_list) {
   return (p_list->i_size);
}

void
list_iterate(List *p_list, void (*func)(void *)) {
   ListElem *p_elem = p_list->p_first;

   while (p_elem) {
      (*func) (p_elem->p_val);
      p_elem = p_elem->p_next;
   }
}

void
list_iterate2(List *p_list, void (*func)(void *, void *), void *p_void) {
   ListElem *p_elem = p_list->p_first;

   while (p_elem) {
      (*func) (p_elem->p_val, p_void);
      p_elem = p_elem->p_next;
   }
}

void
list_iterate2_ptr(List *p_list, void (*func)(void *, void *), void *p_void) {
   ListElem *p_elem = p_list->p_first;

   while (p_elem) {
      (*func) (&p_elem->p_val, p_void);
      p_elem = p_elem->p_next;
   }
}

void
list_iterate3(List *p_list,
              void (*func)(void *, void *, void *),
              void *p_void1, void *p_void2) {
   ListElem *p_elem = p_list->p_first;

   while (p_elem) {
      (*func) (p_elem->p_val, p_void1, p_void2);
      p_elem = p_elem->p_next;
   }
}

void
list_iterate3_ptr(List *p_list,
                  void (*func)(void *, void *, void *),
                  void *p_void1, void *p_void2) {
   ListElem *p_elem = p_list->p_first;

   while (p_elem) {
      (*func) (&p_elem->p_val, p_void1, p_void2);
      p_elem = p_elem->p_next;
   }
}

void
list_remove_elem(List *p_list, ListElem *p_elem_remove) {
   ListElem *p_elem = p_list->p_first;

   if (p_elem == p_elem_remove) {
      p_list->p_first = p_elem->p_next;
      p_list->p_first->p_prev = NULL;
      --p_list->i_size;
      return;
   }

   while ((p_elem = p_elem->p_next)) {
      if (p_elem == p_elem_remove) {
         ListElem *p_prev = p_elem->p_prev;
         ListElem *p_next = p_elem->p_next;

         if (p_next) {
            p_prev->p_next = p_next;
            p_next->p_prev = p_prev;

         } else {
            p_prev->p_next = NULL;
            p_list->p_last = p_prev;
         }

         --p_list->i_size;
         free(p_elem_remove);
         return;
      }
   }
}

ListIterator*
listiterator_new(List *p_list) {
   if (!p_list)
      return (NULL);

   ListIterator *p_iter = malloc(sizeof(ListIterator));

   p_iter->p_cur = p_list->p_first;
   p_iter->b_reverse = false;
   p_iter->func = NULL;

   return (p_iter);
}

ListIterator*
listiterator_new_reverse(List *p_list) {
   if (!p_list)
      return (NULL);

   ListIterator *p_iter = listiterator_new(p_list);

   p_iter->p_cur = p_list->p_last;
   p_iter->b_reverse = true;

   return (p_iter);
}

void
listiterator_delete(ListIterator *p_iter) {
   if (p_iter)
      free(p_iter);
}

void*
listiterator_next(ListIterator *p_iter) {
   if (p_iter->p_cur) {
      void *p_ret = p_iter->p_cur->p_val;

      if (p_iter->b_reverse)
         p_iter->p_cur = p_iter->p_cur->p_prev;

      else
         p_iter->p_cur = p_iter->p_cur->p_next;

      if (p_iter->func)
         return ((*p_iter->func) (p_ret));

      else
         return (p_ret);
   }

   return (NULL);
}

void*
listiterator_prev(ListIterator *p_iter) {
   if (p_iter->p_cur) {
      void *p_ret = p_iter->p_cur->p_val;

      if (!p_iter->b_reverse)
         p_iter->p_cur = p_iter->p_cur->p_prev;

      else
         p_iter->p_cur = p_iter->p_cur->p_next;

      if (p_iter->func)
         return ((*p_iter->func) (p_ret));

      else
         return (p_ret);
   }

   return (NULL);
}

void*
listiterator_current(ListIterator *p_iter) {
   if (p_iter->p_cur)
      return (p_iter->p_cur->p_val);

   return (NULL);
}

void*
listiterator_end(ListIterator *p_iter) {
   void *p_ret = NULL;

   while (listiterator_has_next(p_iter))
      p_ret = listiterator_next(p_iter);

   return (p_ret);
}

ListElem*
listiterator_next_elem(ListIterator *p_iter) {
   if (p_iter->p_cur) {
      ListElem *p_ret = p_iter->p_cur;

      p_iter->p_cur = p_iter->b_reverse ?
                      p_iter->p_cur->p_prev : p_iter->p_cur->p_next;

      return (p_ret);
   }

   return (NULL);
}

_Bool
listiterator_has_next(ListIterator *p_iter) {
   return (p_iter->p_cur != NULL ? true : false);
}


ListIteratorState*
listiterator_get_state(ListIterator *p_iter) {
   ListIteratorState *p_state = malloc(sizeof(ListIteratorState));

   p_state->p_cur = p_iter->p_cur;
   p_state->b_reverse = p_iter->b_reverse;

   return (p_state);
}

void
listiterator_set_state(ListIterator *p_iter, ListIteratorState *p_state) {
   p_iter->p_cur = p_state->p_cur;
   p_iter->b_reverse = p_state->b_reverse;
}

void
listiteratorstate_delete(ListIteratorState *p_state) {
   free(p_state);
}
