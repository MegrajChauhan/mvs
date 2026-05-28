#include <mvs_slist_lexer.h>

MVSSlistLexer *mvs_slist_lexer_create() {
  MVSSlistLexer *l = (MVSSlistParser *)malloc(sizeof(MVSSlistParser));
  if (!l) {
    mvs_log_err("Failed to initialize SLIST Lexer");
    return NULL;
  }
  l->reader = NULL;
  return l;
}

void mvs_slist_lexer_destroy(MVSSlistLexer *l) {
  if (l->reader)
    mvs_slist_reader_destroy(l->reader);
  free(l);
}

mbool_t mvs_slist_lexer_init(MVSSlistLexer *l, mstr_t file_path) {
  mvs_log_dbg("Preparing SLIST lexer for file=%s", file_path);
  if (!(l->reader = mvs_slist_reader_create()))
    return mfalse;
  if (mvs_slist_reader_init(l->reader, file_path) != mtrue)
    return mfalse;
  l->file_path = file_path;
  mvs_log_dbg("Prepared SLIST lexer for file=%s", file_path);
  return mtrue;
}

MVSSlistToken mvs_slist_lexer_next_token(MVSSlistLexer *l) {
  MVSSlistToken tok;
  char curr = mvs_slist_reader_curr(l->reader);
  while ((!_MVS_MFUNC_ISALNUM_(curr) || curr != '_') && curr != '\0') {
    mvs_slist_reader_consume(l->reader);
    curr = mvs_slist_reader_curr(l->reader);
  }
  if (curr == '\0') {
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_EOF};
    return tok;
  }
  if (curr == ';') {
    while (curr != ';' && curr != '\0') {
      mvs_slist_reader_consume(l->reader);
      curr = mvs_slist_reader_curr(l->reader);
    }
  }
  if (curr == '\0') {
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_EOF};
    return tok;
  }
  msize_t len = 0;
  msize_t line = mvs_slist_reader_line(l->reader);
  if (_MVS_MFUNC_ISNUM_(curr)) {
    mstr_t st = mvs_slist_reader_iter(l->reader);
    msize_t col = mvs_slist_reader_col(l->reader);
    while (_MVS_MFUNC_ISNUM_(curr)) {
      mvs_slist_reader_consume(l->reader);
      curr = mvs_slist_reader_curr(l->reader);
      len++;
    }
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_NUM,
                          .line = line,
                          .col = col,
                          .num = (MVSStrSlice){.ptr = st, .len = len}};
  } else if (_MVS_MFUNC_ISALPHA_(curr) || curr == '_') {
    mstr_t st = mvs_slist_reader_iter(l->reader);
    while (_MVS_MFUNC_ISALPHA_(curr) || curr == '_' || _MVS_MFUNC_ISNUM_(curr)) {
      mvs_slist_reader_consume(l->reader);
      curr = mvs_slist_reader_curr(l->reader);
    }
    mstr_t ed = mvs_slist_reader_iter(l->reader);
    tok =
        (MVSSlistToken){.type = MVS_SLIST_TOK_IDEN,
                        .line = line,
                        .col = col,
                        .iden = (MVSStrSlice){.ptr = st, .len = (ed - st + 1)}};
  } else if (curr == '{') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){
        .type = MVS_SLIST_TOK_OPEN_CURLY, .line = line, .col = col};
  } else if (curr == '}') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){
        .type = MVS_SLIST_TOK_CLOSE_CURLY, .line = line, .col = col};
  } else if (curr == ':') {
    mvs_slist_reader_consume(l->reader);
    tok =
        (MVSSlistToken){.type = MVS_SLIST_TOK_COLON, .line = line, .col = col};
  } else if (curr == '=') {
    mvs_slist_reader_consume(l->reader);
    tok =
        (MVSSlistToken){.type = MVS_SLIST_TOK_EQUALS, .line = line, .col = col};
  } else if (curr == ',') {
    mvs_slist_reader_consume(l->reader);
    tok =
        (MVSSlistToken){.type = MVS_SLIST_TOK_COMMA, .line = line, .col = col};
  } else if (curr == '[') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){
        .type = MVS_SLIST_TOK_OPEN_BRAC, .line = line, .col = col};
  } else if (curr == ']') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){
        .type = MVS_SLIST_TOK_CLOSE_BRAC, .line = line, .col = col};
  } else {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Couldn't build a token out of this '%c'.",
        l->file_path, mvs_slist_reader_line(l->reader),
        mvs_slist_reader_col(l->reader), curr);
    return (MVSSlistToken){.type = MVS_SLIST_TOK_ERR};
  }
  return tok;
}

MVSSlistToken mvs_slist_lexer_peek_token(MVSSlistLexer *l) {
  MVSSlistToken tok;
  mvs_slist_reader_make_backup(l->reader);
  tok = mvs_slist_lexer_next_token(l);
  mvs_slist_reader_restore(l->reader);
  return tok;
}

MVSSlistToken mvs_slist_lexer_get_slice(MVSSlistLexer *l) {
  char curr = mvs_slist_reader_curr(l->reader);
  while ((!_MVS_MFUNC_ISALNUM_(curr) || curr != '_') && curr != '\0') {
    mvs_slist_reader_consume(l->reader);
    curr = mvs_slist_reader_curr(l->reader);
  }
  mstr_t st = mvs_slist_reader_iter(l->reader);
  msize_t line = mvs_slist_reader_line(l->reader);
  msize_t col = mvs_slist_reader_col(l->reader);
  msize_t len = 0;
  while (curr != '\\' && curr != '\0') {
    mvs_slist_reader_consume(l->reader);
    curr = mvs_slist_reader_curr(l->reader);
    len++;
  }
  if (len == 0) {
    if (curr == '\0')
      return (MVSSlistToken){.type = MVS_SLIST_TOK_EOF};
    else
      return (MVSSlistToken){.type = MVS_SLIST_TOK_ERR}; // what???
  }
  if (curr != '\\') {
    mvs_log_err("In file=%s:l=%zu:c=%zu: Reached EOF before '\\'", l->file_path,
                mvs_slist_reader_line(l->reader),
                mvs_slist_reader_col(l->reader));
    return (MVSSlistToken){.type = MVS_SLIST_TOK_ERR};
  }
  mvs_slist_reader_consume(l->reader);
  return (MVSSlistToken) {
    .type = MVS_SLIST_TOK_IDEN, .line = line, .col = col,
    .iden = (MVSStrSlice){.ptr = st, .len = len};
  }
