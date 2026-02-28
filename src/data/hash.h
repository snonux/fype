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
