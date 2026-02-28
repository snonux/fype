#include <stdio.h>
#include <stdlib.h>

#include "argv.h"
#include "data/hash.h"
#include "data/list.h"
#include "data/tupel.h"

#ifndef FYPE_H
#define FYPE_H

typedef struct {
   Tupel *p_tupel_argv; // Contains command line options
   List *p_list_token; // Initial list of token
   Hash *p_hash_syms; // Symbol table
   char *c_basename;
} Fype;

Fype *fype_new();
void fype_delete(Fype *p_fype);
int fype_run(int i_argc, char **pc_argv);

#endif /* FYPE_H */
