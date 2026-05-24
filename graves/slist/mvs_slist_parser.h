#ifndef _MVS_SLIST_PARSER_
#define _MVS_SLIST_PARSER_

/*
 * The parser will parse the commands and create nodes that are turned to something 
 * executable
 * */

#include <mvs_types.h>
#include <mvs_logger.h>
#include <mvs_slist_lexer.h>
#include <stdlib.h>

typedef struct MVSSlistParser MVSSlistParser;

struct MVSSlistParser {
		mstr_t file_path;
		MVSSlistLexer *lexer;
};

MVSSlistParser *mvs_slist_parser_create();

void mvs_slist_parser_destroy(MVSSlistParser *p);

mbool_t mvs_slist_parser_init(MVSSlistParser *p, mstr_t file_path);

mbool_t mvs_slist_parser_parse(MVSSlistParser *p);

#endif
