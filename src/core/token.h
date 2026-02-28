/*:*
 *: File: ./src/core/token.h
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

#ifndef TOKEN_H
#define TOKEN_H

#include "../defines.h"
#include "../data/array.h"

#define IS_TERMINAL(t) (START_TERMINALS < t && t < END_TERMINALS)
#define IS_NOT_TERMINAL(t) !IS_TERMINAL(t)
#define IS_TYPE(t) (START_TYPES < t && t < END_TYPES)
#define IS_NOT_TYPE(t) !IS_TYPE(t)
#define IS_KEYWORD(t) (START_KEYWORDS < t && t < END_KEYWORDS)
#define IS_NOT_KEYWORD(t) !IS_KEYWORD(t)
#define IS_OPERATOR(t) (START_OPERATORS < t && t < END_OPERATORS)
#define IS_NOT_OPERATOR(t) !IS_OPERATOR(t)
#define IS_IDENT(t) (t == TT_IDENT)
#define IS_NOT_IDENT(t) !(IS_IDENT(t))
#define IS_ASSIGNABLE(t) (START_ASSIGNABLES < t && t < END_ASSIGNABLES)
#define IS_NUMERICAL(t) (START_NUMERICAL < t && t < END_NUMERICAL)
#define IS_NOT_NUMERICAL(t) !(IS_NUMERICAL(t))

#define token_get_filename(t) \
	(t->c_filename != NULL ? t->c_filename : "Code string")
#define token_get_line_nr(t) t->i_line_nr
#define token_get_pos_nr(t) t->i_pos_nr
#define token_get_tt(t) (t ? t->tt_cur : TT_NONE)
#define token_get_val(t) t->c_val
#define token_get_ival(t) t->i_val
#define token_get_dval(t) t->d_val
#define token_set_tt(t,tt) t->tt_cur = tt
#define token_set_val(t,c) t->c_val = c
#define token_set_ival(t,i) t->i_val = i
#define token_set_dval(t,d) t->d_val = d
#define token_set_pos(t, l, p) t->i_line_nr = l; t->i_pos_nr = p
#define token_get_posnr(t) t->i_pos_nr
#define token_get_linenr(t) t->i_line_nr
#define token_ref_up(t) ++t->i_ref_count
#define token_ref_down(t) --t->i_ref_count
#define TOKEN_GET_ARRAY(t) t->p_array

typedef enum {
   // Diverse
   TT_NONE,
   TT_END_OF_CODE,
   // Diverse types
   START_TERMINALS,
   START_TYPES,
   START_ASSIGNABLES,
   START_NUMERICAL,
   TT_INTEGER,
   TT_DOUBLE,
   END_NUMERICAL,
   TT_STRING,
   TT_ARRAY,
   END_ASSIGNABLES,
   TT_IDENT,
   END_TYPES,

   // Keywords
   START_KEYWORDS,
   TT_ELSE,
   TT_BREAK,
   TT_IF,
   TT_IFNOT,
   TT_RET,
   TT_CONST,
   TT_PROC,
   TT_FUNC,
   TT_MY,
   TT_WHILE,
   TT_UNTIL,
   TT_NEXT,
   TT_LOOP,
   TT_DO,
   TT_INT,
   TT_DEFINED,
   TT_UNDEF,
   TT_SYMS,
   END_KEYWORDS,

   START_PARANTS,
   TT_PARANT_CL,
   TT_PARANT_CR,
   TT_PARANT_AL,
   TT_PARANT_AR,
   TT_PARANT_L,
   TT_PARANT_R,
   END_PARANTS,

   // Operators
   START_OPERATORS,
   TT_ADD,
   TT_AND,
   TT_AAND,
   TT_ASSIGN,
   TT_COMMA,
   TT_DDOT,
   TT_DECR,
   TT_DIV,
   TT_DOT,
   TT_EQ,
   TT_GE,
   TT_GT,
   TT_INCR,
   TT_LE,
   TT_LSHIFT,
   TT_LT,
   TT_MULT,
   TT_NEQ,
   TT_NOT,
   TT_OR,
   TT_RE,
   TT_RSHIFT,
   TT_SEMICOLON,
   TT_SUB,
   TT_VID,
   TT_XOR,
   END_OPERATORS,

   END_TERMINALS,

} TokenType;

typedef struct {
   TokenType tt_cur;
   char *c_val;
   int i_val;
   double d_val;
   int i_line_nr;
   int i_pos_nr;
   char *c_filename;
   unsigned int u_token_id;
   int i_ref_count;
   Array *p_array;
} Token;

Token* token_new(char *c_val, TokenType tt_cur, int i_line_nr,
                 int i_pos_nr, char *c_filename);
Token* token_new_integer(int i_val);
Token* token_new_string(char *c_val);
Token* token_new_array(int i_size);
Token* token_new_copy(Token *p_token);
Token* token_new_(char *c_val, TokenType tt_cur, char *c_filename);
Token* token_new_dummy();
void token_copy_vals(Token *p_token_to, Token *p_token_from);
void token_delete(Token *p_token);
void token_delete_cb(void *p_token);
void token_ref_down_cb(void *p_token);
void* token_copy_cb(void *p_token);
char* tt_get_name(TokenType tt_cur);
void token_print_cb(void *p_void);
void token_print(Token *p_token);
void token_print_val(Token *p_token);
TokenType get_tt(char *c_token);

#endif
