#ifndef _MVS_ARG_DEFS_
#define _MVS_ARG_DEFS_

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 8

#define _MVS_HELP_MSG_ \
    "Usage: mvs [options]....\n" \
    "Options:\n" \
    "-h, --help                Display this help message\n" \
    "-v, --version             Display MVS version\n" \
    "-log=[info/warn/err/dbg]  Set Log Level\n" \
    "-es                       Ensure that all launch commands, including " \
    "slist, are " \
    "successfully executed else terminate\n" \
    "-spawn=<N> EID <-=K>[...] Make a request to spawn 'N' instances of entity " \
    "with entity ID 'EID'\n" \
    "                          Optionally provide arguments to be passed to " \
    "the entity. The arguments\n" \
    "                          are provided after '-=K' where 'K' is the " \
    "number of arguments to be passed\n" \
    "-slist [command_file]     Provide a command file containing the spawn " \
    "commands instead of providing them\n" \
    "                          through arguments. Using a command file allows " \
    "the user to provide flags and set config\n" \
    "                          unlike -spawn where the default config and " \
    "properties are set\n"

#define _MVS_VERSION_MSG_ "MVS: v0.2.0\n"

#endif
