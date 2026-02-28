#ifndef SCOPE_H
#define SCOPE_H

#include "../data/hash.h"
#include "../data/stack.h"
#include "../defines.h"
#include "symbol.h"

typedef struct {
   Hash *p_hash_global;
   Stack *p_stack_scopes;
} Scope;

Scope* scope_new(Hash *p_hash_syms);
void scope_delete(Scope *p_scope);
Symbol *scope_get(Scope *p_scope, char *c_key);
Symbol *scope_remove(Scope *p_scope, char *c_key);
_Bool scope_exists(Scope *p_scope, char *c_key);
_Bool scope_newset(Scope *p_scope, char *c_key, Symbol *p_symbol);
_Bool scope_reset(Scope *p_scope, char *c_key, Symbol *p_symbol);
void scope_down(Scope *p_scope);
void scope_up(Scope *p_scope);
void scope_print(Scope *p_scope);

#endif /* SCOPE_H */
