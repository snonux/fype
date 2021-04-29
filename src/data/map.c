/*:*
 *: File: ./src/data/map.c
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

#include "map.h"

Map*
map_new(int i_max_size) {
   return (map_new_named(i_max_size, "noname"));
}

Map*
map_new_named(int i_max_size, char *c_name) {
   Map *p_map = malloc(sizeof(Map));

   p_map->c_name = c_name;
   p_map->pc_keys = calloc(i_max_size, sizeof(char*));
   p_map->pp_vals = calloc(i_max_size, sizeof(void*));

   for (int i = 0; i < i_max_size; ++i) {
      p_map->pc_keys[i] = NULL;
      p_map->pp_vals[i] = NULL;
   }

   p_map->i_size = 0;
   p_map->i_max_size = i_max_size;

   return (p_map);
}

_Bool
map_empty(Map *p_map) {
   return (p_map->i_size == 0);
}

_Bool
map_full(Map *p_map) {
   return (p_map->i_size == p_map->i_max_size);
}

int
map_next_free_addr(Map *p_map) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] == NULL)
         return (i);

   ERROR("No free space left in the map (%s)", p_map->c_name);

   // This point should not be reached!
   return (0);
}

_Bool
map_insert(Map *p_map, char *c_key, void *p_val) {
   int i_free_addr = map_next_free_addr(p_map);
   int i_len = strlen(c_key);

   p_map->pc_keys[i_free_addr] = STR_NEW(i_len+1);
   strncpy(p_map->pc_keys[i_free_addr], c_key, i_len);
   p_map->pp_vals[i_free_addr] = p_val;

   ++p_map->i_size;
   return (true);
}

_Bool
map_insert2(Map *p_map, char *c_key1, char *c_key2, void *p_val) {
   char c_key[HASH_MKEYLEN];
   sprintf(c_key, "%s%s%s", c_key1, SEP, c_key2);
   return (map_insert(p_map, c_key, p_val));
}

_Bool
map_insert_if_not_exists(Map *p_map, char *c_key, void *p_val) {
   void *p_void = map_get(p_map, c_key);

   if (p_void)
      return (false);

   return (map_insert(p_map, c_key, p_val));

   return (true);
}

void
map_remove(Map *p_map, char *c_key) {
   if (map_empty(p_map))
      return;

   int i_index = map_get_addr(p_map, c_key);
   if (i_index < 0)
      return;

   free(p_map->pc_keys[i_index]);
   p_map->pc_keys[i_index] = NULL;
   p_map->pp_vals[i_index] = NULL;
   --p_map->i_size;
}

void*
map_get(Map *p_map, char *c_key) {
   if (map_empty(p_map))
      return (NULL);

   int i_index = map_get_addr(p_map, c_key);
   return (i_index >= 0 ? p_map->pp_vals[i_index] : NULL);
}

void*
map_get2(Map *p_map, char *c_key1, char *c_key2) {
   char c_key[HASH_MKEYLEN];
   sprintf(c_key, "%s%s%s", c_key1, SEP, c_key2);
   return (map_get(p_map, c_key));
}


_Bool
map_exists(Map *p_map, char *c_key) {
   if (map_empty(p_map))
      return (false);

   int i_index = map_get_addr(p_map, c_key);
   return (i_index >= 0 ? true : false);
}

_Bool
map_exists2(Map *p_map, char *c_key1, char *c_key2) {
   if (map_empty(p_map))
      return (false);

   char c_key[HASH_MKEYLEN];
   sprintf(c_key, "%s%s%s:", c_key1, SEP, c_key2);

   int i_index = map_get_addr(p_map, c_key);
   return i_index >= 0 ? true : false;
}

char*
map_get_key(Map *p_map, void *p_val) {
   if (map_empty(p_map))
      return (NULL);

   for (int i = 0; i < p_map->i_max_size; ++i)
      if ((unsigned) p_map->pp_vals[i] == (unsigned) p_val)
         return (p_map->pc_keys[i]);

   return (NULL);
}

int
map_get_addr(Map *p_map, char *c_key) {
   if (map_empty(p_map))
      return (-1);

   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL && strcmp(p_map->pc_keys[i], c_key) == 0)
         return (i);

   return (-1);
}

void
map_clear(Map *p_map) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL) {
         free(p_map->pc_keys[i]);
         p_map->pc_keys[i] = NULL;
         p_map->pp_vals[i] = NULL;
      }

   p_map->i_size = 0;
}

void
map_clear_and_free_vals(Map *p_map) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL) {
         free(p_map->pc_keys[i]);

         if (p_map->pp_vals[i])
            free(p_map->pp_vals[i]);

         p_map->pc_keys[i] = NULL;
         p_map->pp_vals[i] = NULL;
      }

   p_map->i_size = 0;
}

void
map_delete(Map *p_map) {
   map_clear(p_map);
   free(p_map);
}

void
map_delete_and_free_vals(Map *p_map) {
   map_clear_and_free_vals(p_map);
   free(p_map);
}

void
map_print(Map *p_map) {
   printf("Map<size=%d, max_size=%d>:",
          map_get_size(p_map), map_get_max_size(p_map));
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         printf("(%d,%s)", i, p_map->pc_keys[i]);
   puts("");
}

void
map_iterate(Map *p_map, void (*func) (void *)) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i]);
}

void
map_iterate_keys(Map *p_map, void (*func) (void *, char *)) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i], p_map->pc_keys[i]);
}

void
map_iterate2(Map *p_map, void (*func) (void *, void *), void *p_void) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i], p_void);
}

void
map_iterate2_keys(Map *p_map,
                  void (*func) (void *, void *, char *),
                  void *p_void) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i], p_void, p_map->pc_keys[i]);
}

void
map_iterate3(Map *p_map,
             void (*func) (void *, void *, void *),
             void *p_void1, void *p_void2) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i], p_void1, p_void2);
}

void
map_iterate3_keys(Map *p_map,
                  void (*func) (void *, void *, void *, char *),
                  void *p_void1, void *p_void2) {
   for (int i = 0; i < p_map->i_max_size; ++i)
      if (p_map->pc_keys[i] != NULL)
         (*func) (p_map->pp_vals[i], p_void1, p_void2, p_map->pc_keys[i]);
}

