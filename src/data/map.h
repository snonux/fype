#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#define SEP "::"

typedef struct {
   char *c_name;
   char **pc_keys;
   void **pp_vals;
   int i_size;
   int i_max_size;
} Map;

Map *map_new(int i_max_size);
Map *map_new_named(int i_max_size, char *c_name);
_Bool map_empty(Map *p_map);
_Bool map_full(Map *p_map);
int map_next_free_addr(Map *p_map);
_Bool map_insert(Map *p_map, char *c_key, void *p_val);
_Bool map_insert2(Map *p_map, char *c_key1, char *c_key2, void *p_val);
_Bool map_insert_if_not_exists(Map *p_map, char *c_key, void *p_val);
void map_remove(Map *p_map, char *c_key);
void *map_get(Map *p_map, char *c_key);
void *map_get2(Map *p_map, char *c_key1, char *c_key2);
_Bool map_exists(Map *p_map, char *c_key);
_Bool map_exists2(Map *p_map, char *c_key1, char *c_key2);
char *map_get_key(Map *p_map, void *p_val);
int map_get_addr(Map *p_map, char *c_key);
void map_clear(Map *p_map);
void map_clear_and_free_vals(Map *p_map);
void map_delete(Map *p_map);
void map_delete_and_free_vals(Map *p_map);
void map_print(Map *p_map);
void map_iterate(Map *p_map, void (*func) (void *));
void map_iterate_keys(Map *p_map, void (*func) (void *, char *));
void map_iterate2(Map *p_map, void (*func) (void *, void *), void *p_void);
void map_iterate2_keys(Map *p_map,
                       void (*func) (void *, void *, char *),
                       void *p_void);
void map_iterate3(Map *p_map,
                  void (*func) (void *, void *, void *),
                  void *p_void1, void *p_void2);
void map_iterate3_keys(Map *p_map,
                       void (*func) (void *, void *, void *, char *),
                       void *p_void1, void *p_void2);

#define map_get_size(map) map->i_size
#define map_get_max_size(map) map->i_max_size

#endif
