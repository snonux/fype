#include "../defines.h"

#include "function.h"

Function*
function_new() {
   Function *p_function = malloc(sizeof(Function));

   return (p_function);
}

void
function_delete(Function *p_function) {
   free(p_function);
}

