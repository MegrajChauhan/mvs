#include <merry_arg_parse.h>

#define _MERRY_OPTION_COUNT_ 3 

_MVS_ATTR_INTERNAL_ mstr_t MERRY_HELP_MSG =
		"Entity: Merry\n"
		"   Ver: 0.0.0.0\n"
		"  Type: Core(64-bit)\n"
		"   EID: 0\n"
		"\nMerry allows following arguments:\n"
		"\t-h                    Display this help message\n"
        "\t-v                    Display Merry(Core64) version information\n"
        "\t-f=[path]             Provide input file\n"
		;

_MVS_ATTR_INTERNAL_ mstr_t MERRY_VERSION_MSG = "Entity Merry Version: 0.0.0.0\n";

_MVS_ATTR_INTERNAL_ void merry_set_options_to_default(MerryParseResult *res) {
	return;
}

mbool_t merry_HELP_MSG(MVSArgParse *parser, mptr_t res) {
	MINFO(MERRY_HELP_MSG);
	return mtrue;	
}

mbool_t merry_VERSION(MVSArgParse *parser, mptr_t res) {
	MINFO(MERRY_VERSION_MSG);
	return mtrue;	
}

mbool_t merry_INP_FILE(MVSArgParse *parser, mptr_t r) {
	MerryParseResult *res = (MerryParseResult*)r;
	if (res->inp_file_path) {
		MWARN("Input File Already Provided '%s'", res->inp_file_path);
		return mtrue;
	}
	mstr_t curr_arg = parser->argv[parser->ptr];
	msize_t len = strlen(curr_arg);
	if (len < 4 || *(curr_arg + 2) != '=') {
		MERR("Invalid Option '%s' provided", curr_arg);
		return mfalse;
	}
	curr_arg += 3;
	res->inp_file_path = curr_arg;
	return mtrue;
}

mbool_t merry_parse_arg(MerryParseResult *res, mstr_t *argv, msize_t argc, msize_t sig) {
	MVSArgOption opts[_MERRY_OPTION_COUNT_] = {
		{"Merry Help", "-h", 2, mtrue, merry_HELP_MSG},
		{"Merry Version", "-v", 2, mtrue, merry_VERSION},
		{"Merry Input File", "-f", 2, mfalse, merry_INP_FILE}
	};
	MVSArgParser parser = _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, _MERRY_OPTION_COUNT_);
	merry_set_options_to_default(res);
	if (!mvs_parse_all_arg(&parser, opts, (mptr_t)(res), merry_HELP_MSG, mfalse, sig)) {
		MERR("Failed to parse arguments");
		return mfalse;
	}
	return mtrue;
}
