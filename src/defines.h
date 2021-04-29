/*:*
 *: File: ./src/defines.h
 *: A simple interpreter
 *: 
 *: AUTHOR	: Paul Buetow http://buetow.org
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

#ifndef DEFINES_H
#define DEFINES_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"

#define COPYRIGHT "(c) Paul C. Buetow (2005 - 2008) <fype@dev.buetow.org>"
#define FYPE
#define GRAMMAR_MAP_SIZES 128
#define ARRAY_SIZE 16
#define HASH_MAXOCC 5
#define HASH_MKEYLEN 32
#define HASH_SCALE 10
#define NAME "Fype"
#define ASSEMBLER "yasm"
#define LINKER "cc"
#define SCANNER_BUFSIZE 512
#define URL "<http://github.com/snonux/fype>"
#define VERSION "Superalpha Build"



#ifndef false
#define false (_Bool)0
#endif

#ifndef true
#define true (_Bool)1
#endif

#define UNLESS(x) if (!x)
// Makes the compiler always happy (end of switch statements) :)
#define NO_DEFAULT default: if (0)

#define EPRINTF(...) fprintf(stdout, __VA_ARGS__)
#define ERROR(...) { fprintf(stdout, __VA_ARGS__); \
	fprintf(stdout, " (%s @ %s line %d)\n", NAME, __FILE__, __LINE__); \
	exit(1); }
#define DPRINTF(...) printf("DEBUG("); printf(__VA_ARGS__); printf(")\n");

//#define DEBUG_GC
//#define DEBUG_TOKEN_REFCOUNT
//#define DEBUG_FUNCTION_PROCESS
//#define DEBUG_TRACK
//#define DEBUG_BLOCK_GET
//#define DEBUG_EXPRESSION_GET

#ifdef DEBUG_TRACK
#define TRACK \
		DPRINTF("Track: %s:%s:%d", \
			__FILE__, __FUNCTION__, __LINE__); \
		if (p_interpret && p_interpret->p_token) \
			DPRINTF("Token: %s", token_get_val(p_interpret->p_token));
#else
#define TRACK
#endif /* DEBUG_TRACK */

#define STR_NEW(len) (char *) calloc(len, sizeof(char))
#define STR_RESIZE(str, len) (char *) realloc(str, len *sizeof(char))
#define STRLST(str) str[strlen(str)-1]

#endif
