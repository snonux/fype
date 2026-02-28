#ifndef REFERENCE_H
#define REFERENCE_H

#include "../defines.h"
#include "symbol.h"

#define reference_get_sym(r) (r->p_symbol)

typedef struct {
   Symbol *p_symbol;
} Reference;

Reference* reference_new(Symbol *p_symbol);
void reference_delete_cb(void *p_void);
void reference_delete(Reference *p_reference);

#endif /* REFERENCE_H */
