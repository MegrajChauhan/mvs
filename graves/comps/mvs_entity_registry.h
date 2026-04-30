#ifndef _MVS_ENTITY_REGISTRY_
#define _MVS_ENTITY_REGISTRY_

#include <mvs_types.h>
#include <mvs_tools.h>
#include <mvs_entity_registry_entry.h>
#include <mvs_graves_constants.h>
#include <stddef.h>

typedef struct MVSEntityRegistry MVSEntityRegistry; 

// since constructors are executed sequentially, thus, there is no need to synchronize
struct MVSEntityRegistry {
	msize_t entities_registered;
	struct {
		mbool_t registered;
		MVSEntityRegistryEntry API;
	} entries[_MVS_CONSTANT_ENTITY_COUNT_];
};

_MVS_ATTR_EXTERNAL_ MVSEntityRegistry registry;

/*
 * Return:
 * 0 = Success
 * 1 = Already Registered
 * 2 = Invalid entry
 */
msize_t mvs_register_component(msize_t ID, mentcreate_t create, mentdestroy_t destroy, mentexec_t exec);

MVSEntityRegistryEntry* mvs_registry_get_entry(msize_t ID);

msize_t mvs_registry_get_count();

#endif
