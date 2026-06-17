#ifndef _MVS_ARG_PARSE_
#define _MVS_ARG_PARSE_

#include <mvs_arg_defs.h>
#include <mvs_logger.h>
#include <mvs_system_config.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdio.h>
#include <string.h>

typedef struct MVSEntitySpawnCommand MVSEntitySpawnCommand;
typedef struct MVSArgOption MVSArgOption;
typedef struct MVSArgParse MVSArgParse;
typedef struct MVSArgParseResult MVSArgParseResult;
typedef mbool_t (*marghdlr_t)(MVSArgParse *, MVSArgParseResult *);

struct MVSEntitySpawnCommand {
  msize_t EID;
  msize_t instances;
  msize_t argc;
  mstr_t *argv;
  MVSEntitySpawnCommand *nxt_command;
};

struct MVSArgParseResult {
  msize_t log_lvl;
  msize_t entities_to_spawn;
  MVSEntitySpawnCommand *spawn_commands;
  mstr_t slist;
};

struct MVSArgParse {
  msize_t argc;
  mstr_t *argv;
  msize_t opt_count;
  msize_t ptr;
};

struct MVSArgOption {
  mstr_t name;
  mstr_t option_name; // the format like '-h' or '--verbose' etc
  msize_t opt_len;
  mbool_t must_match;
  marghdlr_t hdlr; // to handle the option
  // The handler function will handle all of the remaining logic
};

#define _MVS_MFUNC_ARG_PARSE_INIT_(_argc, _argv, _opt_count)                   \
  (MVSArgParse){                                                               \
      .argc = (_argc), .argv = (_argv), .opt_count = (_opt_count), .ptr = 1}

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgParseResult *res);

void mvs_arg_parse_populate_config(MVSArgParseResult *res,
                                   MVSSystemConfig *conf);

#endif
