#ifndef _MVS_SLIST_READER_
#define _MVS_SLIST_READER_

#include <mvs_logger.h>
#include <mvs_mapped_memory.h>
#include <mvs_types.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSSlistReader MVSSlistReader;

struct MVSSlistReader {
  MVSMappedMemory *file; // for quicker IO
  mstr_t file_contents;
  mstr_t curr;
  mstr_t end;
  msize_t line;
  msize_t col;

  msize_t backup_line, backup_col;
  mstr_t backup_curr;
};

MVSSlistReader *mvs_slist_reader_create();

void mvs_slist_reader_destroy(MVSSlistReader *r);

mbool_t mvs_slist_reader_init(MVSSlistReader *r, mstr_t file_path);

_MVS_ATTR_ALWAYS_INLINE_ void mvs_slist_reader_make_backup(MVSSlistReader *r) {
  r->backup_col = r->col;
  r->backup_curr = r->curr;
  r->backup_line = r->line;
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_slist_reader_restore(MVSSlistReader *r) {
  r->col = r->backup_col;
  r->line = r->line;
  r->curr = r->backup_curr;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_slist_reader_line(MVSSlistReader *r) {
  return r->line;
}

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_slist_reader_col(MVSSlistReader *r) {
  return r->col;
}

_MVS_ATTR_ALWAYS_INLINE_ mstr_t mvs_slist_reader_iter(MVSSlistReader *r) {
  return r->curr;
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_slist_reader_consume(MVSSlistReader *r) {
  if (r->curr == r->end)
    return;
  if (*(r->curr) == '\n') {
    r->line++;
    r->col = 0;
  }
  r->curr++;
}

_MVS_ATTR_ALWAYS_INLINE_ char mvs_slist_reader_peek(MVSSlistReader *r) {
  return (r->curr == r->end) ? '\0' : *(r->curr + 1);
}

_MVS_ATTR_ALWAYS_INLINE_ char mvs_slist_reader_curr(MVSSlistReader *r) {
  return *(r->curr);
}

_MVS_ATTR_ALWAYS_INLINE_ mbool_t mvs_slist_reader_eof(MVSSlistReader *r) {
  return (r->curr == r->end) ? mtrue : mfalse;
}

#endif
