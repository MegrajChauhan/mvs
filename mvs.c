#include <mvs_arg_parse.h>
#include <mvs_graves.h>

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 4

mstr_t HELP_MSG = "Usage: mvs [options]....\n"
                  "Options:\n"
                  "-h, --help                Display this help message\n"
                  "-v, --version             Display MVS version\n"
                  ;
mstr_t VERSION_MSG = "<--->";

MVSArgOption opts[_MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_] = {
   {"help message", "-h", 2, mvs_HELP_MSG},
   {"help message", "--help", 6, mvs_HELP_MSG},
   {"version information", "-v", 2, mvs_VERSION},
   {"version information", "--version", 9, mvs_VERSION}	
};

int main(int argc, mstr_t *argv) {
   MVSArgParse parser = _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_, HELP_MSG, VERSION_MSG);
   MVSArgParseResult res;
   if (!mvs_parse_all_arg(&parser, opts, &res)) {
   	fprintf(stderr, "Terminating...\n");
   	return -1;
   }
   mvs_run(&res);
   return 0;
}
