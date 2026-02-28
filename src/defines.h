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
#define URL "<https://codeberg.org/snonux/fype>"
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
