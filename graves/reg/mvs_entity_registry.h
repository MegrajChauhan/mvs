#ifndef _MVS_ENTITY_REGISTRY_
#define _MVS_ENTITY_REGISTRY_

#include <api_entity.h>
#include <mvs_graves_constants.h>
#include <mvs_system_config.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct MVSEntityRegistry MVSEntityRegistry;
typedef struct MVSEntityRegistryPackage MVSEntityRegistryPackage;

struct MVSEntityRegistryPackage {
  mbool_t registered;
  EntityRegistryEntry API;
};

struct MVSEntityRegistry {
  msize_t entities_registered;
  MVSSystemConfig *config;
  MVSEntityRegistryPackage *entries;
};

mbool_t mvs_registry_init(MVSSystemConfig *conf);

void mvs_registry_destroy();

/*
 * Return:
 * 0 = Success
 * 1 = Already Registered
 * 2 = Invalid entry
 */
_MVS_ATTR_EXPORT_ msize_t mvs_register_component(msize_t ID,
                                                 EntityRegistryEntry *entry);

EntityRegistryEntry *mvs_registry_get_entry(msize_t ID);

msize_t mvs_registry_get_count();

#endif
