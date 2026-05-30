#ifndef _MVS_SLIST_LEXER_
#define _MVS_SLIST_LEXER_

#include <mvs_logger.h>
#include <mvs_slist_reader.h>
#include <mvs_slist_token.h>
#include <mvs_types.h>
#include <stdlib.h>

typedef struct MVSSlistLexer MVSSlistLexer;

struct MVSSlistLexer {
  mstr_t file_path;
  MVSSlistReader *reader;
};

MVSSlistLexer *mvs_slist_lexer_create();

void mvs_slist_lexer_destroy(MVSSlistLexer *l);

mbool_t mvs_slist_lexer_init(MVSSlistLexer *l, mstr_t file_path);

MVSSlistToken mvs_slist_lexer_next_token(MVSSlistLexer *l);

MVSSlistToken mvs_slist_lexer_peek_token(MVSSlistLexer *l);

mstr_t mvs_slist_lexer_get_block(MVSSlistLexer *l, char block_borders);
