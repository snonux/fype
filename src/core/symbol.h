#ifndef SYMBOL_H
#define SYMBOL_H

#include "../defines.h"
#include "../data/list.h"

#define symbol_set_val(s,v) s->p_val = v
#define symbol_set_sym(s,st) s->sym = st
#define symbol_get_val(s) s->p_val
#define symbol_get_sym(s) s->sym
#define symbol_ref_up(s) ++s->i_refs

typedef enum {
   SYM_BUILDIN,
   SYM_CONSTANT,
   SYM_FUNCTION,
   SYM_PROCEDURE,
   SYM_ARRAY,
   SYM_VARIABLE,
} SymbolType;

/* Wraps a function's body token list and its named parameter names.
 * Every SYM_FUNCTION symbol stores a FuncDef* as its p_val. */
typedef struct {
   List *p_body;    /* function body token list */
   List *p_params;  /* list of strdup'd char* param names */
   int   i_nparams; /* cached count of p_params */
} FuncDef;

FuncDef* funcdef_new(List *p_body, List *p_params, int i_nparams);
void     funcdef_delete(FuncDef *p_funcdef);

typedef struct {
   SymbolType sym;
   void *p_val;
   unsigned i_refs;
} Symbol;

Symbol* symbol_new(SymbolType sym, void *p_val);
void symbol_delete(Symbol *p_symbol);
void symbol_cleanup_hash_syms_cb(void *p_void);
void symbol_print(Symbol *p_symbol, char *c_key);
void symbol_print_cb(void *p_void, char *c_key);
char* sym_get_name(SymbolType sym);

#endif
