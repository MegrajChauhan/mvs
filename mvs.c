#include <mvs_graves_arg_parse.h>
#include <mvs_arg_parse.h>
#include <mvs_graves.h>

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 6

MVSArgOption opts[_MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_] = {
   {"help message", "-h", 2, mtrue, mvs_HELP_MSG},
   {"help message", "--help", 6, mtrue, mvs_HELP_MSG},
   {"version information", "-v", 2, mtrue, mvs_VERSION},
   {"version information", "--version", 9, mtrue, mvs_VERSION}, 
   {"set log level", "-log", 4, mfalse, mvs_LOG_LVL},	
   {"Provide entity spawn commands", "-spawn", 6, mfalse, mvs_SPAWN_ENTITY_COMMAND}
};

int main(int argc, mstr_t *argv) {
   MVSArgParse parser = _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_);
   MVSArgParseResult res;
   mvs_graves_arg_parse_set_default(&res);
   if (!mvs_parse_all_arg(&parser, opts, (mptr_t)(&res), mvs_HELP_MSG, mtrue, 0)) {
   	fprintf(stderr, "Terminating...\n");
   	return -1;
   }
   mvs_run(&res);
   return 0;
}
