#include <mvs_slist.h>

mbool_t mvs_slist_make_commands(MVSSlist *l, mstr_t path,
                                MVSArgParseResult *cmd, MVSSystemConfig *conf) {
  l->parser = mvs_slist_parser_create(cmd, conf);
  if (!l->parser)
    return mfalse;
  if (!mvs_slist_parser_init(l->parser, path)) {
    mvs_slist_parser_destroy(l->parser);
    return mfalse;
  }
  if (!mvs_slist_parser_build(l->parser)) {
    mvs_slist_parser_destroy(l->parser);
    return mfalse;
  }
  return mtrue;
}
