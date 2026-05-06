#ifndef _MERRY_ARGS_PARSE_
#define _MERRY_ARGS_PARSE_

#include <mvs_arg_parse.h>
#include <merry_logger_subsystem.h>

typedef struct MerryParseResult MerryParseResult;

struct MerryParseResult {
	mstr_t inp_file_path;
};

mbool_t merry_HELP_MSG(MVSArgParse *parser, mptr_t res);
mbool_t merry_VERSION(MVSArgParse *parser, mptr_t res);
mbool_t merry_INP_FILE(MVSArgParse *parser, mptr_t r);

mbool_t merry_parse_arg(MerryParseResult *res, mstr_t *argv, msize_t argc, msize_t sig);

#endif
