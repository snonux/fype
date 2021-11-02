/*:*
 *: File: ./src/data/dat.h
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

#ifndef DAT_H
#define DAT_H

#include "types.h"

typedef struct DatElem_ {
   struct DatElem_ *p_next;
   void *p_val;
   TYPE type;
} DatElem;

typedef struct {
   DatElem *p_first;
   DatElem *p_last;
   unsigned i_size;
} Dat;

typedef struct {
   unsigned i_left;
   Dat *p_dat;
   DatElem *p_current;
   DatElem *p_next;
} DatIter;

Dat *dat_new();
DatElem *datelem_new();
DatElem *datelem_new_t(TYPE type);
_Bool dat_empty(Dat *p_dat);
void dat_push(Dat *p_dat, void *p_val);
void dat_push_t(Dat *p_dat, void *p_val, TYPE type);
void *dat_pop(Dat *p_dat);
void *dat_pop_t(Dat *p_dat, TYPE *p_type);
void dat_clear(Dat *p_dat);
void dat_delete(Dat *p_dat);
unsigned dat_size(Dat *p_dat);
void dat_iterate(Dat *p_dat, void (*func)(void *));
void dat_iterate_t(Dat *p_dat, void (*func)(void *, TYPE));
void dat_iterate_tl(Dat *p_dat, void (*func)(void *, TYPE, _Bool));
void *dat_first(Dat *p_dat);
void *dat_second(Dat *p_dat);
void *dat_last(Dat *p_dat);
void *dat_first_t(Dat *p_dat, TYPE *p_type);
void *dat_second_t(Dat *p_dat, TYPE *p_type);
void *dat_last_t(Dat *p_dat, TYPE *p_type);

DatIter *datiter_new(Dat *p_dat);
void datiter_delete(DatIter *p_iter);
void datiter_skip(DatIter *p_iter, unsigned i_num);
void *datiter_next(DatIter *p_iter);
void *datiter_next_t(DatIter *p_iter, TYPE *p_type);
unsigned datiter_left(DatIter *p_iter);
Dat *datiter_dat(DatIter *p_iter);

#endif
