#ifndef CONVERT_H
#define CONVERT_H

#include "../defines.h"

#include "../data/stack.h"

#include "token.h"

int convert_to_integer_get(Token *p_token);
void convert_to_integer(Token *p_token);
void convert_to_double(Token *p_token);
void convert_to_string(Token *p_token);
void convert_to_array(Token *p_token);
void convert_to_tt(Token *p_token, TokenType tt);
TokenType convert_to_highest(Token *p_token1, Token *p_token2);
TokenType convert_function_arg_types_to_highest(Stack *p_stack_args, int
      i_args);

#endif
