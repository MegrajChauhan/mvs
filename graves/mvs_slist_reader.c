#include <mvs_slist_reader.h>

MVSSlistReader *mvs_slist_reader_create() {
  MVSSlistReader *r = (MVSSlistReader *)malloc(sizeof(MVSSlistReader));
  if (!r) {
    mvs_log_err("Failed to initialize SLIST reader");
    return NULL;
  }
  r->file = NULL;
  r->file_contents = NULL;
  return r;
}

void mvs_slist_reader_destroy(MVSSlistReader *r) {
  if (r->file)
    mvs_mapped_memory_destroy(r->file);
  r->file_contents = NULL;
  free(r);
}

mbool_t mvs_slist_reader_init(MVSSlistReader *r, mstr_t file_path) {
  mvs_log_dbg("Preparing SLIST reader for file: %s", file_path);
  if (mvs_mapped_memory_create(&r->file, 0) != MRES_SUCCESS) {
    mvs_log_err("Failed to initialize memory for SLIST file: PATH=%s",
                file_path);
    return mfalse;
  }
  if (mvs_mapped_memory_mapf(r->file, file_path, 0,
                             MVS_MMAPPED_FLAG_READ | MVS_MMAPPED_FLAG_WRITE |
                                 MVS_MMAPPED_FLAG_PRIVATE) != MRES_SUCCESS) {
    mvs_log_err("Failed to allocate memory for SLIST file: PATH=%s", file_path);
    return mfalse;
  }
  // Now, the ptr can be easily obtained
  mvs_mapped_memory_obtain_ptr(r->file, (mbptr_t *)(&r->file_contents), 0);
  msize_t len;
  mvs_mapped_memory_obtain_map_size(r->file, &len);
  r->curr = r->file_contents;
  r->end = r->curr + (len);
  r->line = 1;
  r->col = 0;
  mvs_log_dbg("Prepared SLIST reader for file=%s with length=%zu", file_path,
              len);
  return mtrue;
}
