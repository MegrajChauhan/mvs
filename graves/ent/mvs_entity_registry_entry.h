#ifndef _MVS_ENTITY_REGISTRY_ENTRY_
#define _MVS_ENTITY_REGISTRY_ENTRY_

#include <mvs_types.h>
#include <mvs_entity_interface.h>

typedef struct MVSEntityRegistryEntry MVSEntityRegistryEntry;

/*
 * Returns:
 * 0 = Success
 * 1 = Failure
 */
typedef msize_t (*mentcreate_t)(MVSEntityContext *, mbptr_t*); // entity create

/*
 * Returns don't really matter here. The returns are printed out as debug info
 * so it can be considered as return values of the entity
 */ 
typedef msize_t (*mentdestroy_t)(mptr_t);   // entity destroy

/*
 * Returns on this are used to define the state of the entity.
 * Graves uses this function to let the entity do some work.
 * This function is used by the entity to let Graves know what it wants.
 * Returns:
 * 0 -> The entity has stopped, thus, terminate the entity
 * 1 -> Continue Running. The entity may return temporarily. Graves will immediately recall the function
 * 2 -> Busy. The entity is busy. The function is immediately recalled but its state is set to MENTITY_BUSY.
 *      This state(for now) does nothing. But in the future, it will set DO NOT DISTURB flag and the entity cannot receive any events from
 *      other entities.
 * Undefined return values map to 1 automatically. The default starting state MENTITY_RUNNING 
 */
typedef msize_t (*mentexec_t)(mptr_t);     // entity execute

struct MVSEntityRegistryEntry {
	mentcreate_t create;
	mentdestroy_t destroy;
	mentexec_t exec;
};

#endif
