#include <mvs_slist_lexer.h>

MVSSlistLexer *mvs_slist_lexer_create() {
		MVSSlistLexer *l = (MVSSlistParser*)malloc(sizeof(MVSSlistParser));
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
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_EOF};
				return tok;
		}
        if (curr == ';') {
           while (curr != ';' && curr != '\0') {
				mvs_slist_reader_consume(l->reader);
				curr = mvs_slist_reader_curr(l->reader);
		   }
		}
		if (curr == '\0') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_EOF};
				return tok;
		}
		msize_t len = 1;
		msize_t line = mvs_slist_reader_line(l->reader);
		if (_MVS_MFUNC_ISNUM_(curr)) {
				if (!(curr == '0' || curr == '1')) {
						mvs_log_err("In file=%s:l=%zu:c=%zu: Only binary numbers are allowed.", l->file_path, mvs_slist_reader_line(l->reader), mvs_slist_reader_col(l->reader));
						return (MVSSlistToken){.type=MVS_SLIST_TOK_ERR};
				}
				mstr_t st = mvs_slist_reader_iter(l->reader);		
				msize_t col = mvs_slist_reader_col(l->reader);
				while (curr == '0' || curr == '1') {
						mvs_slist_reader_consume(l->reader);
						curr = mvs_slist_reader_curr(l->reader);
						len++;
				}
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_NUM, .line=line, .col=col, .num=(MVSStrSlice){.ptr=st, .len=len}};
		} else if (_MVS_MFUNC_ISALPHA_(curr) || curr == '_') {
				// This could either be an identifier or a command
				mstr_t st = mvs_slist_reader_iter(l->reader);
				while (_MVS_MFUNC_ISALPHA_(curr) || curr == '_') {
						mvs_slist_reader_consume(l->reader);
						curr = mvs_slist_reader_curr(l->reader);
				}
				mstr_t ed = mvs_slist_reader_iter(l->reader);
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_IDEN, .line=line, .col=col, .iden=(MVSStrSlice){.ptr=st, .len=(ed - st + 1)}};
		} else if (curr == '(') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_OPEN_PAREN, .line=line, .col=col};
        } else if (curr == ')') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_CLOSE_PAREN, .line=line, .col=col};
		} else if (curr == '.') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_DOT, .line=line, .col=col};
		} else if (curr == '|') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_OR, .line=line, .col=col};
		} else if (curr == ',') {
				tok = (MVSSlistToken){.type=MVS_SLIST_TOK_COMMA, .line=line, .col=col};
		} else {
				mvs_log_err("In file=%s:l=%zu:c=%zu: Couldn't build a token out of this '%c'.", l->file_path, mvs_slist_reader_line(l->reader), mvs_slist_reader_col(l->reader), curr);
				return (MVSSlistToken){.type=MVS_SLIST_TOK_ERR};
		}
		return tok;
}
