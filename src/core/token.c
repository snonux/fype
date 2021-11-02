/*:*
 *: File: ./src/core/token.c
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

#include "token.h"

#include "garbage.h"

#define CHECK(...) if (!strcmp(c_token, __VA_ARGS__)) return

long TOKEN_ID_COUNTER = 0;

TokenType
get_tt(char *c_token) {
   CHECK("if") TT_IF;
   CHECK("ifnot") TT_IFNOT;
   CHECK("else") TT_ELSE;
   CHECK("break") TT_BREAK;
   CHECK("while") TT_WHILE;
   CHECK("until") TT_UNTIL;
   CHECK("int") TT_INT;
   CHECK("next") TT_NEXT;
   CHECK("defined") TT_DEFINED;
   CHECK("undef") TT_UNDEF;
   CHECK("syms") TT_SYMS;
   CHECK("ret") TT_RET;
   CHECK("const") TT_CONST;
   CHECK("proc") TT_PROC;
   CHECK("func") TT_FUNC;
   CHECK("my") TT_MY;
   CHECK("arr") TT_ARR;
   CHECK("!") TT_NOT;
   CHECK("!=") TT_NEQ;
   CHECK("=~") TT_RE;
   CHECK("(") TT_PARANT_L;
   CHECK(")") TT_PARANT_R;
   CHECK("[") TT_PARANT_AL;
   CHECK("]") TT_PARANT_AR;
   CHECK("*") TT_MULT;
   CHECK("+") TT_ADD;
   CHECK("++") TT_INCR;
   CHECK(",") TT_COMMA;
   CHECK("-") TT_SUB;
   CHECK("--") TT_DECR;
   CHECK(".") TT_DOT;
   CHECK("/") TT_DIV;
   CHECK("\\") TT_VID;
   CHECK(":") TT_DDOT;
   CHECK("==") TT_EQ;
   CHECK(";") TT_SEMICOLON;
   CHECK("<<") TT_LSHIFT;
   CHECK(">>") TT_RSHIFT;
   CHECK("and") TT_AND;
   CHECK("&") TT_AAND;
   CHECK("or") TT_OR;
   CHECK("xor") TT_XOR;
   CHECK("=") TT_ASSIGN;
   CHECK("<") TT_LT;
   CHECK(">") TT_GT;
   CHECK(">=") TT_GE;
   CHECK("<=") TT_LE;
   CHECK("{") TT_PARANT_CL;
   CHECK("}") TT_PARANT_CR;

   return TT_NONE;
}

#define CASE(t,r) case t: return r;

char*
tt_get_name(TokenType tt_cur) {
   switch (tt_cur) {
      CASE(START_TERMINALS, "START_TERMINALS")
      CASE(START_TYPES, "START_TYPES")
      CASE(START_NUMERICAL, "START_NUMERICAL")
      CASE(START_ASSIGNABLES, "START_ASSIGNABLES")
      CASE(TT_IDENT, "TT_IDENT")
      CASE(TT_DOUBLE,"TT_DOUBLE")
      CASE(TT_STRING,"TT_STRING")
      CASE(TT_ARRAY,"TT_ARRAY")
      CASE(TT_INTEGER,"TT_INTEGER")
      CASE(TT_BOOL,"TT_BOOL")
      CASE(END_ASSIGNABLES, "END_ASSIGNABLES")
      CASE(END_NUMERICAL, "END_NUMERICAL")
      CASE(END_TYPES, "END_TYPES")

      // Keywords
      CASE(START_KEYWORDS, "START_KEYWORDS")
      CASE(TT_ELSE,"TT_ELSE")
      CASE(TT_BREAK,"TT_BREAK")
      CASE(TT_IF,"TT_IF")
      CASE(TT_IFNOT,"TT_IFNOT")
      CASE(TT_RET,"TT_RET")
      CASE(TT_CONST,"TT_CONST")
      CASE(TT_PROC,"TT_PROC")
      CASE(TT_FUNC,"TT_FUNC")
      CASE(TT_MY,"TT_MY")
      CASE(TT_ARR,"TT_ARR")
      CASE(TT_WHILE,"TT_WHILE")
      CASE(TT_UNTIL,"TT_UNTIL")
      CASE(TT_NEXT,"TT_NEXT")
      CASE(TT_DEFINED,"TT_DEFINED")
      CASE(TT_UNDEF,"TT_UNDEF")
      CASE(TT_SYMS,"TT_SYMS")
      CASE(TT_INT,"TT_INT")
      CASE(END_KEYWORDS, "END_KEYWORDS")

      // Braces
      CASE(START_PARANTS, "START_PARANTS")
      CASE(TT_PARANT_CL,"TT_PARANT_CL")
      CASE(TT_PARANT_CR,"TT_PARANT_CR")
      CASE(TT_PARANT_AL,"TT_PARANT_AL")
      CASE(TT_PARANT_AR,"TT_PARANT_AR")
      CASE(TT_PARANT_L,"TT_PARANT_L")
      CASE(TT_PARANT_R,"TT_PARANT_R")
      CASE(END_PARANTS, "END_PARANTS")

      // Operators
      CASE(START_OPERATORS, "START_OPERATORS")
      CASE(TT_ADD,"TT_ADD")
      CASE(TT_ASSIGN,"TT_ASSIGN")
      CASE(TT_COMMA,"TT_COMMA")
      CASE(TT_DDOT,"TT_DDOT")
      CASE(TT_DECR,"TT_DECR")
      CASE(TT_DIV,"TT_DIV")
      CASE(TT_VID,"TT_VID")
      CASE(TT_DOT,"TT_DOT")
      CASE(TT_EQ,"TT_EQ")
      CASE(TT_LE,"TT_LE")
      CASE(TT_LT,"TT_LT")
      CASE(TT_GE,"TT_GE")
      CASE(TT_GT,"TT_GT")
      CASE(TT_INCR,"TT_INCR")
      CASE(TT_LSHIFT,"TT_LSHIFT")
      CASE(TT_RSHIFT,"TT_RSHIFT")
      CASE(TT_AND,"TT_AND")
      CASE(TT_AAND,"TT_AAND")
      CASE(TT_OR,"TT_OR")
      CASE(TT_XOR,"TT_XOR")
      CASE(TT_NOT,"TT_NOT")
      CASE(TT_MULT,"TT_MULT")
      CASE(TT_NEQ,"TT_NEQ")
      CASE(TT_RE,"TT_RE")
      CASE(TT_NOTEQ,"TT_NOTEQ")
      CASE(TT_SEMICOLON,"TT_SEMICOLON")
      CASE(TT_SUB,"TT_SUB")
      CASE(END_OPERATORS, "END_OPERATORS")
      CASE(END_TERMINALS, "END_TERMINALS")

      // Diverse
      CASE(TT_NONE,"TT_NONE")
      CASE(TT_END_OF_CODE,"TT_END_OF_CODE")
   }

   // Never reach this point
   return "XXXXXXX";
}

Token*
token_new(char *c_val, TokenType tt_cur, int i_line_nr,
          int i_pos_nr, char *c_filename) {
   Token *p_token = token_new_dummy();

   p_token->c_val = c_val;
   p_token->i_val = 0;
   p_token->d_val = 0;
   p_token->tt_cur = tt_cur;
   p_token->i_line_nr = i_line_nr;
   p_token->i_pos_nr = i_pos_nr;
   p_token->c_filename = c_filename;
   p_token->p_array = NULL;

   switch (tt_cur) {
   case TT_INTEGER:
      p_token->i_val = atoi(c_val);
      break;
   case TT_DOUBLE:
   {
      p_token->d_val = atof(c_val);
      break;
   }
   case TT_ARRAY:
   {
      p_token->p_array = array_new();
      break;
   }
   NO_DEFAULT;
   }

   return p_token;
}

Token*
token_new_integer(int i_val) {
   Token *p_token = token_new_dummy();
   token_set_tt(p_token, TT_INTEGER);
   token_set_ival(p_token, i_val);

   return (p_token);
}

Token*
token_new_double(double d_val) {
   Token *p_token = token_new_dummy();
   token_set_tt(p_token, TT_DOUBLE);
   token_set_dval(p_token, d_val);

   return (p_token);
}

Token*
token_new_string(char *c_val) {
   Token *p_token = token_new_dummy();
   token_set_tt(p_token, TT_STRING);

   p_token->c_val = calloc(strlen(c_val)+1, sizeof(char));
   strcpy(p_token->c_val, c_val);

   return (p_token);
}

Token*
token_new_array(int i_size) {
   Token *p_token = token_new_dummy();

   token_set_tt(p_token, TT_ARRAY);
   p_token->p_array = array_new();

   array_resize(p_token->p_array, i_size);

   return (p_token);
}


Token*
token_new_(char *c_val, TokenType tt_cur, char *c_filename) {
   return token_new(c_val, tt_cur, -1, -1, c_filename);
}

Token*
token_new_dummy() {
   Token *p_token = malloc(sizeof(Token));

   p_token->c_val = NULL;
   p_token->tt_cur = TT_NONE;
   p_token->i_line_nr = -1;
   p_token->i_pos_nr = -1;
   p_token->c_filename = NULL;
   p_token->u_token_id = TOKEN_ID_COUNTER++;
   p_token->i_ref_count = 0;
   p_token->p_array = NULL;

   /* Register the token in the garbage collector */
   garbage_add_token(p_token);

   return p_token;
}

Token*
token_new_copy(Token *p_token) {
   Token *p_token_copy = malloc(sizeof(Token));
   p_token->u_token_id = TOKEN_ID_COUNTER++;

   if (p_token_copy == NULL)
      ERROR("Memory alloc error");

   p_token->p_array = NULL;
   token_copy_vals(p_token_copy, p_token);
   p_token_copy->i_ref_count = 0;

   /* Register the token in the garbage collector */
   garbage_add_token(p_token_copy);

   return (p_token_copy);
}

void token_copy_vals(Token *p_token_to, Token *p_token_from) {
   int i_len;

   if (p_token_from->c_val) {
      i_len = strlen(p_token_from->c_val);
      p_token_to->c_val = calloc(i_len+1, sizeof(char));
      strcpy(p_token_to->c_val, p_token_from->c_val);

   } else {
      p_token_to->c_val = NULL;
   }

   p_token_to->tt_cur = p_token_from->tt_cur;
   p_token_to->i_val = p_token_from->i_val;
   p_token_to->d_val = p_token_from->d_val;
   p_token_to->i_line_nr = p_token_from->i_line_nr;
   p_token_to->i_pos_nr = p_token_from->i_pos_nr;
   p_token_to->c_filename = p_token_from->c_filename;

   if (NULL != p_token_from->p_array)
      // Copy all tokens by reference (pointers)
      p_token_to->p_array = array_new_copy(p_token_from->p_array);
}

void
token_delete_cb(void *p_void) {
   token_delete(p_void);
}

void
token_ref_down_cb(void *p_void) {
   Token *p_token = p_void;
   token_ref_down(p_token);
}

void*
token_copy_cb(void *p_void) {
   return (token_new_copy(p_void));
}

void
token_delete(Token *p_token) {
   if (token_ref_down(p_token) <= 0) {
      if (p_token->i_ref_count == 0) {
#ifdef DEBUG_TOKEN_REFCOUNT
         printf("DEBUG::TOKEN::REFCOUNT: Token ref count is 0 == %d\n",
                p_token->i_ref_count);
#endif /* DEBUG_TOKEN_REFCOUNT */
         if (p_token->c_val)
            free(p_token->c_val);

         if (NULL != p_token->p_array)
            array_delete_iterate(p_token->p_array, token_delete_cb);

         free(p_token);
      }
#ifdef DEBUG_TOKEN_REFCOUNT
      else {
         printf("DEBUG::TOKEN::REFCOUNT: Token ref count is 0 > %d\n",
                p_token->i_ref_count);
      }
#endif /* DEBUG_TOKEN_REFCOUNT */
   }
#ifdef DEBUG_TOKEN_REFCOUNT
   else {
      printf("DEBUG::TOKEN::REFCOUNT: Token ref count is 0 < %d\n",
             p_token->i_ref_count);
   }
#endif /* DEBUG_TOKEN_REFCOUNT */
}

void
token_print(Token *p_token) {
   printf("(id=%05u, line=%05d, pos=%04d, type=%s, val=%s, ival=%d, dval=%f,"
          " refs=%d)",
          p_token->u_token_id,
          p_token->i_line_nr,
          p_token->i_pos_nr,
          tt_get_name(p_token->tt_cur),
          p_token->c_val,
          p_token->i_val,
          p_token->d_val,
          p_token->i_ref_count);
}

void
token_print_val(Token *p_token) {
   TokenType tt = token_get_tt(p_token);
   switch (tt) {
   case TT_INTEGER:
      printf("(%s, %d)", tt_get_name(tt), token_get_ival(p_token));
      break;
   case TT_DOUBLE:
      printf("(%s, %f)", tt_get_name(tt), token_get_dval(p_token));
      break;
   case TT_STRING:
      printf("(%s, %s)", tt_get_name(tt), token_get_val(p_token));
      break;
   case TT_ARRAY:
      printf("(%s, size:%d)", tt_get_name(tt),
             array_get_size(p_token->p_array));
      break;
   default:
      ERROR("Ouups(%s)!", tt_get_name(tt));
   }
}

void
token_print_cb(void *p_void) {
   Token *p_token = p_void;
   printf("Token ");
   token_print(p_token);
   printf("\n");
}

