#ifndef _MVS_SLIST_
#define _MVS_SLIST_

#include <mvs_slist_parser.h>
#include <mvs_system_config.h>

typedef struct MVSSlist MVSSlist;

struct MVSSlist {
  MVSSlistParser *parser;
};

mbool_t mvs_slist_make_commands(MVSSlist *l, mstr_t path,
                                MVSArgParseResult *cmd, MVSSystemConfig *conf);

_MVS_ATTR_ALWAYS_INLINE_ void mvs_slist_destroy(MVSSlist *l) {
  mvs_slist_parser_destroy(l->parser);
}

_MVS_ATTR_ALWAYS_INLINE_ MVSDynamicListLinear *
mvs_slist_get_commands(MVSSlist *l) {
  return mvs_slist_parser_get_commands(l->parser);
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_slist_get_command_count(MVSSlist *l) {
  return mvs_slist_parser_get_command_count(l->parser);
}

#endif
