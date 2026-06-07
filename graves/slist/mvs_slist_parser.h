#ifndef _MVS_SLIST_PARSER_
#define _MVS_SLIST_PARSER_

/*
 * The parser will parse the commands and create nodes that are turned to
 * something executable
 * */

#include <api_entity.h>
#include <mvs_entity_defs.h>
#include <mvs_entity_registry.h>
#include <mvs_graves_arg_parse.h>
#include <mvs_graves_constants.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_slist_lexer.h>
#include <mvs_slist_node.h>
#include <mvs_types.h>
#include <mvs_system_config.h>
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
  MVSSystemConfig *config;
};

MVSSlistParser *mvs_slist_parser_create(MVSArgParseResult *cmd, MVSSystemConfig *config);

void mvs_slist_parser_destroy(MVSSlistParser *p);

void mvs_slist_parser_destroy_command(MVSSlistCommand *c);

mbool_t mvs_slist_parser_init(MVSSlistParser *p, mstr_t file_path);

mbool_t mvs_slist_parser_build(MVSSlistParser *p);

_MVS_ATTR_ALWAYS_INLINE_ msize_t
mvs_slist_parser_get_command_count(MVSSlistParser *p) {
  return p->curr_id_count;
}

_MVS_ATTR_ALWAYS_INLINE_ MVSDynamicListLinear *
mvs_slist_parser_get_commands(MVSSlistParser *p) {
  return p->command_list;
}

#endif
