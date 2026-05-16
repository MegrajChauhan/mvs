#ifndef _API_ENTITY_REGISTRATION_
#define _API_ENTITY_REGISTRATION_

#include <api_entity.h>

#define _API_MAKE_ENTITY_REGISTRY_ENTRY_(_create, _destroy, _exec, _getdefsetup, _checksetup) \
		(EntityRegistryEntry) {\
				.create=(_create),\
				.destroy=(_destroy),\
				.exec=(_exec),\
				.get_default_setup=(_getdefsetup),\
				.check_setup=(_checksetup)\
		}

typedef struct EntityRegistryEntry EntityRegistryEntry;

typedef msize_t (*entcreate_t)(EntityContext *, mbptr_t *, mstr_t* /*argv*/, msize_t/*argc*/,msize_t/*conf*/, msize_t/*signature*/); // entity create

/*
 * Returns don't really matter here. The returns are printed out as debug info
 * so it can be considered as return values of the entity
 */
typedef msize_t (*entdestroy_t)(mptr_t); // entity destroy

/*
 * Returns on this are used to define the state of the entity.
 * Graves uses this function to let the entity do some work.
 * This function is used by the entity to let Graves know what it wants.
 * Returns:
 * 0 -> The entity has stopped, thus, terminate the entity
 * 1 -> Continue Running. The entity may return temporarily. Graves will
 * immediately recall the function 2 -> Busy. The entity is busy. The function
 * is immediately recalled but its state is set to MENTITY_BUSY. This state(for
 * now) does nothing. But in the future, it will set DO NOT DISTURB flag and the
 * entity cannot receive any events from other entities. Undefined return values
 * map to 1 automatically. The default starting state MENTITY_RUNNING
 */
typedef msize_t (*entexec_t)(mptr_t); // entity execute

/*
 * If there is no configuration provided for the entity during its creation than, Graves will
 * use the following API call to obtain the default configuration.
 * MVS uses three kinds of behavior setups for an entity. The first is the config, which is used
 * internally by Graves, that cannot be changed once set. The second is the properties, which is also
 * used internally by Graves, that can be changed during runtime using specific requests.
 * The third type is the config used internally by the entity itself. The following API call
 * attempts to obtain the third type of configuration.
 * The following API is an optional one. The entities are not obligated to provide it. If none
 * is provided than Graves will assume no configuration is needed.
 * */
typedef msize_t (*entgetdefsetup_t)(void);

/*
 * If there is some setup provided, that is not default, the following API call is used to verify
 * that the configuration is valid. This is also an optional one and exists purely for efficiency purposes.
 * The configuration is verified before any resource allocations are made. The entity, however, may just 
 * perform the verification during entity creation.
 * Graves guarantees that only verified configuration is passed if verifiable.
 * */
typedef mbool_t (*entchecksetup_t)(msize_t);

struct EntityRegistryEntry {
  entcreate_t create;
  entdestroy_t destroy;
  entexec_t exec;
  entgetdefsetup_t get_default_setup;
  entchecksetup_t check_setup;
};

/*
 * Return:
 * 0 = Success
 * 1 = Already Registered
 * 2 = Invalid entry
 */
msize_t api_register_component(msize_t ID, EntityRegistryEntry *entry);

#endif
