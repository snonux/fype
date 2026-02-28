#include "queue.h"

#include <stdlib.h>

Queue*
queue_new() {
   Queue *p_queue = (Queue *) malloc(sizeof(Queue));

   p_queue->p_first = 0;
   p_queue->p_last = 0;
   p_queue->i_size = 0;

   return p_queue;
}

QueueElem*
queueelem_new() {
   return queueelem_new_t(TYPE_UNKNOWN);
}

QueueElem*
queueelem_new_t(TYPE type) {
   QueueElem *p_elem = (QueueElem *) malloc(sizeof(QueueElem));

   p_elem->p_next = 0;
   p_elem->p_val = 0;
   p_elem->type = type;

   return p_elem;
}

_Bool
queue_empty(Queue *p_queue) {
   if (p_queue == NULL)
      return 0;

   return p_queue->i_size == 0;
}

void
queue_push(Queue *p_queue, void *p_val) {
   queue_push_t(p_queue, p_val, TYPE_UNKNOWN);
}

void
queue_push_t(Queue *p_queue, void *p_val, TYPE type) {
   QueueElem *p_elem = queueelem_new_t(type);
   p_elem->p_val = p_val;

   if (0 == p_queue->i_size++)
      p_queue->p_first = p_elem;
   else
      p_queue->p_last->p_next = p_elem;

   p_queue->p_last = p_elem;
}

void*
queue_pop(Queue *p_queue) {
   TYPE type;
   return queue_pop_t(p_queue, &type);
}

void*
queue_pop_t(Queue *p_queue, TYPE *p_type) {
   if (queue_empty(p_queue))
      return 0;

   QueueElem *p_elem = p_queue->p_first;
   p_queue->p_first = p_elem->p_next;

   --p_queue->i_size;

   void *p_ret =  p_elem->p_val;
   *p_type = p_elem->type;
   free(p_elem);
   return p_ret;
}

void
queue_clear(Queue *p_queue) {
   for (;!queue_empty(p_queue); queue_pop(p_queue));
}

void
queue_delete(Queue *p_queue) {
   queue_clear(p_queue);
   free(p_queue);
}

unsigned
queue_size(Queue *p_queue) {
   return p_queue->i_size;
}

void
queue_iterate(Queue *p_queue, void (*func)(void *)) {
   QueueElem *p_elem = p_queue->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val);

      p_elem = p_elem->p_next;
   }
}

void
queue_iterate_t(Queue *p_queue, void (*func)(void *, TYPE)) {
   QueueElem *p_elem = p_queue->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val, p_elem->type);

      p_elem = p_elem->p_next;
   }
}

void
queue_iterate_tl(Queue *p_queue, void (*func)(void *, TYPE, _Bool)) {
   QueueElem *p_elem = p_queue->p_first;
   while (p_elem) {
      if (p_elem->p_val)
         (*func) (p_elem->p_val, p_elem->type, p_elem->p_next == NULL);

      p_elem = p_elem->p_next;
   }
}

QueueIter*
queueiter_new(Queue *p_queue) {
   QueueIter *p_iter =
      (QueueIter *) malloc(sizeof(QueueIter));

   p_iter->p_current = NULL;
   p_iter->p_next = p_queue->p_first;
   p_iter->i_left = queue_size(p_queue);
   p_iter->p_queue = p_queue;

   return p_iter;
}

void
queueiter_delete(QueueIter *p_iter) {
   free(p_iter);
}

void*
queueiter_next(QueueIter *p_iter) {
   TYPE type;
   return queueiter_next_t(p_iter, &type);
}

void*
queueiter_next_t(QueueIter *p_iter, TYPE *p_type) {
   if (p_iter->p_next == NULL)
      return NULL;

   void *p_ret = p_iter->p_next->p_val;
   *p_type = p_iter->p_next->type;
   p_iter->p_current = p_iter->p_next;
   p_iter->p_next = p_iter->p_next->p_next;
   --p_iter->i_left;

   return p_ret;
}

unsigned
queueiter_left(QueueIter *p_iter) {
   return p_iter->i_left;
}

Queue*
queueiter_queue(QueueIter *p_iter) {
   return p_iter->p_queue;
}

