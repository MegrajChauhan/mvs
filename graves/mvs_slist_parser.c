#include <mvs_slist_parser.h>

MVSSlistParser *mvs_slist_parser_create(MVSArgParseResult *cmd) {
  MVSSlistParser *p = (MVSSlistParser *)malloc(sizeof(MVSSlistParser));
  if (!p) {
    mvs_log_err("Failed to initialize SLIST Parser");
    return NULL;
  }
  p->lexer = NULL;
  p->cmd = cmd;
  return p;
}

void mvs_slist_parser_destroy(MVSSlistParser *p) {
  if (p->lexer)
    mvs_slist_lexer_destroy(p->lexer);
  mvs_dynamic_listl_destroy(p->command_list);
  free(p);
}

mbool_t mvs_slist_parser_init(MVSSlistParser *p, mstr_t file_path) {
  mvs_log_dbg("Preparing SLIST parser for file=%s", file_path);
  if (!(p->lexer = mvs_slist_lexer_create()))
    return mfalse;
  if (mvs_slist_lexer_init(p->lexer, file_path) != mtrue)
    return mfalse;
  if (mvs_dynamic_listl_create(&p->command_list, 10,
                               sizeof(MVSSlistCommand *)) != MRES_SUCCESS) {
    mvs_log_err("While prepping to build file=%zu, failed to allocate memory "
                "for command list",
                file_path);
    mvs_slist_lexer_destroy(p->lexer);
    return mfalse;
  }
  p->file_path = file_path;
  p->metadata_read = mfalse;
  p->max_entity_count = 0;
  p->curr_id_count = (msize_t)(-1);
  mvs_log_dbg("Prepared SLIST parser for file=%s", file_path);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_check_equals(MVSSlistParser *p,
                                                          mstr_t field) {
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_EQUALS) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected '=' after '%s'.",
                p->file_path, tok.line, tok.col, field);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_check_colon(MVSSlistParser *p,
                                                         mstr_t field) {
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_COLON) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected ':' after '%s'.",
                p->file_path, tok.line, tok.col, field);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_check_block_open(MVSSlistParser *p,
                                                              mstr_t field) {
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_OPEN_CURLY) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected '{' to open block '%s'.",
                p->file_path, tok.line, tok.col, field);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_check_block_close(MVSSlistParser *p, mstr_t field) {
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected '}' to close block '%s'.",
                p->file_path, tok.line, tok.col, field);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_metadata_field_count(MVSSlistParser *p) {
  if (!mvs_slist_parser_check_equals(p, "count"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_NUM) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: 'count' only accepts integer values.",
                p->file_path, tok.line, tok.col);
    return mfalse;
  }
  char num[tok.iden.len + 1];
  memcpy(num, tok.iden.st, tok.iden.len);
  num[tok.iden.len] = 0;
  p->max_entity_count =
      strtoull(num, NULL, 10); // This won't fail(unless strtoull itself fails)
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_metadata_field(
    MVSSlistParser *p, MVSSlistToken *tok) {
  if (strncmp(tok->iden.st, "count", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_metadata_field_count(p);
  }
  char iden[tok->iden.len + 1];
  memcpy(iden, tok->iden.st, tok->iden.len);
  iden[tok->iden.len] = 0;
  mvs_log_err("In file=%s:l=%zu:c=%zu: Unknown metadata field '%s'.",
              p->file_path, tok->line, tok->col, iden);
  return mfalse;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_metadata(MVSSlistParser *p) {
  if (p->metadata_read) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Metadata already provided.",
                p->file_path, tok.line, tok.col);
    return mfalse;
  }
  mvs_log_dbg("Dealing with metadata: file=%s", p->file_path);
  if (!mvs_slist_parser_check_colon(p, "metadata"))
    return mfalse;
  if (!mvs_slist_parser_check_block_open(p, "metadata"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  while (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      mvs_log_err(
          "In file=%s:l=%zu:c=%zu: Unexpected EOF while expecting metadata.",
          p->file_path, tok.line, tok.col);
      return mfalse;
    case MVS_SLIST_TOK_IDEN: {
      if (!mvs_slist_parser_deal_with_metadata_field(p, &tok))
        return mfalse;
      tok = mvs_slist_lexer_peek_token(p->lexer);
      if (tok.type == MVS_SLIST_TOK_COMMA)
        mvs_slist_lexer_next_token(p->lexer);
      else if (tok.type == MVS_SLIST_TOK_CLOSE_CURLY)
        break;
      else {
        mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a ',' or '}'",
                    p->file_path, tok.line, tok.col);
        return mfalse;
      }
      break;
    }
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a proper metadata field.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
  if (!p->max_entity_count) {
    mvs_log_err("In file=%s: 'count'=0 in metadata", p->file_path);
    return mfalse;
  }
  p->metadata_read = mtrue;
  mvs_log_dbg("Done dealing with metadata: file=%s", p->file_path);
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_config_field_local_list(
    MVSSlistParser *p, MVSSlistCommand *c) {
  if (!mvs_slist_parser_check_equals(p, "local_list"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_IDEN) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Expected a true or false for local_list.",
        p->file_path, tok.line, tok.col);
    return mfalse;
  }
  char iden[tok.iden.len + 1];
  memcpy(iden, tok.iden.st, tok.iden.len);
  iden[tok.iden.len] = 0;
  if (strncmp(iden, "true", tok.iden.len) == 0)
    c->config |= MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE;
  else if (strncmp(iden, "false", tok.iden.len) == 0)
    c->config = c->config & ~(MVS_CONF_ENTITY_LOCAL_ENTITY_LIST_ENABLE);
  else {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a true or false for "
                "local_list but got '%s'.",
                p->file_path, tok.line, tok.col, iden);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_config_field_ll_lim(
    MVSSlistParser *p, MVSSlistCommand *c) {
  if (!mvs_slist_parser_check_equals(p, "ll_lim"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_NUM) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected an integer for ll_lim.",
                p->file_path, tok.line, tok.col);
    return mfalse;
  }
  char iden[tok.iden.len + 1];
  memcpy(iden, tok.iden.st, tok.iden.len);
  iden[tok.iden.len] = 0;
  msize_t lim = strtoull(tok.num.st, NULL, 10);
  if (!lim) {
    mvs_log_note(
        "In file=%s:l=%zu:c=%zu: ll_lim=0 is the same as not setting a limit.",
        p->file_path, tok.line, tok.col);
    c->config = c->config & ~(MVS_CONF_ENTITY_ENTITY_TRACKING_LIM);
  } else {
    c->config |= MVS_CONF_ENTITY_ENTITY_TRACKING_LIM;
    c->limits.local_list_lim = lim;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_config_field(
    MVSSlistParser *p, MVSSlistToken *t, MVSSlistCommand *c) {
  char iden[t->iden.len + 1];
  memcpy(iden, t->iden.st, t->iden.len);
  iden[t->iden.len] = 0;
  if (strncmp(iden, "local_list", t->iden.len) == 0)
    return mvs_slist_parser_deal_with_config_field_local_list(p, c);
  else if (strncmp(iden, "ll_lim", t->iden.len) == 0)
    return mvs_slist_parser_deal_with_config_field_ll_lim(p, c);
  else
    mvs_log_err("In file=%s:l=%zu:c=%zu: Unknown config field '%s'.",
                p->file_path, tok->line, tok->col, iden);
  return mfalse;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_config(MVSSlistParser *p, MVSSlistCommand *c) {
  if (!mvs_slist_parser_check_colon(p, "config"))
    return mfalse;
  if (!mvs_slist_parser_check_block_open(p, "config"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  while (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      mvs_log_err(
          "In file=%s:l=%zu:c=%zu: Unexpected EOF while expecting config.",
          p->file_path, tok.line, tok.col);
      return mfalse;
    case MVS_SLIST_TOK_IDEN: {
      if (!mvs_slist_parser_deal_with_config_field(p, &tok, c))
        return mfalse;
      tok = mvs_slist_lexer_peek_token(p->lexer);
      if (tok.type == MVS_SLIST_TOK_COMMA)
        mvs_slist_lexer_next_token(p->lexer);
      else if (tok.type == MVS_SLIST_TOK_CLOSE_CURLY)
        break;
      else {
        mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a ',' or '}'",
                    p->file_path, tok.line, tok.col);
        return mfalse;
      }
      break;
    }
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a proper config field.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_property_field(
    MVSSlistParser *p, MVSSlistToken *t, MVSSlistCommand *c) {
  char iden[t->iden.len + 1];
  memcpy(iden, t->iden.st, t->iden.len);
  iden[t->iden.len] = 0;
  mvs_log_err("In file=%s:l=%zu:c=%zu: Unknown property field '%s'.",
              p->file_path, tok->line, tok->col, iden);
  return mfalse;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_properties(MVSSlistParser *p, MVSSlistCommand *c) {
  if (!mvs_slist_parser_check_colon(p, "property"))
    return mfalse;
  if (!mvs_slist_parser_check_block_open(p, "property"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  while (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      mvs_log_err(
          "In file=%s:l=%zu:c=%zu: Unexpected EOF while expecting property.",
          p->file_path, tok.line, tok.col);
      return mfalse;
    case MVS_SLIST_TOK_IDEN: {
      if (!mvs_slist_parser_deal_with_property_field(p, &tok, c))
        return mfalse;
      tok = mvs_slist_lexer_peek_token(p->lexer);
      if (tok.type == MVS_SLIST_TOK_COMMA)
        mvs_slist_lexer_next_token(p->lexer);
      else if (tok.type == MVS_SLIST_TOK_CLOSE_CURLY)
        break;
      else {
        mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a ',' or '}'",
                    p->file_path, tok.line, tok.col);
        return mfalse;
      }
      break;
    }
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a proper property field.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
  return mtrue; // we have no properties yet
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_setup_field(MVSSlistParser *p, 
				MVSSlistToken *t, MVSSlistCommand *c) {
    if (!mvs_slist_parser_check_equals(p, "setup"))
			return mfalse;
    char field[t->iden.len + 1];
    memcpy(field, t->iden.st, t->iden.len);
    field[t->iden.len] = 0;	
	MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
	if (tok.type != MVS_SLIST_TOK_IDEN || tok.type != MVS_SLIST_TOK_NUM) {
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected an integer or some identifier for setup.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
	}
	char value[tok.iden.len + 1];
    memcpy(value, tok.iden.st, tok.iden.len);
    value[tok.iden.len] = 0;	
    EntityRegistryEntry *entry = mvs_registry_get_entry(EID);
	c->setup |= entry->deduce_setup(field, value);
	return mtrue;
}

_MVS_ATTR_INTERNAL_ 
mbool_t mvs_slist_parser_deal_with_setup(MVSSlistParser *p, MVSSlistCommand *c) {
  if (!mvs_slist_parser_check_colon(p, "setup"))
    return mfalse;
  if (!mvs_slist_parser_check_block_open(p, "setup"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  while (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      mvs_log_err(
          "In file=%s:l=%zu:c=%zu: Unexpected EOF while expecting setup.",
          p->file_path, tok.line, tok.col);
      return mfalse;
    case MVS_SLIST_TOK_IDEN: {
      if (!mvs_slist_parser_deal_with_setup_field(p, &tok, c))
        return mfalse;
      tok = mvs_slist_lexer_peek_token(p->lexer);
      if (tok.type == MVS_SLIST_TOK_COMMA)
        mvs_slist_lexer_next_token(p->lexer);
      else if (tok.type == MVS_SLIST_TOK_CLOSE_CURLY)
        break;
      else {
        mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a ',' or '}'",
                    p->file_path, tok.line, tok.col);
        return mfalse;
      }
      break;
    }
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a proper setup field.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_args(MVSSlistParser *p, MVSSlistCommand *c) {
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_add_to_ll(MVSSlistParser *p, MVSSlistCommand *c) {
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_select_path(MVSSlistParser *p,
                                                         MVSSlistToken *tok) {
  if (!p->metadata_read) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Expected metadata at the start of the file.",
        p->file_path, tok->line, tok->col) return mfalse;
  }
  if (strncmp(tok->iden.st, "metadata", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_metadata(p);
  }
  char iden[tok->iden.len + 1];
  memcpy(iden, tok->iden.st, tok->iden.len);
  iden[tok->iden.len] = 0;
  mvs_log_err("In file=%s:l=%zu:c=%zu: Unknown identifier '%s'.", p->file_path,
              tok->line, tok->col, iden);
  return mfalse;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_build_entity(MVSSlistParser *p,
                                                          MVSSlistToken *tok,
                                                          MVSSlistCommand *c) {
  if (strncmp(tok->iden.st, "config", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_config(p, c);
  } else if (strncmp(tok->iden.st, "property", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_properties(p, c);
  } else if (strncmp(tok->iden.st, "setup", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_setup(p, c);
  } else if (strncmp(tok->iden.st, "args", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_config(p, c);
  } else if (strncmp(tok->iden.st, "add_to_ll", tok->iden.len) == 0) {
    return mvs_slist_parser_deal_with_add_to_ll(p, c);
  }
  char iden[tok->iden.len + 1];
  memcpy(iden, tok->iden.st, tok->iden.len);
  iden[tok->iden.len] = 0;
  mvs_log_err("In file=%s:l=%zu:c=%zu: Unknown field '%s'.", p->file_path,
              tok->line, tok->col, iden);
  return mfalse;
}

_MVS_ATTR_INTERNAL_ mbool_t
mvs_slist_parser_deal_with_id_copy(MVSSlistParser *p, msize_t ID) {
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_NUM) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected an ID to copy entity ID=%zu.",
                p->file_path, tok.line, tok.col, ID);
    return mfalse;
  }
  msize_t OID = strtoull(t->num.st, NULL, 10);
  if (OID > p->curr_id_count) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected an ID to copy entity ID=%zu.",
                p->file_path, tok.line, tok.col, ID);
    return mfalse;
  }
  tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type != MVS_SLIST_TOK_CLOSE_BRAC) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected ']' to end the block for "
                "entity ID=%zu.",
                p->file_path, tok.line, tok.col, ID);
    return mfalse;
  }
  MVSSlistCommand *c = mvs_dynamic_listl_ref_of_unsafe(p->command_list, OID);
  // c will be valid
  MVSSlistCommand *n = (MVSSlistCommand *)malloc(sizeof(MVSSlistCommand));
  if (!n) {
    mvs_log_err("Memory allocation failed: Building entity=%zu, file=%zu", ID,
                p->file_path);
    return mfalse;
  }
  memcpy(n, c, sizeof(MVSSlistCommand));
  n->copy = mtrue;
  if (mvs_dynamic_listl_push(p->command_list, &n) != MRES_SUCCESS) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Failed to add command for entity ID=%zu.",
        p->file_path, tok.line, tok.col, ID);
    free(n);
    return mfalse;
  }
  return mtrue;
}

_MVS_ATTR_INTERNAL_ mbool_t mvs_slist_parser_deal_with_id(MVSSlistParser *p,
                                                          MVSSlistToken *t) {
  if (!p->metadata_read) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Expected metadata at the start of the file.",
        p->file_path, tok->line, tok->col);
    return mfalse;
  }
  msize_t ID = strtoull(t->num.st, NULL, 10);
  if (ID >= p->max_entity_count) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Provided ID=%zu exceeds the limit set=%zu",
        p->file_path, t->line, t->col, ID, p->max_entity_count);
    return mfalse;
  }
  if (ID != (curr_id_count + 1)) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Invalid ID provided %zu", p->file_path,
                t->line, t->col, ID);
    return mfalse;
  }
  if (!mvs_slist_parser_check_colon(p, "ID"))
    return mfalse;
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  if (tok.type == MVS_SLIST_TOK_OPEN_BRAC) {
    return mvs_slist_parser_deal_with_id_copy(p, ID);
  } else if (tok.type != MVS_SLIST_TOK_NUM) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Expected EID after ID.", p->file_path,
                tok.line, tok.col);
    return mfalse;
  }
  msize_t EID = strtoull(tok.num.st, NULL, 10);
  if (EID >= _MVS_CONSTANT_ENTITY_COUNT_) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Invalid EID '%zu' provided.",
                p->file_path, tok.line, tok.col, EID);
    return mfalse;
  }
  EntityRegistryEntry *entry = mvs_registry_get_entry(EID);
  if (!entry) {
    mvs_log_err("In file=%s:l=%zu:c=%zu: EID='%zu' is not registered.",
                p->file_path, tok.line, tok.col, EID) return mfalse;
  }
  p->curr_id_count++;
  if (!mvs_slist_parser_check_block_open(p, "ENTITY"))
    return mfalse;
  MVSSlistCommand *c = (MVSSlistCommand *)malloc(sizeof(MVSSlistCommand));
  if (!c) {
    mvs_log_err("Memory allocation failed: Building entity=%zu, file=%zu", ID,
                p->file_path);
    return mfalse;
  }
  c->copy = mfalse;
  c->setup = 0;
  c->config = 0;
  c->properties = 0;
  tok = mvs_slist_lexer_next_token(p->lexer);
  while (tok.type != MVS_SLIST_TOK_CLOSE_CURLY) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Unexpected EOF while expecting "
                  "entity fields.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    case MVS_SLIST_TOK_IDEN: {
      if (!mvs_slist_parser_build_entity(p, &tok, c))
        return mfalse;
      tok = mvs_slist_lexer_peek_token(p->lexer);
      if (tok.type == MVS_SLIST_TOK_COMMA)
        mvs_slist_lexer_next_token(p->lexer);
      else if (tok.type == MVS_SLIST_TOK_CLOSE_CURLY)
        break;
      else {
        mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a ',' or '}'",
                    p->file_path, tok.line, tok.col);
        return mfalse;
      }
      break;
    }
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Expected a proper entity field.",
                  p->file_path, tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
  // add the command to the list
  if (mvs_dynamic_listl_push(p->command_list, &c) != MRES_SUCCESS) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Failed to add command for entity ID=%zu.",
        p->file_path, tok.line, tok.col, ID);
    free(c);
    return mfalse;
  }
  return mtrue;
}

mbool_t mvs_slist_parser_build(MVSSlistParser *p) {
  mvs_log_dbg("Building from file=%s", p->file_path);
  MVSSlistToken tok = mvs_slist_lexer_next_token(p->lexer);
  while (mtrue) {
    switch (tok.type) {
    case MVS_SLIST_TOK_EOF:
      goto __mvs_slist_parser_build_exit;
    case MVS_SLIST_TOK_ERR:
      return mfalse;
    case MVS_SLIST_TOK_IDEN:
      if (!mvs_slist_parser_select_path(p, &tok))
        return mfalse;
      break;
    case MVS_SLIST_TOK_NUM:
      if (!mvs_slist_parser_deal_with_id(p, &tok))
        return mfalse;
      break;
    default:
      mvs_log_err("In file=%s:l=%zu:c=%zu: Unexpected token.", p->file_path,
                  tok.line, tok.col);
      return mfalse;
    }
    tok = mvs_slist_lexer_next_token(p->lexer);
  }
__mvs_slist_parser_build_exit:
  mvs_log_dbg("Finished building from file=%s", p->file_path);
  return mtrue;
}
