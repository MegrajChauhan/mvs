#include <mvs_file.h>

mResult_t mvs_file_create(MVSFile **file) {
  if (!file)
    return MRES_INVALID_ARGS;
  *file = NULL;
  MVSFile *f = (MVSFile*)malloc(sizeof(MVSFile));
  if (!f)
    return MRES_SYS_FAILURE;
  f->in_use = mfalse;
  *file = f;
  return MRES_SUCCESS;
}

mResult_t mvs_file_open(MVSFile *file, mstr_t file_path, mqword_t modes) {
  if (!file || !file_path)
    return MRES_INVALID_ARGS;
  if (file->in_use)
    return MRES_RESOURCE_ALREADY_IN_USE;
#ifdef _USE_LINUX_
  // the following isn't the best way to deal with flags
  // But lets deal with that in the future
  int open_modes = 0;
  if (modes & MVS_FILE_MODE_APPEND)
    open_modes = O_APPEND;
  if (modes & MVS_FILE_MODE_READ)
    open_modes |= O_RDONLY;
  if ((modes & MVS_FILE_MODE_WRITE)) {
    if (modes & MVS_FILE_MODE_APPEND) {
      return MRES_INVALID_ARGS;
    }
    open_modes |= O_WRONLY;
  }
  if ((modes & MVS_FILE_MODE_READ_WRITE)) {
    if (((modes & MVS_FILE_MODE_WRITE) || (modes & MVS_FILE_MODE_READ) ||
         (modes & MVS_FILE_MODE_APPEND))) {
      return MRES_INVALID_ARGS;
    }
    open_modes |= O_RDWR;
  }
  if (modes & MVS_FILE_MODE_CREATE)
    open_modes |= O_CREAT;
  file->fd = open(file_path, open_modes, MVS_FILE_MODE_PERMISSIONS);
  if (file->fd == -1) {
    return MRES_SYS_FAILURE;
  }
#else
// not yet
#endif
  file->flags.append = (modes & MVS_FILE_MODE_APPEND) ? 1 : 0;
  file->flags.write = (modes & MVS_FILE_MODE_WRITE)        ? 1
                           : (modes & MVS_FILE_MODE_READ_WRITE) ? 1
                                                                : 0;
  file->flags.read = (modes & MVS_FILE_MODE_READ)         ? 1
                          : (modes & MVS_FILE_MODE_READ_WRITE) ? 1
                                                               : 0;
  file->flags.new = (modes & MVS_FILE_MODE_CREATE) ? 1 : 0;
  file->in_use = mtrue;
  return MRES_SUCCESS;
}

mResult_t mvs_file_close(MVSFile *file) {
  if (!file)
    return MRES_INVALID_ARGS;
  if (!file->in_use)
    return MRES_SUCCESS;
#ifdef _USE_LINUX_
    close(file->fd);
#else
// not yet
#endif
  file->in_use = mfalse;
  return MRES_SUCCESS;
}

mResult_t mvs_file_destroy(MVSFile *file) {
  if (!file)
    return MRES_INVALID_ARGS;
  mvs_file_close(file);
  free(file);
  return MRES_SUCCESS;
}

mResult_t mvs_file_seek(MVSFile *file, msqword_t off, msize_t whence,
                        msize_t *_len) {
  if (!file)
    return MRES_INVALID_ARGS;
  if (!file->in_use)
    return MRES_RESOURCE_NOT_CONFIGURED;
  if (whence != SEEK_CUR && whence != SEEK_END && whence != SEEK_SET)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  mqword_t len;
  if ((len = lseek(file->fd, off, whence)) == (mqword_t)-1)
    return MRES_SYS_FAILURE;
  if (_len)
    *_len = len;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_file_tell(MVSFile *file, msize_t *off) {
  if (!file || !off)
    return MRES_INVALID_ARGS;
  if (!file->in_use)
    return MRES_RESOURCE_NOT_CONFIGURED;
#ifdef _USE_LINUX_
  if ((msqword_t)(*off = lseek(file->fd, 0, SEEK_CUR)) == -1)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_file_read(MVSFile *file, mbptr_t buf, msize_t num_of_bytes,
                        msize_t *bytes_read) {
  if (!file || !buf)
    return MRES_INVALID_ARGS;
  if (!file->in_use)
    return MRES_RESOURCE_NOT_CONFIGURED;
  if (!file->flags.read)
    return MRES_RESOURCE_STATE_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = read(file->fd, buf, num_of_bytes);
  if (len == -1)
    return MRES_SYS_FAILURE;

  if (bytes_read)
    *bytes_read = len;
  return MRES_SUCCESS;
}

mResult_t mvs_file_write(MVSFile *file, mbptr_t buf, msize_t num_of_bytes,
                         msize_t *bytes_written) {
  if (!file || !buf)
    return MRES_INVALID_ARGS;
  if (!file->in_use)
    return MRES_RESOURCE_NOT_CONFIGURED;
  if ((!file->flags.write && !file->flags.append))
    return MRES_RESOURCE_STATE_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = write(file->fd, buf, num_of_bytes);
  if (len == -1)
    return MRES_SYS_FAILURE;
  if (bytes_written)
    *bytes_written = len;
  return MRES_SUCCESS;
}
