#ifndef _MVS_ARG_PARSE_
#define _MVS_ARG_PARSE_

#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdio.h>
#include <string.h>

typedef struct MVSArgOption MVSArgOption;
typedef struct MVSArgParse MVSArgParse;
typedef struct MVSArgParseResult MVSArgParseResult;
typedef mbool_t (*marghdlr_t)(MVSArgParse *, MVSArgParseResult *);

struct MVSArgParseResult {
  msize_t log_lvl; 
};

struct MVSArgParse {
  msize_t argc;
  mstr_t *argv;
  msize_t opt_count;
  msize_t ptr;
  mstr_t help_msg;
  mstr_t version_msg;
};

struct MVSArgOption {
  mstr_t name;
  mstr_t option_name; // the format like '-h' or '--verbose' etc
  msize_t opt_len;
  mbool_t must_match;
  marghdlr_t hdlr; // to handle the option
  // The handler function will handle all of the remaining logic
};

#define _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, opt_count, hmsg, vmsg)          \
  {(argc), (argv), (opt_count), 1, hmsg, vmsg}

mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res);

mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res);

mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult *res);

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts,
                          MVSArgParseResult *res);

#endif
