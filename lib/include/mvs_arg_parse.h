#ifndef _MVS_ARG_PARSE_
#define _MVS_ARG_PARSE_

#include <mvs_tools.h>
#include <mvs_types.h>
#include <mvs_logger.h>
#include <stdio.h>
#include <string.h>

typedef struct MVSArgOption MVSArgOption;
typedef struct MVSArgParse MVSArgParse;
typedef mbool_t (*marghdlr_t)(MVSArgParse *, mptr_t);

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

#define _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, opt_count)          \
  {(argc), (argv), (opt_count), 1}

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts,
                          mptr_t res, marghdlr_t hlp, mbool_t mvs, msize_t sig);

_MVS_ATTR_ALWAYS_INLINE_ mbool_t mvs_has_arg(MVSArgParse *parser) {
	return (parser->argc > parser->ptr);
}

#endif
