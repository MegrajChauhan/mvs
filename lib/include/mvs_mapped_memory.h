#ifndef _MERRY_MAPPED_MEMORY_
#define _MERRY_MAPPED_MEMORY_

#include <mvs_file.h>
#include <mvs_mapped_memory_defs.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <stdlib.h>

typedef struct MVSMappedMemory MVSMappedMemory;

struct MVSMappedMemory {
  mbool_t in_use;
  mmap_t addr_space;
  msize_t addr_space_len;
  msize_t offset;
  msize_t align_param;
  MVSFile *backing; // Is there a backing file for this mapped memory?

      // if backing == NULL than, the mapping is anonymous
  struct {
    msize_t can_read : 1;
    msize_t can_write : 1;
    msize_t can_exec : 1; // This is ignored really

    msize_t shared : 1;  // The updates on the address space is shared with
                         // other processes sharing the same memory region
    msize_t private : 1; // Opposite of shared
    msize_t sync : 1; // This causes the changes made on the mapped memory
                      // to be immediately reflected on the file(the
                      // operation is synchronous)

    msize_t resb : 58;
  } flags;
};

mResult_t mvs_mapped_memory_create(MVSMappedMemory **map);

mResult_t mvs_mapped_memory_add_align_param(MVSMappedMemory *map,
                                            msize_t align_param);
/*
 * Here, the default map we create is an anonymous mapping.
 * If 'map' is to be used for a file mapping, than, directly use
 * mvs_mapped_memory_mapf and it will allocate an anonymous map automatically
 * before mapping the actual file
 */
mResult_t mvs_mapped_memory_map(MVSMappedMemory *map, msize_t len);

mResult_t mvs_mapped_memory_unmap(MVSMappedMemory *map);

/*
 * The flags here are owner-specified. MVS_MMAPPED_FLAG_EXEC doesn't do
 * anything(yet) If file_path is NULL than the mapping can be configured
 * according to the flags. But if file_path is NULL, than offset refers to the
 * size of the mapping and it should't be larger than the original mapping. If
 * mvs_mapped_memory_map hasn't been called yet and, thus, no mapping created,
 * the function will fail immediately. The file is opened by default in such a
 * way that it doesn't conflict with the flags provided in 'flags'. If flags = 0
 * than the default mapping is set: for backing file- RW+P(Copy-On Write), for
 * mapped memory- RW+P
 */
mResult_t mvs_mapped_memory_mapf(MVSMappedMemory *map, mstr_t file_path,
                                 msize_t offset, mqword_t flags);

mResult_t mvs_mapped_memory_obtain_ptr(MVSMappedMemory *map, mbptr_t *ptr,
                                       msize_t off);

mResult_t mvs_mapped_memory_obtain_map_size(MVSMappedMemory *map, msize_t *len);

mResult_t mvs_mapped_memory_destroy(MVSMappedMemory *map);

#endif
