#ifndef _MVS_RLIST_READER_
#define _MVS_RLIST_READER_

#include <mvs_file.h>
#include <mvs_types.h>
#include <mvs_tools.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct MVSRlistReader MVSRlistReader;

struct MVSRlistReader {
  MVSFile *file; // for quicker IO
  mstr_t file_contents;
  mstr_t curr;
  mstr_t end;
  msize_t line;
  msize_t col;
};

MVSRlistReader *mvs_rlist_reader_create();

void mvs_rlist_reader_destroy(MVSRlistReader *r);

mbool_t mvs_rlist_reader_init(MVSRlistReader *r, mstr_t file_path);

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_rlist_reader_line(MVSRlistReader *r) {
  return r->line;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_rlist_reader_col(MVSRlistReader *r) {
  return r->col;
}

_MVS_ATTR_ALWAYS_INLINE_ mstr_t mvs_rlist_reader_iter(MVSRlistReader *r) {
  return r->curr;
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_rlist_reader_consume(MVSRlistReader *r) {
  if (r->curr == r->end)
    return;
  if (*(r->curr) == '\n') {
    r->line++;
    r->col = 0;
  }
  r->curr++;
}

_MVS_ATTR_ALWAYS_INLINE_ char mvs_rlist_reader_peek(MVSRlistReader *r) {
  return (r->curr == r->end) ? '\0' : *(r->curr + 1);
}

_MVS_ATTR_ALWAYS_INLINE_ char mvs_rlist_reader_curr(MVSRlistReader *r) {
  return *(r->curr);
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t mvs_rlist_reader_eof(MVSRlistReader *r) {
  return (r->curr == r->end) ? mtrue : mfalse;
}

#endif
