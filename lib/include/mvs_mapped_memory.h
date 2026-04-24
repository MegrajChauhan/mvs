#ifndef _MERRY_MAPPED_MEMORY_
#define _MERRY_MAPPED_MEMORY_

#include <mvs_file.h>
#include <mvs_interface.h>
#include <mvs_mapped_memory_defs.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <stdlib.h>

typedef struct MVSInterface MVSMappedMemory;

/*
 * conf will configure the interface whereas flag defines the behavior of the
 * instance
 */
mResult_t mvs_mapped_memory_create(MVSMappedMemory **map, mqword_t conf);

/*
 * Here, the default map we create is an anonymous mapping.
 * The owner should using the following call to map the size needed by the file
 * if file mapping is needed afterwards
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

mResult_t merry_mapped_memory_destroy(MVSMappedMemory *map);

#endif
