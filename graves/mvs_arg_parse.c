#include <mvs_arg_parse.h>

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_SPAWN_ENTITY_COMMAND(MVSArgParse *parser, MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_SLIST(MVSArgParse *parser, MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ void mvs_graves_arg_parse_set_default(MVSArgParseResult *res);

_MVS_ATTR_INTERNAL_ mbool_t mvs_has_arg(MVSArgParse *parser);

_MVS_ATTR_INTERNAL_ msize_t mvs_args_left_to_parse(MVSArgParse *parser);

_MVS_ATTR_INTERNAL_ void mvs_arg_parse_consume_arg(MVSArgParse *parser);

_MVS_ATTR_INTERNAL_ mstr_t mvs_arg_parse_peek(MVSArgParse *parser);

_MVS_ATTR_INTERNAL_ mstr_t mvs_arg_parse_get_arg(MVSArgParse *parser); 

_MVS_ATTR_INTERNAL_ void mvs_arg_parse_consume_args(MVSArgParse *parser,
                                                         msize_t count); 

_MVS_ATTR_INTERNAL_ mstr_t *mvs_arg_parse_arg_slice(MVSArgParse *parser);

_MVS_ATTR_INTERNAL_ MVSArgOption *mvs_find_option_hdlr(MVSArgParse *parser,
                                                       MVSArgOption *opts);

_MVS_ATTR_INTERNAL_ MVSArgOption *mvs_find_option_hdlr(MVSArgParse *parser,
                                                       MVSArgOption *opts) {
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

mbool_t mvs_parse_all_arg(MVSArgParse *parser, MVSArgParseResult *res) {
  // NOTE: This will get horrible
  MVSArgOption opts[_MVS_CONSTANT_PARSE_ARG_OPTION_COUNT_] = {
    {"help message", "-h", 2, mtrue, mvs_HELP_MSG},
	{"help message", "--help", 6, mtrue, mvs_HELP_MSG},
	{"version information", "-v", 2, mtrue, mvs_VERSION},
	{"version information", "--version", 9, mtrue, mvs_VERSION}, 
	{"set log level", "-log", 4, mfalse, mvs_LOG_LVL},	
	{"Provide entity spawn commands", "-spawn", 6, mfalse, mvs_SPAWN_ENTITY_COMMAND},
	{"Provide spawn list", "-slist", 6, mtrue, mvs_SLIST},
  }; 
  mvs_graves_arg_parse_set_default(res);
  while (parser->ptr < parser->argc) {
    MVSArgOption *opt = mvs_find_option_hdlr(parser, opts);
    if (!opt) {
      fprintf(stderr, "<INIT>: Unknown argument '%s'\n",
              parser->argv[parser->ptr]);
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

void mvs_arg_parse_populate_config(MVSArgParseResult *res, MVSSystemConfig *conf) {
  return;
}
/*----Deal with the parser----*/
_MVS_ATTR_INTERNAL_ mbool_t mvs_has_arg(MVSArgParse *parser) {
  return (parser->argc > parser->ptr);
}

_MVS_ATTR_INTERNAL_ msize_t mvs_args_left_to_parse(MVSArgParse *parser) {
  return (parser->argc - parser->ptr);
}

_MVS_ATTR_INTERNAL_ void mvs_arg_parse_consume_arg(MVSArgParse *parser) {
  parser->ptr++;
}

_MVS_ATTR_INTERNAL_ mstr_t mvs_arg_parse_peek(MVSArgParse *parser) {
  return (mvs_has_arg(parser)) ? parser->argv[parser->ptr + 1] : NULL;
}

_MVS_ATTR_INTERNAL_ mstr_t mvs_arg_parse_get_arg(MVSArgParse *parser) {
  return parser->argv[parser->ptr];
}

_MVS_ATTR_INTERNAL_ void mvs_arg_parse_consume_args(MVSArgParse *parser,
                                                         msize_t count) {
  parser->ptr += count;
}

_MVS_ATTR_INTERNAL_ mstr_t *mvs_arg_parse_arg_slice(MVSArgParse *parser) {
  return &parser->argv[parser->ptr];
}

/*----The functions that deal with the options----*/
_MVS_ATTR_INTERNAL_ void mvs_graves_arg_parse_set_default(MVSArgParseResult *res) {
  res->log_lvl = 2;
  res->spawn_commands = NULL;
  res->entities_to_spawn = 0;
  res->slist = NULL;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_HELP_MSG(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<HELP>:\n%s", _MVS_HELP_MSG_);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_VERSION(MVSArgParse *parser, MVSArgParseResult *res) {
  fprintf(stdout, "<VERSION>:\n%s", _MVS_VERSION_MSG_);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_LOG_LVL(MVSArgParse *parser, MVSArgParseResult *r) {
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

_MVS_ATTR_INTERNAL_ mbool_t mvs_SPAWN_ENTITY_COMMAND(MVSArgParse *parser, MVSArgParseResult *res) {
  msize_t INSTANCE_COUNT;
  msize_t EID;
  mstr_t *args;
  msize_t ARG_COUNT;
  mstr_t arg = mvs_arg_parse_get_arg(parser);
  msize_t len = strlen(arg);
  mstr_t instance_count = arg + 6;
  mstr_t tmp = NULL;
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
      if (tmp && *tmp != '\0') {
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
  if (tmp && *tmp != '\0') {
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
      if (tmp && *tmp != '\0') {
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
  if (!command) {
    fprintf(stderr, "<ArgParse>: Registering ENTITY SPAWN command "
                    "failed[Memory allocation failure]\n");
    return mfalse;
  }
  command->EID = EID;
  command->instances = INSTANCE_COUNT;
  command->argc = ARG_COUNT;
  command->argv = args;
  command->nxt_command = res->spawn_commands;
  res->spawn_commands = command;
  res->entities_to_spawn += INSTANCE_COUNT;
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_SLIST(MVSArgParse *parser, MVSArgParseResult *res) {
  if (res->slist) {
    fprintf(stderr, "<ArgParse>: SLIST file already provided %s\n", res->slist);
    return mfalse;
  }
  mvs_arg_parse_consume_arg(parser);
  if (!mvs_has_arg(parser)) {
    fprintf(stderr, "<ArgParse>: Expected a file path for SLIST file\n");
    return mfalse;
  }
  res->slist = mvs_arg_parse_get_arg(parser);
  return mtrue;
}
