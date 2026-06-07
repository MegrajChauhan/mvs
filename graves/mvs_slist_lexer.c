#include <mvs_slist_lexer.h>

MVSSlistLexer *mvs_slist_lexer_create() {
  MVSSlistLexer *l = (MVSSlistLexer *)malloc(sizeof(MVSSlistLexer));
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
                          .num = (MVSStrSlice){.st = st, .len = len}};
  } else if (_MVS_MFUNC_ISALPHA_(curr) || curr == '_') {
    mstr_t st = mvs_slist_reader_iter(l->reader);
    msize_t col = mvs_slist_reader_col(l->reader);
    while (_MVS_MFUNC_ISALPHA_(curr) || curr == '_' ||
           _MVS_MFUNC_ISNUM_(curr)) {
      mvs_slist_reader_consume(l->reader);
      curr = mvs_slist_reader_curr(l->reader);
    }
    mstr_t ed = mvs_slist_reader_iter(l->reader);
    tok =
        (MVSSlistToken){.type = MVS_SLIST_TOK_IDEN,
                        .line = line,
                        .col = col,
                        .iden = (MVSStrSlice){.st = st, .len = (ed - st + 1)}};
  } else if (curr == '{') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_OPEN_CURLY,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == '}') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_CLOSE_CURLY,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == ':') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_COLON,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == '=') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_EQUALS,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == ',') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_COMMA,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == '[') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_OPEN_BRAC,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == ']') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_CLOSE_BRAC,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
  } else if (curr == '\\') {
    mvs_slist_reader_consume(l->reader);
    tok = (MVSSlistToken){.type = MVS_SLIST_TOK_SEPARATOR,
                          .line = line,
                          .col = mvs_slist_reader_col(l->reader)};
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

mstr_t mvs_slist_lexer_get_block(MVSSlistLexer *l, char block_borders) {
  char curr = mvs_slist_reader_curr(l->reader);
  MVSSlistReaderState s1, s2;
  mvs_slist_reader_make_backup(l->reader);
  s1 = mvs_slist_reader_get_backup(l->reader);
  msize_t len = 0;
  while (curr != block_borders) {
    if (curr == '\0') {
      mvs_log_err("In file=%s:l=%zu:c=%zu: Reached EOF before reaching the end "
                  "of block",
                  l->file_path, mvs_slist_reader_line(l->reader),
                  mvs_slist_reader_col(l->reader));
      return NULL;
    }
    mvs_slist_reader_consume(l->reader);
    curr = mvs_slist_reader_curr(l->reader);
    len++;
  }
  mvs_slist_reader_make_backup(l->reader);
  s2 = mvs_slist_reader_get_backup(l->reader);
  mvs_slist_reader_restore_from(l->reader, s1);
  mstr_t block = (mstr_t)malloc(len + 1);
  if (!block) {
    mvs_log_err(
        "In file=%s:l=%zu:c=%zu: Failed to allocate memory for the block",
        l->file_path, mvs_slist_reader_line(l->reader),
        mvs_slist_reader_col(l->reader));
    return NULL;
  }
  memcpy(block, mvs_slist_reader_iter(l->reader), len);
  block[len] = 0;
  mvs_slist_reader_consume(l->reader);
  mvs_slist_reader_restore_from(l->reader, s2);
  return block;
}
