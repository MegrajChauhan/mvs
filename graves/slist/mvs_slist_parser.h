#ifndef _MVS_SLIST_PARSER_
#define _MVS_SLIST_PARSER_

/*
 * The parser will parse the commands and create nodes that are turned to
 * something executable
 * */

#include <mvs_graves_arg_parse.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_slist_lexer.h>
#include <mvs_slist_node.h>
#include <mvs_types.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSSlistParser MVSSlistParser;

struct MVSSlistParser {
  mstr_t file_path;
  mbool_t metadata_read;
  mbool_t footer_read;
  msize_t max_entity_count;
  msize_t curr_id_count;
  MVSSlistLexer *lexer;
  MVSDynamicListLinear *command_list;
  MVSArgParseResult *cmd;
};

MVSSlistParser *mvs_slist_parser_create(MVSArgParseResult *cmd);

void mvs_slist_parser_destroy(MVSSlistParser *p);

void mvs_slist_parser_destroy_command(MVSSlistCommand *c);

mbool_t mvs_slist_parser_init(MVSSlistParser *p, mstr_t file_path);

mbool_t mvs_slist_parser_build(MVSSlistParser *p);

#endif
