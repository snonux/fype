#ifndef TUPEL_H
#define TUPEL_H

typedef struct {
   void *a;
   void *b;
   void *c;
} Tupel;

Tupel *tupel_new();
void tupel_delete(Tupel *p_tupel);

#endif
