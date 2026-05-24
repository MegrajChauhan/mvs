#include <mvs_slist_parser.h>

MVSSlistParser *mvs_slist_parser_create() {
		MVSSlistParser *p = (MVSSlistParser*)malloc(sizeof(MVSSlistParser));
		if (!p) {
				mvs_log_err("Failed to initialize SLIST Parser");
				return NULL;
		}
		p->lexer = NULL;
		return p;
}

void mvs_slist_parser_destroy(MVSSlistParser *p) {
		if (p->reader)
				mvs_slist_lexer_destroy(p->lexer);
		free(p);
}

mbool_t mvs_slist_parser_init(MVSSlistParser *p, mstr_t file_path) {
		mvs_log_dbg("Preparing SLIST parser for file=%s", file_path);
		if (!(p->lexer = mvs_slist_lexer_create()))
				return mfalse;
		if (mvs_slist_lexer_init(p->lexer, file_path) != mtrue)
				return mfalse;
		p->file_path = file_path;
		mvs_log_dbg("Prepared SLIST parser for file=%s", file_path);
		return mtrue;
}
