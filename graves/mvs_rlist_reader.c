#include <mvs_rlist_reader.h>

MVSRlistReader *mvs_rlist_reader_create() {
  MVSRlistReader *r = (MVSRlistReader *)malloc(sizeof(MVSRlistReader));
  if (!r) {
    fprintf(stderr, "Failed to initialize RLIST reader\n");
    return NULL;
  }
  r->file = NULL;
  r->file_contents = NULL;
  return r;
}

void mvs_rlist_reader_destroy(MVSRlistReader *r) {
  if (r->file)
    mvs_file_destroy(r->file);
  if (r->file_contents)
    free(r->file_contents);
  free(r);
}

mbool_t mvs_rlist_reader_init(MVSRlistReader *r, mstr_t file_path) {
  if (mvs_file_create(&r->file) != MRES_SUCCESS) {
    fprintf(stderr, "Failed to allocate memory for file: PATH=%s\n", file_path);
    return mfalse;
  }
  if (mvs_file_open(r->file, file_path, MVS_FILE_MODE_READ_WRITE) !=
      MRES_SUCCESS) {
    fprintf(stderr,
            "Failed to open RLIST file: PATH=%s. Check if it exists and not a "
            "directory\n",
            file_path);
    return mfalse;
  }
  msize_t len, tmp;
  mvs_file_seek(r->file, 0, SEEK_END, &len);
  mvs_file_seek(r->file, 0, SEEK_SET, NULL);
  if (!len) {
    fprintf(stderr, "Empty RLIST file\n");
    return mfalse;
  }
  r->file_contents = (mstr_t)malloc(len);
  if (!r->file_contents) {
    fprintf(stderr, "Failed to allocate memory for RLIST file contents\n");
    return mfalse;
  }
  mvs_file_read(r->file, r->file_contents, len, &tmp);
  if (len != tmp) {
    fprintf(stderr, "Failed to read the contents of the RLIST file\n");
    return mfalse;
  }
  r->curr = r->file_contents;
  r->end = r->curr + (len - 1);
  r->line = 1;
  r->col = 0;
  return mtrue;
}
