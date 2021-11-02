/*:*
 *: File: ./src/argv.c
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

#include "argv.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "data/dat.h"

#define ARGV_OSLEN 128

char *BINARY;

void
argv_run(Fype *p_fype, int i_argc, char **pc_argv) {
   Dat *p_dat_string = dat_new();

   BINARY = pc_argv[0];
   char *c_tmp = STR_NEW(ARGV_OSLEN);
   c_tmp[0] = '\0';

   p_fype->p_tupel_argv->a = p_dat_string;
   p_fype->p_tupel_argv->b = c_tmp;

   if (i_argc <= 1) {
      argv_synopsis(p_fype->p_tupel_argv);
      exit(1);
   }

   for (int i = 1; i < i_argc; ++i) {
      if (pc_argv[i][0] != '-' && pc_argv[i][0] != '/') {
         dat_push(p_dat_string, pc_argv[i]);
         continue;
      }

      int i_len = strlen(pc_argv[i]);
      unsigned i_argc_left = i_argc - i + dat_size(p_dat_string);

      for (int j = 1; j < i_len; ++j)
         argv_switch(pc_argv[i][j], p_fype->p_tupel_argv, i_argc_left);
   }
}

void
argv_switch(char c_arg, Tupel *p_tupel_argv, unsigned i_argc_left) {
   switch (c_arg) {
   case 'e':
   case 'T':
   case 'V':
      argv_addopt(c_arg, p_tupel_argv);
      return;
   case 'h':
      argv_help();
      exit(0);
   case 's':
      argv_synopsis(p_tupel_argv);
      exit(0);
   case 'v':
      printf("%s %s %d\n", NAME, VERSION, BUILDNR);
      exit(0);
   }

   char buf[2];
   buf[0] = c_arg;
   buf[1] = '\0';

   EPRINTF("Error: No such option '%s'!\n", buf);
   exit(1);
}

void
argv_synopsis(Tupel *p_tupel_argv) {
   printf("Synopsis:\n");
   printf("%s ", BINARY);
   printf("[-[hsvTV]] file.fy\n");
   printf("%s ", BINARY);
   printf("-e \"code string;\"\n");

   if (!argv_checkopt('h', p_tupel_argv))
      printf("\t(Hint: Try -h for details)\n");
}

void
argv_help() {
   printf("%s %s %d\n", NAME, VERSION, BUILDNR);
   printf("%s\n", COPYRIGHT);
   printf("\t-e\tExecutes given code string (see synopses)\n");
   printf("\t-h\tPrints this help\n");
   printf("\t-s\tPrints the synopsis\n");
   printf("\t-v\tPrints the current version\n");
   printf("\t-T\tPrints token list after scanning\n");
   printf("\t-V\tVerbose mode: Print all possible output\n");
   exit(0);
}

void
argv_addopt(char c_opt, Tupel *p_tupel_argv) {
   char *c_opts = (char *) p_tupel_argv->b;
   int i_len = strlen(c_opts), i;

   for (i = 0; i < i_len; ++i)
      if (c_opts[i] == c_opt)
         return;

   c_opts[i] = c_opt;
   c_opts[i+1] = '\0';
}

_Bool
argv_checkopt(char c_opt, Tupel *p_tupel_argv) {
   char *c_opts = (char *) p_tupel_argv->b;
   int i_len = strlen(c_opts);

   for (int i = 0; i < i_len; ++i)
      if (c_opts[i] == c_opt)
         return 1;

   return 0;
}

_Bool
argv_checkopts(char *c_opts, Tupel *p_tupel_argv) {
   int i_len = strlen(c_opts);

   for (int i = 0; i < i_len; ++i)
      if (argv_checkopt(c_opts[i], p_tupel_argv))
         return 1;

   return 0;
}

void
argv_check_argc(int i_required, unsigned i_argc_left, Tupel *p_tupel_argv) {
   if ((unsigned) ++i_required > i_argc_left) {
      argv_synopsis(p_tupel_argv);
      EPRINTF("%d argument(s) missing!\n", i_required-i_argc_left);
      exit(1);
   }
}

void
argv_tupel_delete(Tupel *p_tupel_argv) {
   dat_delete(p_tupel_argv->a);
   free(p_tupel_argv->b);
   tupel_delete(p_tupel_argv);
}

