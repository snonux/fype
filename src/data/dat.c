#include "dat.h"

#include <stdlib.h>
#include "../defines.h"

Dat*
dat_new() {
   Dat *p_dat = (Dat *) malloc(sizeof(Dat));

   p_dat->p_first = 0;
   p_dat->p_last = 0;
   p_dat->i_size = 0;

   return (p_dat);
}

DatElem*
datelem_new() {
   return (datelem_new_t(TYPE_UNKNOWN));
}

DatElem*
datelem_new_t(TYPE type) {
   DatElem *p_elem = (DatElem *) malloc(sizeof(DatElem));

   p_elem->p_next = 0;
   p_elem->p_val = 0;
   p_elem->type = type;

   return (p_elem);
}

_Bool
dat_empty(Dat *p_dat) {
   if (p_dat == NULL)
      return (false);

   return (p_dat->i_size == 0);
}

void
dat_push(Dat *p_dat, void *p_val) {
   dat_push_t(p_dat, p_val, TYPE_UNKNOWN);
}

void
dat_push_t(Dat *p_dat, void *p_val, TYPE type) {
   DatElem *p_elem = datelem_new_t(type);
   p_elem->p_val = p_val;

   if (0 == p_dat->i_size++)
      p_dat->p_first = p_elem;
   else
      p_dat->p_last->p_next = p_elem;

   p_dat->p_last = p_elem;
}

void*
dat_pop(Dat *p_dat) {
   TYPE type;
   return (dat_pop_t(p_dat, &type));
}

void*
dat_pop_t(Dat *p_dat, TYPE *p_type) {
   if (dat_empty(p_dat))
      return (NULL);

   DatElem *p_elem = p_dat->p_first;
   p_dat->p_first = p_elem->p_next;

   --p_dat->i_size;

   void *p_ret =  p_elem->p_val;
   *p_type = p_elem->type;
   free(p_elem);
   return (p_ret);
}

void
dat_clear(Dat *p_dat) {
   for (;!dat_empty(p_dat); dat_pop(p_dat));
}

void
dat_delete(Dat *p_dat) {
   dat_clear(p_dat);
   free(p_dat);
}

unsigned
dat_size(Dat *p_dat) {
   return (p_dat->i_size);
}

void
dat_iterate(Dat *p_dat, void (*func)(void *)) {
   DatElem *p_elem = p_dat->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val);

      p_elem = p_elem->p_next;
   }
}

void
dat_iterate_t(Dat *p_dat, void (*func)(void *, TYPE)) {
   DatElem *p_elem = p_dat->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val, p_elem->type);

      p_elem = p_elem->p_next;
   }
}

void
dat_iterate_tl(Dat *p_dat, void (*func)(void *, TYPE, _Bool)) {
   DatElem *p_elem = p_dat->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val, p_elem->type, p_elem->p_next == NULL);

      p_elem = p_elem->p_next;
   }
}

void*
dat_first(Dat *p_dat) {
   if (dat_empty(p_dat))
      return (NULL);

   return (p_dat->p_first->p_val);
}

void*
dat_second(Dat *p_dat) {
   if (2 > dat_size(p_dat))
      return (NULL);

   return (p_dat->p_first->p_next->p_val);
}

void*
dat_last(Dat *p_dat) {
   if (dat_empty(p_dat))
      return (NULL);

   return (p_dat->p_last->p_val);
}

void*
dat_first_t(Dat *p_dat, TYPE *p_type) {
   if (dat_empty(p_dat))
      return (NULL);

   *p_type = p_dat->p_first->type;
   return (p_dat->p_first->p_val);
}

void*
dat_second_t(Dat *p_dat, TYPE *p_type) {
   if (2 > dat_size(p_dat))
      return (NULL);

   *p_type = p_dat->p_first->p_next->type;
   return (p_dat->p_first->p_next->p_val);
}

void*
dat_last_t(Dat *p_dat, TYPE *p_type) {
   if (dat_empty(p_dat))
      return (NULL);

   *p_type = p_dat->p_last->type;
   return (p_dat->p_last->p_val);
}

DatIter*
datiter_new(Dat *p_dat) {
   DatIter *p_iter =
      (DatIter *) malloc(sizeof(DatIter));

   p_iter->p_current = NULL;
   p_iter->p_next = p_dat->p_first;
   p_iter->i_left = dat_size(p_dat);
   p_iter->p_dat = p_dat;

   return (p_iter);
}

void
datiter_delete(DatIter *p_iter) {
   free(p_iter);
}

void
datiter_skip(DatIter *p_iter, unsigned i_num) {
   for (int i = 0; i < i_num; ++i)
      datiter_next(p_iter);
}

void*
datiter_next(DatIter *p_iter) {
   TYPE type;
   return (datiter_next_t(p_iter, &type));
}

void*
datiter_next_t(DatIter *p_iter, TYPE *p_type) {
   if (p_iter->p_next == NULL)
      return (NULL);

   void *p_ret = p_iter->p_next->p_val;
   *p_type = p_iter->p_next->type;
   p_iter->p_current = p_iter->p_next;
   p_iter->p_next = p_iter->p_next->p_next;
   --p_iter->i_left;

   return (p_ret);
}

unsigned
datiter_left(DatIter *p_iter) {
   return (p_iter->i_left);
}

Dat*
datiter_dat(DatIter *p_iter) {
   return (p_iter->p_dat);
}

