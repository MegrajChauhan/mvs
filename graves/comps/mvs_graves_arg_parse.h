#ifndef _MVS_GRAVES_ARG_PARSE_
#define _MVS_GRAVES_ARG_PARSE_

#include <mvs_tools.h>
#include <mvs_types.h>
#include <mvs_arg_parse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult* res);

mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult* res);

mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult* res);

mbool_t mvs_SPAWN_ENTITY_COMMAND(MVSArgParse *parser, MVSArgParseResult *res);

mbool_t mvs_SLIST(MVSArgParse *parser, MVSArgParseResult *res);

void mvs_graves_arg_parse_set_default(MVSArgParseResult* res);

#endif
