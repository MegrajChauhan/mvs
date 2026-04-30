#include <mvs_arg_parse.h>

_MVS_ATTR_ALWAYS_INLINE_ MVSArgOption* mvs_find_option_hdlr(MVSArgParse *parser, MVSArgOption *opts) {
    mstr_t arg = parser->argv[parser->ptr];
	for (msize_t i = 0; i < parser->opt_count; i++) {
	    if (strncmp(arg, opts[i].option_name, opts[i].opt_len) == 0)
	       return &opts[i];
	}
	return NULL;
}

mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res) {
	fprintf(stdout, "<HELP>:\n%s", parser->help_msg);
    return mtrue;
}

mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res) {
	fprintf(stdout, "<VERSION>:\n%s", parser->version_msg);
    return mtrue;
}

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgOption *opts, MVSArgParseResult *res) {
	// All of the arguments will be valid, of course.
	if (parser->argc < 2) {
		fprintf(stderr, "<INIT>: Invalid arguments provided!\n");
        mvs_HELP_MSG(parser, res);
        return mfalse;
	}
	while (parser->ptr < parser->argc) {
		MVSArgOption *opt = mvs_find_option_hdlr(parser, opts);
        if (!opt) {
        	fprintf(stderr, "<INIT>: Unknown argument '%s'\n", parser->argv[parser->ptr]);
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
