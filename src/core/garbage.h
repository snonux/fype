#ifndef GARBAGE_H
#define GARBAGE_H

#include "../defines.h"
#include "../data/list.h"
#include "token.h"

typedef enum {
   GC_TOKEN,
} GarbageType;

void garbage_init();
void garbage_destroy();
int garbage_collect();
void garbage_add(void *p, GarbageType type);
void garbage_add2(void *p, void (*p_func)(void*),
                  int *p_ref_count,
                  GarbageType type);
void garbage_add3(void *p, void (*p_func)(void*),
                  void (*p_print)(void*),
                  int *p_ref_count,
                  GarbageType type);
void garbage_add_token(Token *p_token);

#endif
