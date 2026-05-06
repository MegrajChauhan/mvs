#ifndef _MVS_GRAVES_ARG_PARSE_
#define _MVS_GRAVES_ARG_PARSE_

#include <mvs_tools.h>
#include <mvs_types.h>
#include <mvs_arg_parse.h>
#include <stdio.h>
#include <string.h>

typedef struct MVSArgParseResult MVSArgParseResult;

struct MVSArgParseResult {
  msize_t log_lvl; 
};

mbool_t mvs_HELP_MSG(MVSArgParse *parser, mptr_t res);

mbool_t mvs_VERSION(MVSArgParse *parser, mptr_t res);

mbool_t mvs_LOG_LVL(MVSArgParse *parser, mptr_t res);

void mvs_graves_arg_parse_set_default(MVSArgParseResult* res);

#endif
