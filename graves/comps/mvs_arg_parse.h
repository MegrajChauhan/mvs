#ifndef _MVS_ARG_PARSE_
#define _MVS_ARG_PARSE_

#include <mvs_logger.h>
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
  msize_t props;
  msize_t confs;
  msize_t in_setup;
  mbool_t slist; // Was the command provided in the slist?
  msize_t argc;
  mstr_t *argv;
  MVSEntitySpawnCommand *nxt_command;
};

struct MVSArgParseResult {
  msize_t log_lvl;
  mbool_t ensure_success; // If at least one entity fails to launch, terminate
                          // entirely
  msize_t entities_to_spawn;
  MVSEntitySpawnCommand *spawn_commands;
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

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts,
                          MVSArgParseResult *res, marghdlr_t hlp);

_MVS_ATTR_ALWAYS_INLINE_ mbool_t mvs_has_arg(MVSArgParse *parser) {
  return (parser->argc > parser->ptr);
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_args_left_to_parse(MVSArgParse *parser) {
  return (parser->argc - parser->ptr);
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_arg_parse_consume_arg(MVSArgParse *parser) {
  parser->ptr++;
}

_MVS_ATTR_ALWAYS_INLINE_ mstr_t mvs_arg_parse_peek(MVSArgParse *parser) {
  return (mvs_has_arg(parser)) ? parser->argv[parser->ptr + 1] : NULL;
}

_MVS_ATTR_ALWAYS_INLINE_ mstr_t mvs_arg_parse_get_arg(MVSArgParse *parser) {
  return parser->argv[parser->ptr];
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_arg_parse_consume_args(MVSArgParse *parser,
                                                         msize_t count) {
  parser->ptr += count;
}

_MVS_ATTR_ALWAYS_INLINE_ mstr_t *mvs_arg_parse_arg_slice(MVSArgParse *parser) {
  return &parser->argv[parser->ptr];
}

#endif
