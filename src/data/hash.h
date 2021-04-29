/*:*
 *: File: ./src/data/hash.h
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

#ifndef HASH_H
#define HASH_H

#include "../defines.h"
#include "types.h"

typedef enum HASH_OP_ {
   FREE_ADDR,
   OCC_ADDR
} HASH_OP;

typedef struct {
   char c_key[HASH_MKEYLEN];
   char flag;
   TYPE type;
   void *p_val;
} HashElem;

typedef struct {
   HashElem *p_elems;
   unsigned i_size;
   unsigned i_cur_size;
} Hash;

Hash*hash_new(unsigned i_size);
void hash_delete(Hash *p_hash);
RETCODE hash_insert(Hash *p_hash, char *c_key, void *p_val);
RETCODE hash_insert_ht(Hash *p_hash, char *c_key, void *p_val, TYPE type);
void*hash_get(Hash *p_hash, char *c_key);
void*hash_get_ht(Hash *p_hash, char *c_key, TYPE *p_type);
void*hash_get_ht_addr(Hash *p_hash, char *c_key, TYPE *p_type, int *p_addr);
void*hash_remove(Hash *p_hash, char *c_key);
void hash_print(Hash *p_hash);
void hash_print_addrval(Hash *p_hash, int i_addr);
RETCODE hash_size(Hash *p_hash, int i_size);

int hash_getaddr(Hash *p_hash, char *c_key, HASH_OP OP);
RETCODE hash_addrisfree(Hash *p_hash, int i_addr);
RETCODE hash_addrisocc(Hash *p_hash, int i_addr, char *c_key);
int hash_nextaddr(Hash *p_hash, int i_max_tries,
                  char *c_key, int i_addr,
                  HASH_OP OP);
void hash_iterate(Hash *p_hash, void (*func)(void *));
void hash_iterate_key(Hash *p_hash, void (*func)(void *, char *));

#define hash_get_cur_size(hash) hash->i_cur_size
#define hash_get_size(hash) hash->i_size

#endif
