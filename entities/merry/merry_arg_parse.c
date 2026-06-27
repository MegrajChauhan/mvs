#include <merry_arg_parse.h>

_MVS_ATTR_INTERNAL_
mbool_t merry_HELP_MSG(MerryArgParser *parser, MerryArgParseResult *res);

_MVS_ATTR_INTERNAL_
mbool_t merry_VERSION(MerryArgParser *parser, MerryArgParseResult *res);

_MVS_ATTR_INTERNAL_
mbool_t merry_INP_FILE(MerryArgParser *parser, MerryArgParseResult *res);

_MVS_ATTR_INTERNAL_ void merry_arg_parse_set_default(MerryArgParseResult *res);

_MVS_ATTR_INTERNAL_ mbool_t merry_has_arg(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ msize_t merry_args_left_to_parse(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ void merry_arg_parse_consume_arg(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ mstr_t merry_arg_parse_peek(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ mstr_t merry_arg_parse_get_arg(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ void merry_arg_parse_consume_args(MerryArgParser *parser,
                                                      msize_t count);

_MVS_ATTR_INTERNAL_ mstr_t *merry_arg_parse_arg_slice(MerryArgParser *parser);

_MVS_ATTR_INTERNAL_ MerryArgOption *
merry_find_option_hdlr(MerryArgParser *parser, MerryArgOption *opts);

_MVS_ATTR_INTERNAL_ MerryArgOption *
merry_find_option_hdlr(MerryArgParser *parser, MerryArgOption *opts) {
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

mbool_t merry_parse_all_arg(MerryArgParser *parser, MerryArgParseResult *res) {
  // NOTE: This will get horrible
  MerryArgOption opts[_MERRY_CMD_OPTION_COUNT_] = {
      {"help message", "-h", 2, mtrue, merry_HELP_MSG},
      {"version information", "-v", 2, mtrue, merry_VERSION},
      {"Provide input file", "-f", 2, mtrue, merry_INP_FILE},
  };
  merry_arg_parse_set_default(res);
  while (parser->ptr < parser->argc) {
    MerryArgOption *opt = merry_find_option_hdlr(parser, opts);
    if (!opt) {
      MERRY_ERR("[ArgParse]: Unknown argument '%s'\n",
                parser->argv[parser->ptr]);
      merry_HELP_MSG(parser, res);
      return mfalse;
    }
    if (!opt->hdlr(parser, res)) {
      merry_HELP_MSG(parser, res);
      return mfalse;
    }
    parser->ptr++;
  }
  return mtrue;
}

void merry_arg_parse_populate_config_from_result(MerryArgParseResult *res,
                                                 MerryConfig *conf) {
  return;
}
/*----Deal with the parser----*/
_MVS_ATTR_INTERNAL_ mbool_t merry_has_arg(MerryArgParser *parser) {
  return (parser->argc > parser->ptr);
}

_MVS_ATTR_INTERNAL_ msize_t merry_args_left_to_parse(MerryArgParser *parser) {
  return (parser->argc - parser->ptr);
}

_MVS_ATTR_INTERNAL_ void merry_arg_parse_consume_arg(MerryArgParser *parser) {
  parser->ptr++;
}

_MVS_ATTR_INTERNAL_ mstr_t merry_arg_parse_peek(MerryArgParser *parser) {
  return (merry_has_arg(parser)) ? parser->argv[parser->ptr + 1] : NULL;
}

_MVS_ATTR_INTERNAL_ mstr_t merry_arg_parse_get_arg(MerryArgParser *parser) {
  return parser->argv[parser->ptr];
}

_MVS_ATTR_INTERNAL_ void merry_arg_parse_consume_args(MerryArgParser *parser,
                                                      msize_t count) {
  parser->ptr += count;
}

_MVS_ATTR_INTERNAL_ mstr_t *merry_arg_parse_arg_slice(MerryArgParser *parser) {
  return &parser->argv[parser->ptr];
}

/*----The functions that deal with the options----*/
_MVS_ATTR_INTERNAL_ void merry_arg_parse_set_default(MerryArgParseResult *res) {
  res->input_file = NULL;
}

_MVS_ATTR_INTERNAL_ mbool_t merry_HELP_MSG(MerryArgParser *parser,
                                           MerryArgParseResult *res) {
  MERRY_NOTE("<HELP>:\n%s", _MERRY_HELP_MSG_);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t merry_VERSION(MerryArgParser *parser,
                                          MerryArgParseResult *res) {
  MERRY_NOTE("<VERSION>:\n%s", _MERRY_VERSION_MSG_);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t merry_INP_FILE(MerryArgParser *parser,
                                           MerryArgParseResult *res) {
  if (res->input_file) {
    MERRY_ERR("<ArgParse>: Input file already provided %s\n", res->input_file);
    return mfalse;
  }
  merry_arg_parse_consume_arg(parser);
  if (!merry_has_arg(parser)) {
    MERRY_ERR("<ArgParse>: Expected a file path for input file\n");
    return mfalse;
  }
  res->input_file = merry_arg_parse_get_arg(parser);
  return mtrue;
}
