#include "tupel.h"

#include <stdlib.h>

Tupel*
tupel_new() {
   Tupel *p_tupel = (Tupel *) malloc(sizeof(Tupel));

   p_tupel->a = NULL;
   p_tupel->b = NULL;
   p_tupel->c = NULL;

   return p_tupel;
}

void
tupel_delete(Tupel *p_tupel) {
   free(p_tupel);
}
