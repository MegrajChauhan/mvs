#ifndef _MVS_RLIST_
#define _MVS_RLIST_

/*
 * RLIST, or Registry List, is a list that is responsible to loading entities
 * dynamically. RLIST loads these entities and provides a way for MVS to acccess
 * them. MVS expects an entities.l list file at launch which is read to load a
 * few of the entities from the get-go. As MVS runs, the running entities will
 * have the power to load more entities as needed. This will make sure that only
 * needed entities are loaded.
 * */

#include <api_graves.h>
#include <mvs_dynamic_lib.h>
#include <mvs_entity_registry.h>
#include <mvs_list.h>
#include <mvs_logger.h>
#include <mvs_rlist_reader.h>
#include <mvs_helpers.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSRlist MVSRlist;
typedef EntityRegistryEntry (*mentityRegister_t)();

struct MVSRlist {
  MVSDynamicListLinear *entity_libs;
  msize_t count;
};

mbool_t mvs_rlist_init(MVSRlist *rlist);

void mvs_rlist_destroy(MVSRlist *rlist);

mbool_t mvs_rlist_load_from_list_file(MVSRlist *rlist, mstr_t file_path);

mbool_t mvs_rlist_load_from_path(MVSRlist *rlist, mstr_t path);

mentityRegister_t mvs_rlist_get_registrar(MVSRlist *rlist, msize_t EID,
                                          mbool_t *sys);

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_rlist_get_count(MVSRlist *rlist) {
  return rlist->count;
}

#endif
