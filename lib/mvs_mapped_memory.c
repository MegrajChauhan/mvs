#include <mvs_mapped_memory.h>

mResult_t mvs_mapped_memory_create(MVSMappedMemory **map) {
  if (!map)
    return MRES_INVALID_ARGS;
  *map = NULL;
  MVSMappedMemory *m = (MVSMappedMemory*)malloc(sizeof(MVSMappedMemory));
  if (!m)
    return MRES_SYS_FAILURE;
  m->addr_space = NULL;
  m->addr_space_len = 0;
  m->backing = NULL;
  m->in_use = mfalse;
  *map = m;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_add_align_param(MVSMappedMemory *map,
                                            msize_t align_param) {
  if (!map || !align_param)
    return MRES_INVALID_ARGS;
  map->align_param = align_param;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_map(MVSMappedMemory *map, msize_t len) {
  if (!map || len == 0)
    return MRES_INVALID_ARGS;
  if (map->in_use)
    return MRES_RESOURCE_STATE_INVALID;
#ifdef _USE_LINUX_
  int f = MAP_ANONYMOUS | MAP_PRIVATE;
  int prot = PROT_READ | PROT_WRITE;
  if ((map->addr_space = mmap(NULL, len, prot, f, -1, 0)) == NULL)
    return MRES_SYS_FAILURE;
#endif
  map->addr_space_len = len;
  map->offset = 0;
  map->flags.can_read = 1;
  map->flags.can_write = 1;
  map->flags.can_exec = 0;
  map->flags.shared = 0;
  map->flags.private = 1;
  map->flags.sync = 0;
  map->in_use = mtrue;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_unmap(MVSMappedMemory *map) {
  if (!map)
    return MRES_INVALID_ARGS;
  if (!map->in_use)
    return MRES_RESOURCE_STATE_INVALID;
#ifdef _USE_LINUX_
  munmap(map->addr_space, map->addr_space_len);
#endif
  map->addr_space = NULL;
  map->addr_space_len = 0;
  if (map->backing) {
    mvs_file_destroy(map->backing);
	map->backing = NULL;
  }
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_mapf(MVSMappedMemory *map, mstr_t file_path,
                                 msize_t offset, mqword_t flags) {
  if (!map)
    return MRES_INVALID_ARGS;
  if (map->backing)
    return MRES_RESOURCE_ALREADY_IN_USE;
  if (!file_path && offset > map->addr_space_len)
    return MRES_RESOURCE_SIZE_LIMITED;
  int host_flag = 0;
  int host_mode = 0;
  mbool_t r = mfalse, w = mfalse, e = mfalse, s = mfalse, p = mfalse,
          syn = mfalse, align = mfalse;
/*
 * I am well-aware of the nighmare that in-function flag decoding might cause
 * especially when many platforms need to be supported but that will be taken
 * care of later on or I will just stay with this method and write absurdly
 * large functions
 */
#ifdef _USE_LINUX_
  if (flags & MVS_MMAPPED_FLAG_READ) {
    host_mode |= PROT_READ;
    r = mtrue;
  }
  if (flags & MVS_MMAPPED_FLAG_WRITE) {
    host_mode |= PROT_WRITE;
    w = mtrue;
  }
  if (flags & MVS_MMAPPED_FLAG_EXEC) {
    host_mode |= PROT_EXEC;
    e = mtrue;
  }
  if (flags & MVS_MMAPPED_FLAG_SHARE) {
    host_flag |= MAP_SHARED;
    s = mtrue;
  }
  if ((flags & MVS_MMAPPED_FLAG_PRIVATE) && s) {
    return MRES_INVALID_ARGS;
  } else {
    host_flag |= MAP_PRIVATE;
    p = mtrue;
  }
  if (flags & MVS_MMAPPED_FLAG_SYNC) {
    syn = mtrue;
  }
  if (flags & MVS_MMAPPED_FLAG_ALIGN) {
    align = mtrue;
  }
  if (!host_mode) {
    host_mode = PROT_READ | PROT_WRITE;
    r = mtrue;
    w = mtrue;
  }
  if (!host_flag) {
    host_flag = MAP_PRIVATE;
    p = mtrue;
  }
#endif

  msize_t file_len = 0, original_file_len = 0;
  if (file_path) {
    mqword_t file_flags = 0;
    if (r)
      file_flags = MVS_FILE_MODE_READ;
    if (w)
      file_flags = MVS_FILE_MODE_WRITE;
    if (r && w)
      file_flags = MVS_FILE_MODE_READ_WRITE;
    mResult_t res =
        mvs_file_create(&map->backing); // no configuration
    if (res != MRES_SUCCESS)
      return res;
    if ((res = mvs_file_open(map->backing, file_path, file_flags)) !=
        MRES_SUCCESS) {
      mvs_file_destroy(map->backing);
      map->backing = NULL;
      return res;
    }
    if ((res = mvs_file_seek(map->backing, 0, SEEK_END,
                             &file_len)) != MRES_SUCCESS) {
      mvs_file_destroy(map->backing);
      map->backing = NULL;
      return res;
    }
    if ((res = mvs_file_seek(map->backing, 0, SEEK_SET, NULL)) !=
        MRES_SUCCESS) {
      mvs_file_destroy(map->backing);
      map->backing = NULL;
      return res;
    }
    original_file_len = file_len;
    if (align) {
      if (!map->align_param) {
        mvs_file_destroy(map->backing);
        map->backing = NULL;
        return MRES_INVALID_ARGS;
      }
      file_len += (map->align_param -
                   (file_len % map->align_param));
    }
    if (map->addr_space)
      mvs_mapped_memory_unmap(map);
    if ((res = mvs_mapped_memory_map(map, file_len)) != MRES_SUCCESS) {
      mvs_file_destroy(map->backing);
      map->backing = NULL;
      return res;
    }
  }

#ifdef _USE_LINUX_
  if (file_path) {
    if ((map->addr_space = mmap(
             map->addr_space, original_file_len, host_mode,
             host_flag, map->backing->fd, offset)) == NULL) {
      mvs_file_destroy(map->backing);
      map->backing = NULL;
      return MRES_SYS_FAILURE;
    }
    map->offset = offset;
    map->addr_space_len = original_file_len;
  } else {
    if ((map->addr_space = mmap(map->addr_space, offset,
                                           host_mode, host_flag, -1, 0)) ==
        NULL) {
      return MRES_SYS_FAILURE;
    }
    map->offset = 0;
    map->addr_space_len = offset;
  }
#endif
  map->flags.can_read = r;
  map->flags.can_write = w;
  map->flags.can_exec = e;
  map->flags.shared = s;
  map->flags.private = p;
  map->flags.sync =
      syn; // Note that this flag doesn't do anything yet
  map->in_use = mtrue;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_obtain_ptr(MVSMappedMemory *map, mbptr_t *ptr,
                                       msize_t off) {
  if (!map || !ptr)
    return MRES_INVALID_ARGS;
  if (!map->in_use)
    return MRES_RESOURCE_STATE_INVALID;
  if (off > map->addr_space_len)
    return MRES_INVALID_ARGS;
  *ptr = (mbptr_t)map->addr_space + off;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_obtain_map_size(MVSMappedMemory *map,
                                            msize_t *len) {
  if (!map || !len)
    return MRES_INVALID_ARGS;
  if (!map->in_use)
    return MRES_RESOURCE_STATE_INVALID;
  *len = map->addr_space_len;
  return MRES_SUCCESS;
}

mResult_t mvs_mapped_memory_destroy(MVSMappedMemory *map) {
  if (!map)
    return MRES_INVALID_ARGS;
  if (map->addr_space)
    mvs_mapped_memory_unmap(map);
  free(map);
  return MRES_SUCCESS;
}
