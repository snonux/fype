/*:*
 *: File: ./src/fype.c
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

#include "fype.h"

#include <string.h>

#include "argv.h"
#include "core/token.h"
#include "core/garbage.h"
#include "core/interpret.h"
#include "core/scanner.h"
#include "core/symbol.h"

Fype*
fype_new() {
   Fype *p_fype = malloc(sizeof(Fype));

   p_fype->p_hash_syms = hash_new(512);
   p_fype->p_list_token = list_new();
   p_fype->p_tupel_argv = tupel_new();
   p_fype->c_basename = NULL;

   garbage_init();

   return (p_fype);
}

void
fype_delete(Fype *p_fype) {
   argv_tupel_delete(p_fype->p_tupel_argv);

   hash_iterate(p_fype->p_hash_syms, symbol_cleanup_hash_syms_cb);
   hash_delete(p_fype->p_hash_syms);

   //list_iterate(p_fype->p_list_token, token_print_cb);
   list_iterate(p_fype->p_list_token, token_ref_down_cb);
   //list_iterate(p_fype->p_list_token, token_print_cb);
   list_delete(p_fype->p_list_token);

   if (p_fype->c_basename)
      free(p_fype->c_basename);

   garbage_destroy();
}

int
fype_run(int i_argc, char **pc_argv) {
   Fype *p_fype = fype_new();

   // argv: Maintains command line options
   argv_run(p_fype, i_argc, pc_argv);

   // scanner: Tokenise source into the token list
   char *c_filename = NULL;
   scanner_run(p_fype->p_list_token, p_fype->p_tupel_argv, &c_filename);

   // Verbose mode: print the token list after scanning
   if (argv_checkopts("TV", p_fype->p_tupel_argv))
      list_iterate(p_fype->p_list_token, token_print_cb);

   // Derive the script basename (filename minus the .fy extension)
   if (c_filename) {
      int i_len = strlen(c_filename) - 3;
      p_fype->c_basename = calloc(i_len + 1, sizeof(char));
      strncpy(p_fype->c_basename, c_filename, i_len);
      p_fype->c_basename[i_len] = 0;
   }

   // interpret: Interpret the list of token
   interpret_run(p_fype->p_list_token, p_fype->p_hash_syms);

   fype_delete(p_fype);

   return (0);
}
