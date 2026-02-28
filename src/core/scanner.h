#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <ctype.h>

#include "token.h"
#include "../data/dat.h"
#include "../data/list.h"
#include "../data/tupel.h"

#define scanner_get_list_token(s) s->p_list_token
#define scanner_get_fp(s) s->fp
#define scanner_get_filename(s) s->c_filename
#define scanner_get_codestring(s) s->c_codestring

typedef struct {
   int i_current_line_nr;
   int i_current_pos_nr;
   int i_num_tokenends;
   /* Index into c_codestring for inline (-e) mode; replaces the old
    * file-global _CODESTR_INDEX so the scanner is reentrant. */
   int i_codestr_index;
   char *c_filename;
   char *c_codestring;
   FILE *fp;
   List *p_list_token;
   TokenType tt_last;
} Scanner;

Scanner *scanner_new(List *p_list_token, Tupel *p_tupel_argv);
void scanner_post_task(Scanner *p_scanner);
void scanner_delete(Scanner *p_scanner);
/* Tokenise source and post-process into p_list_token.
 * Sets *c_filename_out to the source filename (NULL for inline -e mode);
 * the caller owns that pointer (do not free it). */
void scanner_run(List *p_list_token, Tupel *p_tupel_argv,
                 char **c_filename_out);
void scanner_add_token(Scanner *p_scanner, char **cc_token, int *p_token_len,
                       TokenType tt_cur);
TokenType scanner_get_tt_cur(char *c_token);
void scanner_cleanup_list_token_cb(void *p_void);

#endif
