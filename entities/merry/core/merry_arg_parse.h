#ifndef _MERRY_ARG_PARSE_
#define _MERRY_ARG_PARSE_

/*
 * This argument parser is inspired by Graves
 * */

#include <merry_arg_defs.h>
#include <merry_config.h>
#include <merry_logger.h>
#include <mvs_types.h>
#include <stdio.h>
#include <string.h>

typedef struct MerryArgOption MerryArgOption;
typedef struct MerryArgParser MerryArgParser;
typedef struct MerryArgParseResult MerryArgParseResult;
typedef mbool_t (*arghdlr_t)(MerryArgParser *, MerryArgParseResult *);

struct MerryArgParseResult {
  mstr_t input_file;
};

struct MerryArgParser {
  msize_t argc;
  mstr_t *argv;
  msize_t opt_count;
  msize_t ptr;
};

struct MerryArgOption {
  mstr_t name;
  mstr_t option_name;
  msize_t opt_len;
  mbool_t must_match;
  arghdlr_t hdlr;
};

#define _MERRY_ARG_PARSER_INIT_(_argc, _argv, _opt_count)                      \
  (MerryArgParser){                                                            \
      .argc = (_argc), .argv = (_argv), .opt_count = (_opt_count), .ptr = 0}

mbool_t merry_parse_all_arg(MerryArgParser *parser, MerryArgParseResult *res);

void merry_arg_parse_populate_config_from_result(MerryArgParseResult *res,
                                                 MerryConfig *conf);

#endif
