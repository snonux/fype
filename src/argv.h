#ifndef ARGV_H
#define ARGV_H

#include "data/tupel.h"
#include "fype.h"

void argv_run(Fype *p_fype, int i_argc, char **pc_argv);
void argv_switch(char c_arg, Tupel *p_tupel_argv, unsigned i_argc_left);
void argv_synopsis(Tupel *p_tupel_argv);
void argv_help();
void argv_addopt(char c_opt, Tupel *p_tupel_argv);
_Bool argv_checkopt(char c_opt, Tupel *p_tupel_argv);
_Bool argv_checkopts(char *c_opts, Tupel *p_tupel_argv);
void argv_check_argc(int i_required, unsigned i_argc_left, Tupel *p_tupel_argv);
void argv_tupel_delete(Tupel *p_tupel_argv);

#endif
