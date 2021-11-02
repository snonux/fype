/*:*
 *: File: ./src/core/convert.c
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

#include "convert.h"
#include "../data/array.h"

void
convert_to_integer(Token *p_token) {
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      break;
   case TT_DOUBLE:
      token_set_tt(p_token, TT_INTEGER);
      token_set_ival(p_token, (int)token_get_dval(p_token));
      break;
   case TT_STRING:
      token_set_tt(p_token, TT_INTEGER);
      token_set_ival(p_token, atoi(token_get_val(p_token)));
      break;
   case TT_ARRAY:
      token_set_tt(p_token, TT_INTEGER);
      token_set_ival(p_token, array_get_size(p_token->p_array));
      break;
   default:
      ERROR("Ouups(%s)", tt_get_name(token_get_tt(p_token)));
      break;
   }
}

int
convert_to_integer_get(Token *p_token) {
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      return (token_get_ival(p_token));
   case TT_DOUBLE:
      return ((int) token_get_dval(p_token));
   case TT_STRING:
      return (atoi(token_get_val(p_token)));
   case TT_ARRAY:
      return (array_get_size(p_token->p_array));
      break;
   default:
      ERROR("Ouups(%s)", tt_get_name(token_get_tt(p_token)));
   }

   return (0); /* Never reach this point */
}

void
convert_to_double(Token *p_token) {
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
      token_set_tt(p_token, TT_DOUBLE);
      token_set_dval(p_token, token_get_ival(p_token));
      break;
   case TT_DOUBLE:
      break;
   case TT_STRING:
      token_set_tt(p_token, TT_DOUBLE);
      token_set_dval(p_token, atof(token_get_val(p_token)));
      break;
   case TT_ARRAY:
      token_set_tt(p_token, TT_DOUBLE);
      token_set_dval(p_token, array_get_size(p_token->p_array));
      break;
   default:
      token_print_val(p_token);
      ERROR("Datatype conversion error '%s'", token_get_val(p_token));
      break;
   }
}

void
convert_to_string(Token *p_token) {
   switch (token_get_tt(p_token)) {
   case TT_INTEGER:
   {
      token_set_tt(p_token, TT_STRING);
      char c_tmp[1024];
      sprintf(c_tmp, "%d", token_get_ival(p_token));
      int i_len = strlen(c_tmp);
      p_token->c_val = realloc(p_token->c_val, sizeof(char) * (i_len + 1));
      strcpy(p_token->c_val, c_tmp);
      p_token->c_val[i_len] = 0;
   }
   break;
   case TT_DOUBLE:
   {
      token_set_tt(p_token, TT_STRING);
      char c_tmp[1024];
      sprintf(c_tmp, "%f", token_get_dval(p_token));
      int i_len = strlen(c_tmp);
      p_token->c_val = realloc(p_token->c_val, sizeof(char) * (i_len + 1));
      strcpy(p_token->c_val, c_tmp);
      p_token->c_val[i_len] = 0;
   }
   case TT_ARRAY:
   {
      token_set_tt(p_token, TT_STRING);
      char c_tmp[1024];
      sprintf(c_tmp, "%d", array_get_size(p_token->p_array));
      int i_len = strlen(c_tmp);
      p_token->c_val = realloc(p_token->c_val, sizeof(char) * (i_len + 1));
      strcpy(p_token->c_val, c_tmp);
      p_token->c_val[i_len] = 0;
   }
   break;
   case TT_STRING:
      break;
   default:
      ERROR("Datatype conversion error");
      break;
   }
}

void
convert_to_array(Token *p_token) {
   ERROR("not yet implemented");
}

void
convert_to_tt(Token *p_token, TokenType tt) {
   switch (tt) {
   case TT_INTEGER:
      convert_to_integer(p_token);
      break;
   case TT_DOUBLE:
      convert_to_double(p_token);
      break;
   case TT_STRING:
      convert_to_string(p_token);
      break;
   case TT_ARRAY:
      convert_to_array(p_token);
      break;
   default:
      ERROR("Ouups!");
   }
}

TokenType
convert_to_highest(Token *p_token1, Token *p_token2) {
   TokenType tt_highest = token_get_tt(p_token1);

   if (tt_highest < token_get_tt(p_token2)) {
      tt_highest = token_get_tt(p_token2);

      convert_to_tt(p_token1, tt_highest);

   } else {
      convert_to_tt(p_token2, tt_highest);
   }

   return (tt_highest);
}

TokenType
convert_function_arg_types_to_highest(Stack *p_stack_args, int i_args) {

   if (i_args <= 0)
      i_args = stack_size(p_stack_args);

   StackIterator *p_iter = stackiterator_new(p_stack_args);
   TokenType tt_highest = TT_INTEGER;

   for (int i = 0; i < i_args && stackiterator_has_next(p_iter); ++i) {
      Token *p_token = stackiterator_next(p_iter);
      if (token_get_tt(p_token) > tt_highest)
         tt_highest = token_get_tt(p_token);
   }

   stackiterator_delete(p_iter);
   p_iter = stackiterator_new(p_stack_args);

   for (int i = 0; i < i_args && stackiterator_has_next(p_iter); ++i) {
      Token *p_token = stackiterator_next(p_iter);
      convert_to_tt(p_token, tt_highest);
   }

   stackiterator_delete(p_iter);

   return (tt_highest);
}

