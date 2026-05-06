#include <mvs_graves_arg_parse.h>

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 5

_MVS_ATTR_INTERNAL_ mstr_t HELP_MSG = "Usage: mvs [options]....\n"
                  "Options:\n"
                  "-h, --help                Display this help message\n"
                  "-v, --version             Display MVS version\n"
                  "-log=[info/warn/err/dbg]  Set Log Level\n"
                  ;
_MVS_ATTR_INTERNAL_ mstr_t VERSION_MSG = "MVS: v0.0.0\n";


void mvs_graves_arg_parse_set_default(MVSArgParseResult* res) {
	res->log_lvl = 2;
}

mbool_t mvs_HELP_MSG(MVSArgParse *parser, mptr_t res) {
  fprintf(stdout, "<HELP>:\n%s", HELP_MSG);
  return mtrue;
}

mbool_t mvs_VERSION(MVSArgParse *parser, mptr_t res) {
  fprintf(stdout, "<VERSION>:\n%s", VERSION_MSG);
  return mtrue;
}

mbool_t mvs_LOG_LVL(MVSArgParse *parser, mptr_t r) {
  // The option is used as -log=[info/warn/err/dbg]
  MVSArgParseResult *res = (MVSArgParseResult*)r;
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
