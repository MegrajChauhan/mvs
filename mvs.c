#include <mvs_graves_arg_parse.h>
#include <mvs_system_config.h>
#include <mvs_arg_parse.h>
#include <mvs_graves.h>
#include <mvs_rlist.h>

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 7

MVSArgOption opts[_MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_] = {
   {"help message", "-h", 2, mtrue, mvs_HELP_MSG},
   {"help message", "--help", 6, mtrue, mvs_HELP_MSG},
   {"version information", "-v", 2, mtrue, mvs_VERSION},
   {"version information", "--version", 9, mtrue, mvs_VERSION}, 
   {"set log level", "-log", 4, mfalse, mvs_LOG_LVL},	
   {"Provide entity spawn commands", "-spawn", 6, mfalse, mvs_SPAWN_ENTITY_COMMAND},
   {"Ensure all spawn commands are executed", "-es", 3, mtrue, mvs_ES}
};

void mvs_init_system_config(MVSSystemConfig *conf) {
   // This may involve reading configuration files
   conf->MAX_EID = 0;
}

int main(int argc, mstr_t *argv) {
// TODO: Move this shit into Graves and make it all work
   MVSSystemConfig config = {0};
   MVSRlist rlist;
   mvs_init_system_config(&config);
   if (!mvs_rlist_init(&rlist))
		   return -1;
   if (!mvs_rlist_read(&rlist)) {
	       mvs_rlist_destroy(&rlist);
		   return -1;
   }
   config.MAX_EID = rlist.count; // TODO: There could be a better way to do this
   MVSArgParse parser = _MVS_MFUNC_ARG_PARSE_INIT_(argc, argv, _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_);
   MVSArgParseResult res;
   mvs_graves_arg_parse_set_default(&res);
   if (!mvs_parse_all_arg(&parser, opts, &res, mvs_HELP_MSG)) {
   	fprintf(stderr, "Terminating...\n");
   	return -1;
   }
   mvs_run(&res, &rlist, &config);
   return 0;
}
