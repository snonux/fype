/*:*
 *: File: ./src/data/queue.h
 *: A simple interpreter
 *: 
 *: WWW		: https://codeberg.org/snonux/fype
 *: AUTHOR	: http://buetow.org
 *: E-Mail	: fype at dev.buetow.org
 *: 
 *: Copyright (c) 2005 - 2008, Paul Buetow 
 *: All rights reserved.
 *: 
 *: Redistribution and use in source and binary forms, with or without modi-
 *: fication, are permitted provided that the following conditions are met:
 *:  * Redistributions of source code must retain the above copyright
 *:    notice, this list of conditions and the following disclaimer.
 *:  * Redistributions in binary form must reproduce the above copyright
 *:    notice, this list of conditions and the following disclaimer in the
 *:    documentation and/or other materials provided with the distribution.
 *:  * Neither the name of P. B. Labs nor the names of its contributors may 
 *:    be used to endorse or promote products derived from this software 
 *:    without specific prior written permission.
 *: 
 *: THIS SOFTWARE IS PROVIDED BY PAUL C. BUETOW AS IS'' AND ANY EXPRESS OR 
 *: IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *: WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *: DISCLAIMED. IN NO EVENT SHALL PAUL C. BUETOW BE LIABLE FOR ANY DIRECT, 
 *: INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *: (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *:  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *: HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *: STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 *: IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *: POSSIBILITY OF SUCH DAMAGE.
 *:*/

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
