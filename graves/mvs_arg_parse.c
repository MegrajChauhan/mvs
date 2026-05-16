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

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts,
                          MVSArgParseResult* res, marghdlr_t hlp) {
  while (parser->ptr < parser->argc) {
    MVSArgOption *opt = mvs_find_option_hdlr(parser, opts);
    if (!opt) {
      fprintf(stderr, "<INIT>: Unknown argument '%s'\n",
        	      parser->argv[parser->ptr]);
      hlp(parser, res);
      return mfalse;
    }
    if (!opt->hdlr(parser, res)) {
      hlp(parser, res);
      return mfalse;
    }
    parser->ptr++;
  }
  return mtrue;
}
