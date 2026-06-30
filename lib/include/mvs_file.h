#ifndef _MVS_FILE_
#define _MVS_FILE_

#include <mvs_file_defs.h>
#include <mvs_results.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSFile MVSFile;

struct MVSFile {
  mbool_t in_use;
  mfd_t fd;
  struct {
    msize_t read : 1;
    msize_t write : 1;
    msize_t append : 1;
    msize_t new : 1; // Was the file created?
    // msize_t trunc: 1;  // not yet implemented
    msize_t resb : 59;
  } flags;
};

/*
 * In MVS, these functions are called by individual threads themselves and since
 * errno is thread local, the return of MRES_SYS_FAILURE is the trigger to check
 * the value of that errno.
 */

mResult_t mvs_file_create(MVSFile **file);

/*
 * "modes" is for the file interface.
 * "file" may be a valid unused instance as well! just reusing it! Just make
 * sure the previous file was closed and cleanup was performed before reuse
 */
mResult_t mvs_file_open(MVSFile *file, mstr_t file_path, mqword_t modes);

mResult_t mvs_file_close(MVSFile *file);

mResult_t mvs_file_destroy(MVSFile *file);

mResult_t mvs_file_seek(MVSFile *file, msqword_t off, msize_t whence,
                        msize_t *_len);

mResult_t mvs_file_tell(MVSFile *file, msize_t *off);

mResult_t mvs_file_read(MVSFile *file, mbptr_t buf, msize_t num_of_bytes,
                        msize_t *bytes_read);

mResult_t mvs_file_write(MVSFile *file, mbptr_t buf, msize_t num_of_bytes,
                         msize_t *bytes_written);

#endif
