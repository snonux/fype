#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Hash*
hash_new(unsigned i_size) {
   Hash *p_hash = (Hash *) malloc(sizeof(Hash));

   p_hash->i_size = i_size;
   p_hash->i_cur_size = 0;
   p_hash->p_elems = (HashElem *) calloc(i_size, sizeof(HashElem));

   /*Set all positions as "free" */
   for (int i = 0; i < i_size; ++i)
      p_hash->p_elems[i].flag = 'f';

   return (p_hash);
}

void
hash_delete(Hash *p_hash) {
   if (p_hash->p_elems) {
      free(p_hash->p_elems);
      p_hash->p_elems = 0;
   }

   free(p_hash);
}

RETCODE
hash_insert_ht(Hash *p_hash, char *c_key, void *p_val, TYPE type) {
   if (p_hash->i_cur_size == p_hash->i_size)
      hash_size(p_hash, p_hash->i_size *2);

   int i_addr = hash_getaddr(p_hash, c_key, FREE_ADDR);

   if (i_addr == RET_ERROR )
      return (RET_NO_SPACE);

   strncpy(p_hash->p_elems[i_addr].c_key, c_key, HASH_MKEYLEN);

   p_hash->p_elems[i_addr].flag = 'o';
   p_hash->p_elems[i_addr].type = type;
   p_hash->p_elems[i_addr].p_val = p_val;
   p_hash->i_cur_size++;

   return (RET_OK);
}

RETCODE
hash_insert(Hash *p_hash, char *c_key, void *p_val) {
   return (hash_insert_ht(p_hash, c_key, p_val, TYPE_VOIDP));
}

void*
hash_remove(Hash *p_hash, char *c_key) {
   if (p_hash->i_cur_size < p_hash->i_size / 3)
      hash_size(p_hash, p_hash->i_size / 2);

   int i_addr = hash_getaddr(p_hash, c_key, OCC_ADDR);

   if (i_addr == -1 )
      return (NULL);

   void *p_val = p_hash->p_elems[i_addr].p_val;
   p_hash->p_elems[i_addr].flag = 'm';
   p_hash->p_elems[i_addr].p_val = 0;
   --p_hash->i_cur_size;

   return (p_val);
}

void*
hash_get_ht(Hash *p_hash, char *c_key, TYPE *p_type) {
   int i_addr;
   return (hash_get_ht_addr(p_hash, c_key, p_type, &i_addr));
}

void*
hash_get_ht_addr(Hash *p_hash, char *c_key, TYPE *p_type, int *p_addr) {
   int i_addr = *p_addr = hash_getaddr(p_hash, c_key, OCC_ADDR);

   if (i_addr == -1 )
      return (NULL);

   *p_type = p_hash->p_elems[i_addr].type;
   return (p_hash->p_elems[i_addr].p_val);
}

void*
hash_get(Hash *p_hash, char *c_key) {
   TYPE type;
   return (hash_get_ht(p_hash, c_key, &type));
}

int
hash_getaddr(Hash *p_hash, char *c_key, HASH_OP OP) {
   int i_len = strlen(c_key);
   int i_addr = 0;

   if (i_len > HASH_MKEYLEN) {
      ERROR(": Key length %d is greater than HASH_MKEYLEN = %d!",
            i_len, HASH_MKEYLEN);
      //i_len = HASH_MKEYLEN;
   }

   for (int i= 0; i < i_len; ++i)
      i_addr = (i_addr *p_hash->i_size + (int) c_key[i]) % p_hash->i_size;

   switch (OP) {
   case FREE_ADDR:
      if (!hash_addrisfree(p_hash,i_addr))
         return (i_addr);
      break;

   case OCC_ADDR:
      if (!hash_addrisocc(p_hash,i_addr, c_key))
         return (i_addr);
      break;

   default:
      return (RET_ERROR);
   }

   return (hash_nextaddr(p_hash, p_hash->i_size, c_key, i_addr, OP));
}

RETCODE
hash_addrisfree(Hash *p_hash, int i_addr) {
   if (p_hash->p_elems[i_addr].flag == 'f' ||
         p_hash->p_elems[i_addr].flag == 'm')
      return (RET_OK);

   return (RET_ERROR);
}

RETCODE
hash_addrisocc(Hash *p_hash, int i_addr, char *c_key) {
   if (p_hash->p_elems[i_addr].flag == 'o' &&
         !strcmp(p_hash->p_elems[i_addr].c_key, c_key))
      return (RET_OK);

   return (RET_ERROR);
}

int
hash_nextaddr(Hash *p_hash, int i_max_tries, char *c_key, int i_addr,
              HASH_OP OP) {
   if ( --i_max_tries < 0 )
      return (RET_ERROR);

   i_addr = (i_addr + 1) % p_hash->i_size;

   switch (OP) {
   case FREE_ADDR:
      if (!hash_addrisfree(p_hash,i_addr))
         return (i_addr);
      break;

   case OCC_ADDR:
      if (!hash_addrisocc(p_hash,i_addr, c_key))
         return (i_addr);
      break;
   }

   return (hash_nextaddr(p_hash, i_max_tries, c_key, i_addr, OP));
}

void
hash_print(Hash *p_hash) {
   printf("hash_print [size:%d,cur:%d] syntax "
          " (flag[,key][=TYPE[<val>]]):\n -> ",
          p_hash->i_size,p_hash->i_cur_size);

   for (int i = 0; i < p_hash->i_size; ++i) {
      switch (p_hash->p_elems[i].flag) {
      case 'f':
         printf("(f");
         break;
      case 'm':
         printf("(m,%s=", p_hash->p_elems[i].c_key);
         hash_print_addrval(p_hash, i);
         break;
      case 'o':
         printf("(o,%s=", p_hash->p_elems[i].c_key);
         hash_print_addrval(p_hash, i);
         break;
      }
      printf(") ");
   }

   printf("\n");

}

void
hash_print_addrval(Hash *p_hash, int i_addr) {
   switch (p_hash->p_elems[i_addr].type) {
   case TYPE_NUMBER: {
      double d_val = *(double *) p_hash->p_elems[i_addr].p_val;

      if ( (int) d_val == d_val )
         printf("TYPE_NUMBER<%.0f>",d_val);
      else
         printf("TYPE_NUMBER<%f>",d_val);
   }
   break;

   case TYPE_STRING:
      printf("TYPE_STRING<%s>", (char *) p_hash->p_elems[i_addr].p_val);
      break;

   case TYPE_VOIDP:
      printf("TYPE_VOIDP");
      break;

   default:
      printf("UNKNOWN");
      break;
   }
}

RETCODE
hash_size(Hash *p_hash, int i_size) {
   if (i_size < p_hash->i_cur_size) {
      ERROR("The new hash has not enough elements"
            "to contain the old hash!");
   }

   HashElem *p_old_elems = p_hash->p_elems;
   unsigned i_old_size = p_hash->i_size;

   p_hash->p_elems = (HashElem *) calloc(i_size, sizeof(HashElem));
   p_hash->i_size = i_size;
   p_hash->i_cur_size = 0;

   /*Set all positions as "free" */
   for (int i = 0; i < i_size; ++i)
      p_hash->p_elems[i].flag = 'f';

   for (int i = 0; i < i_old_size; ++i)
      if (p_old_elems[i].flag == 'o')
         hash_insert_ht(p_hash, p_old_elems[i].c_key,
                        p_old_elems[i].p_val, p_old_elems[i].type);

   free(p_old_elems);
   return (RET_OK);
}

void
hash_iterate(Hash *p_hash, void (*func)(void *)) {
   for (int i = 0; i < p_hash->i_size; ++i)
      if (p_hash->p_elems[i].flag == 'o')
         (*func) (p_hash->p_elems[i].p_val);
}

void
hash_iterate_key(Hash *p_hash, void (*func)(void *, char *)) {
   for (int i = 0; i < p_hash->i_size; ++i)
      if (p_hash->p_elems[i].flag == 'o')
         (*func) (p_hash->p_elems[i].p_val, p_hash->p_elems[i].c_key);
}
