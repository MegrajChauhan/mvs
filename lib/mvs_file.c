#include <mvs_file.h>

mResult_t mvs_file_create(MVSFile **file, mqword_t conf) {
  if (!file)
    return MRES_INVALID_ARGS;
  mResult_t res = mvs_interface_create(file);
  if (res != MRES_SUCCESS)
    return res;
  if ((res = mvs_interface_configure(*file, conf)) != MRES_SUCCESS)
    return res;
  if ((res = mvs_interface_init(*file, MINTERFACE_TYPE_FILE)) != MRES_SUCCESS) {
    mvs_interface_destroy(*file);
    *file = NULL;
    return res;
  }
  return MRES_SUCCESS;
}

mResult_t mvs_file_open(MVSFile *file, mstr_t file_path, mqword_t modes) {
  if (!file || !file_path)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE)
    return MRES_RESOURCE_TYPE_INVALID;
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
      mvs_interface_free(file); // free the interface as stated
      return MRES_INVALID_ARGS;
    }
    open_modes |= O_WRONLY;
  }
  if ((modes & MVS_FILE_MODE_READ_WRITE)) {
    if (((modes & MVS_FILE_MODE_WRITE) || (modes & MVS_FILE_MODE_READ) ||
         (modes & MVS_FILE_MODE_APPEND))) {
      mvs_interface_free(file); // free the interface as stated
      return MRES_INVALID_ARGS;
    }
    open_modes |= O_RDWR;
  }
  if (modes & MVS_FILE_MODE_CREATE)
    open_modes |= O_CREAT;
  file->file.fd = open(file_path, open_modes, MVS_FILE_MODE_PERMISSIONS);
  if (file->file.fd == -1) {
    mvs_interface_free(file); // free the interface as stated
    return MRES_SYS_FAILURE;
  }
#else
// not yet
#endif
  file->file.flags.append = (modes & MVS_FILE_MODE_APPEND) ? 1 : 0;
  file->file.flags.write = (modes & MVS_FILE_MODE_WRITE)        ? 1
                           : (modes & MVS_FILE_MODE_READ_WRITE) ? 1
                                                                : 0;
  file->file.flags.read = (modes & MVS_FILE_MODE_READ)         ? 1
                          : (modes & MVS_FILE_MODE_READ_WRITE) ? 1
                                                               : 0;
  file->file.flags.new = (modes & MVS_FILE_MODE_CREATE) ? 1 : 0;
  return MRES_SUCCESS;
}

mResult_t mvs_file_close(MVSFile *file) {
  if (!file)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE) {
    return MRES_RESOURCE_TYPE_INVALID;
  }
#ifdef _USE_LINUX_
  close(file->file.fd);
#else
// not yet
#endif
  mvs_interface_free(file); // to be used for something else
  return MRES_SUCCESS;
}

mResult_t mvs_file_destroy(MVSFile *file) {
  if (!file)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE) {
    return MRES_RESOURCE_TYPE_INVALID;
  }
  mvs_file_close(file);
  mvs_interface_destroy(file);
  return MRES_SUCCESS;
}

mResult_t mvs_file_seek(MVSFile *file, msqword_t off, msize_t whence,
                        msize_t *_len) {
  if (!file)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE)
    return MRES_RESOURCE_TYPE_INVALID;
  if (whence != SEEK_CUR && whence != SEEK_END && whence != SEEK_SET)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  mqword_t len;
  if ((len = lseek(file->file.fd, off, whence)) == -1)
    return MRES_SYS_FAILURE;
  if (_len)
    *_len = len;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_file_tell(MVSFile *file, msize_t *off) {
  if (!file || !off)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE)
    return MRES_RESOURCE_TYPE_INVALID;
#ifdef _USE_LINUX_
  if ((msqword_t)(*off = lseek(file->file.fd, 0, SEEK_CUR)) == -1)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_file_read(MVSFile *file, mbptr_t buf, msize_t num_of_bytes,
                        msize_t *bytes_read) {
  if (!file || !buf)
    return MRES_INVALID_ARGS;
  if (file->interface != MINTERFACE_TYPE_FILE)
    return MRES_RESOURCE_TYPE_INVALID;
  if (!file->file.flags.read)
    return MRES_RESOURCE_STATE_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = read(file->file.fd, buf, num_of_bytes);
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
  if (file->interface != MINTERFACE_TYPE_FILE)
    return MRES_RESOURCE_TYPE_INVALID;
  if ((!file->file.flags.write && !file->file.flags.append))
    return MRES_RESOURCE_STATE_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = write(file->file.fd, buf, num_of_bytes);
  if (len == -1)
    return MRES_SYS_FAILURE;
  if (bytes_written)
    *bytes_written = len;
  return MRES_SUCCESS;
}
