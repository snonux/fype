#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef struct StackElem_ {
   struct StackElem_ *p_next;
   void *p_val;
} StackElem;

typedef struct {
   StackElem *p_first;
   StackElem *p_last; // Only needed for stack_merge
   unsigned i_size;
} Stack;

typedef struct {
   StackElem *p_current;
   StackElem *p_prev;
} StackIterator;

Stack *stack_new();
StackElem *stackelem_new();
_Bool stack_empty(Stack *p_stack);
void stack_iterate(Stack *p_stack, void (*func)(void *p_void));
void stack_iterate2(Stack *p_stack, void (*func)(void *p_void, void *p_void2),
                    void *p_void_arg);
void stack_iterate_level(Stack *p_stack, void (*func)(void *p_void,
                         int i_level));
void stack_push(Stack *p_stack, void *p_val);
void *stack_pop(Stack *p_stack);
void *stack_top(Stack *p_stack);
void stack_clear(Stack *p_stack);
void stack_delete(Stack *p_stack);
void stack_delete_and_free(Stack *p_stack);
unsigned stack_size(Stack *p_stack);
void stack_merge(Stack *p_stack, Stack *p_stack_merge);
void stack_concat(Stack *p_stack, Stack *p_stack_concat);
StackIterator* stackiterator_new(Stack *p_stack);
void stackiterator_delete(StackIterator *p_iter);
_Bool stackiterator_has_next(StackIterator *p_iter);
void* stackiterator_next(StackIterator *p_iter);
_Bool stackiterator_remove_prev(StackIterator *p_iter);

#endif
