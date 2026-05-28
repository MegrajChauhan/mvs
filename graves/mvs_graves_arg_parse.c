#include <mvs_graves_arg_parse.h>

#define _MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_ 5

_MVS_ATTR_INTERNAL_ mstr_t HELP_MSG =
    "Usage: mvs [options]....\n"
    "Options:\n"
    "-h, --help                Display this help message\n"
    "-v, --version             Display MVS version\n"
    "-log=[info/warn/err/dbg]  Set Log Level\n"
    "-es                       Ensure that all launch commands are "
    "successfully executed else terminate\n"
    "-spawn=<N> EID <-=K>[...] Make a request to spawn 'N' instances of entity "
    "with entity ID 'EID'\n"
    "                          Optionally provide arguments to be passed to "
    "the entity. The arguments are\n"
    "                          are provided after '-=K' where 'K' is the "
    "number of arguments to be passed\n"
    "-slist [command_file]     Provide a command file containing the spawn "
    "commands instead of providing them\n"
    "                          through arguments. Using a command file allows "
    "the user to provide flags and set config\n"
    "                          unlike -spawn where the default config and "
    "properties are set\n";
_MVS_ATTR_INTERNAL_ mstr_t VERSION_MSG = "MVS: v0.0.0\n";

void mvs_graves_arg_parse_set_default(MVSArgParseResult *res) {
  res->log_lvl = 2;
  res->spawn_commands = NULL;
  res->entities_to_spawn = 0;
}

mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<HELP>:\n%s", HELP_MSG);
  return mtrue;
}

mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<VERSION>:\n%s", VERSION_MSG);
  return mtrue;
}

mbool_t mvs_ES(MVSArgParse *parser, MVSArgParseResult *res) {
  res->ensure_success = mtrue;
  return mtrue;
}

mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult *r) {
  // The option is used as -log=[info/warn/err/dbg]
  mstr_t arg = mvs_arg_parse_get_arg(parser);
  msize_t len = strlen(arg);
  mstr_t type_ptr = arg + 4;
  if (len < 8) {
    fprintf(stderr, "<ArgParse>: Invalid use of '-log'.\n");
    return mfalse;
  }
  if (*type_ptr != '=') {
    fprintf(stderr,
            "<ArgParse>: Expected a '=' before a log level for '-log'\n");
    return mfalse;
  }
  type_ptr++;
  if (strcmp(type_ptr, "info") == 0) {
    r->log_lvl = 0;
  } else if (strcmp(type_ptr, "warn") == 0) {
    r->log_lvl = 1;
  } else if (strcmp(type_ptr, "err") == 0) {
    r->log_lvl = 2;
  } else if (strcmp(type_ptr, "dbg") == 0) {
    r->log_lvl = 3;
  }
  return mtrue;
}

mbool_t mvs_SPAWN_ENTITY_COMMAND(MVSArgParse *parser, MVSArgParseResult *res) {
  msize_t INSTANCE_COUNT;
  msize_t EID;
  mstr_t *args;
  msize_t ARG_COUNT;
  mstr_t arg = mvs_arg_parse_get_arg(parser);
  msize_t len = strlen(arg);
  mstr_t instance_count = arg + 6;
  mstr_t tmp;
  if (mvs_args_left_to_parse(parser) < 2) {
    fprintf(stderr,
            "<ArgParse>: SPAWN ENTITY commands require the EID as well\n");
    return mfalse;
  }

  if (len > 6) {
    if (len > 7 && *(instance_count) == '=') {
      // There is an instance number provided
      instance_count++;
      INSTANCE_COUNT = strtoull(instance_count, &tmp, 10);
      if (tmp) {
        fprintf(stderr,
                "<ArgParse>: Invalid value provided for INSTANCE COUNT '%s'\n",
                instance_count);
        return mfalse;
      }
    } else {
      fprintf(stderr, "<ArgParse>: Invalid SPAWN command given '%s'\n", arg);
      return mfalse;
    }
  } else {
    INSTANCE_COUNT = 1;
  }

  // Next, figure out the EID
  mvs_arg_parse_consume_arg(parser);
  arg = mvs_arg_parse_get_arg(parser);
  tmp = NULL;
  EID = strtoull(arg, &tmp, 10);
  if (tmp) {
    fprintf(stderr, "<ArgParse>: Invalid EID '%s' provided\n", arg);
    return mfalse;
  }
  mstr_t any_args = mvs_arg_parse_peek(parser);
  if (any_args) {
    len = strlen(any_args);
    if (len >= 3 && *any_args == '-' && *(any_args + 1) == '=') {
      // yep, we have arguments to pass
      mvs_arg_parse_consume_arg(parser);
      any_args += 2;
      tmp = NULL;
      ARG_COUNT = strtoull(any_args, &tmp, 10);
      if (tmp) {
        fprintf(stderr,
                "<ArgParse>: Arg count for spawn command[EID=%zu, "
                "INSTANCE_COUNT=%zu] is invalid '%s'\n",
                EID, INSTANCE_COUNT, any_args - 2);
        return mfalse;
      }
      if (mvs_args_left_to_parse(parser) < ARG_COUNT) {
        fprintf(stderr,
                "<ArgParse>: SPAWN ENTITY command for EID=%zu with "
                "INSTANCE_COUNT=%zu dictates %zu arguments but there isn't "
                "enough arguments provided\n",
                EID, INSTANCE_COUNT, ARG_COUNT);
        return mfalse;
      }
      args = mvs_arg_parse_arg_slice(parser);
      mvs_arg_parse_consume_args(
          parser,
          ARG_COUNT -
              1 /*since the caller, mvs_parse_all_arg, advances by 1 as well*/);
    } else {
      ARG_COUNT = 0;
      args = NULL;
    }
  }
  MVSEntitySpawnCommand *command =
      (MVSEntitySpawnCommand *)malloc(sizeof(MVSEntitySpawnCommand));
  if (command) {
    fprintf(stderr, "<ArgParse>: Registering ENTITY SPAWN command "
                    "failed[Memory allocation failure]\n");
    return mfalse;
  }
  command->EID = EID;
  command->instances = INSTANCE_COUNT;
  command->argc = ARG_COUNT;
  command->argv = args;
  command->nxt_command = res->spawn_commands;
  command->slist = mfalse;
  res->spawn_commands = command;
  res->entities_to_spawn += INSTANCE_COUNT;
  return mtrue;
}

mbool_t mvs_SLIST(MVSArgParse *parser, MVSArgParseResult *res) { return mtrue; }
