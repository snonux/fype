#include "reference.h"

Reference*
reference_new(Symbol *p_symbol) {
   Reference *p_reference = malloc(sizeof(Reference));

   symbol_ref_up(p_symbol);
   p_reference->p_symbol = p_symbol;

   return (p_reference);
}

void
reference_delete_cb(void *p_void) {
   reference_delete(p_void);
}

void
reference_delete(Reference *p_reference) {
   symbol_delete(p_reference->p_symbol);
   free(p_reference);
}
