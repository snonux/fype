#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct QueueElem_ {
   struct QueueElem_ *p_next;
   void *p_val;
   TYPE type;
} QueueElem;

typedef struct {
   QueueElem *p_first;
   QueueElem *p_last;
   unsigned i_size;
} Queue;

typedef struct {
   unsigned i_left;
   Queue *p_queue;
   QueueElem *p_current;
   QueueElem *p_next;
} QueueIter;

Queue *queue_new();
QueueElem *queueelem_new();
QueueElem *queueelem_new_t(TYPE type);
_Bool queue_empty(Queue *p_queue);
void queue_push(Queue *p_queue, void *p_val);
void queue_push_t(Queue *p_queue, void *p_val, TYPE type);
void *queue_pop(Queue *p_queue);
void *queue_pop_t(Queue *p_queue, TYPE *p_type);
void queue_clear(Queue *p_queue);
void queue_delete(Queue *p_queue);
unsigned queue_size(Queue *p_queue);
void queue_iterate(Queue *p_queue, void (*func)(void *));
void queue_iterate_t(Queue *p_queue, void (*func)(void *, TYPE));
void queue_iterate_tl(Queue *p_queue, void (*func)(void *, TYPE, _Bool));

QueueIter *queueiter_new(Queue *p_queue);
void queueiter_delete(QueueIter *p_iter);
void *queueiter_next(QueueIter *p_iter);
void *queueiter_next_t(QueueIter *p_iter, TYPE *p_type);
unsigned queueiter_left(QueueIter *p_iter);
Queue *queueiter_queue(QueueIter *p_iter);

#endif
