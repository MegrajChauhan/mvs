#ifndef _MVS_RLIST_
#define _MVS_RLIST_

/*
 * RLIST, or Registry List, is a list that is responsible to loading entities dynamically
 * MVS expects an entities.l file at launch which is read to load a few of the entities
 * from the get-go. As MVS runs, the running entities will have the power to load more 
 * entities as needed. This will make sure that only needed entities are loaded.
 * Of course, the dynamically loaded entities at runtime are not managed by RLIST.
 * */

#include <mvs_rlist_reader.h>
#include <mvs_entity_registry.h>
#include <mvs_dynamic_lib.h>
#include <api_graves.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct MVSRlist MVSRlist;

struct MVSRlist {
		MVSRlistReader *reader;
		MVSDynamicLib** entity_libs;
		msize_t count;
};

mbool_t mvs_rlist_init(MVSRlist *rlist);

void mvs_rlist_destroy(MVSRlist *rlist);

mbool_t mvs_rlist_read(MVSRlist *rlist);

mbool_t mvs_rlist_register_entities(MVSRlist *rlist, GravesAPI *API);

_MVS_ATTR_ALWAYS_INLINE_ msize_t mvs_rlist_get_count(MVSRlist *rlist) {
		return rlist->count;
}

#endif
