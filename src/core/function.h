#ifndef FUNCTION_H
#define FUNCTION_H

typedef struct {
   char *c_name;
} Function;

Function* function_new();
void function_delete(Function *p_function);

#endif /* FUNCTION_H */
