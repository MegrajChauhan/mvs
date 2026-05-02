#include <mvs_arg_parse.h>

_MVS_ATTR_INTERNAL_ MVSArgOption *
mvs_find_option_hdlr(MVSArgParse *parser, MVSArgOption *opts) {
  mstr_t arg = parser->argv[parser->ptr];
  for (msize_t i = 0; i < parser->opt_count; i++) {
    if (opts[i].must_match) {
    	if (strcmp(arg, opts[i].option_name) == 0)
      		return &opts[i];
    } else {
    	if (strncmp(arg, opts[i].option_name, opts[i].opt_len) == 0)
      		return &opts[i];
    }
  }
  return NULL;
}

_MVS_ATTR_INTERNAL_ void mvs_set_default_option_values(MVSArgParseResult* res) {
	res->log_lvl = 2;
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t mvs_has_arg(MVSArgParse *parser) {
	return (parser->argc > parser->ptr);
} 

mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<HELP>:\n%s", parser->help_msg);
  return mtrue;
}

mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<VERSION>:\n%s", parser->version_msg);
  return mtrue;
}

mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult *res) {
  // The option is used as -log=[info/warn/err/dbg]
  mstr_t arg = parser->argv[parser->ptr];
  msize_t len = strlen(arg);
  mstr_t type_ptr = arg + 4;
  if (len < 8) {
  	fprintf(stderr, "<ArgParse>: Invalid use of '-log'.\n");
  	return mfalse;
  }
  if (*type_ptr != '=') {
  	fprintf(stderr, "<ArgParse>: Expected a '=' before a log level for '-log'\n");
  	return mfalse;
  }
  type_ptr++;
  if (strcmp(type_ptr, "info") == 0) {
  	res->log_lvl = 0;
  } else if (strcmp(type_ptr, "warn") == 0) {
  	res->log_lvl = 1;
  } else if (strcmp(type_ptr, "err") == 0) {
   	res->log_lvl = 2;
  } else if (strcmp(type_ptr, "dbg") == 0) {
   	res->log_lvl = 3;
  }
  return mtrue;
}

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts,
                          MVSArgParseResult *res) {
  // All of the arguments will be valid, of course.
  mvs_set_default_option_values(res);
  if (parser->argc < 2) {
    fprintf(stderr, "<INIT>: Invalid arguments provided!\n");
    mvs_HELP_MSG(parser, res);
    return mfalse;
  }
  while (parser->ptr < parser->argc) {
    MVSArgOption *opt = mvs_find_option_hdlr(parser, opts);
    if (!opt) {
      fprintf(stderr, "<INIT>: Unknown argument '%s'\n",
              parser->argv[parser->ptr]);
      mvs_HELP_MSG(parser, res);
      return mfalse;
    }
    if (!opt->hdlr(parser, res)) {
      mvs_HELP_MSG(parser, res);
      return mfalse;
    }
    parser->ptr++;
  }
  return mtrue;
}
