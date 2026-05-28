#include <mvs_entity_registry.h>

MVSEntityRegistry registry = {0};

msize_t mvs_register_component(msize_t ID, EntityRegistryEntry *entry) {
  if (ID >= _MVS_CONSTANT_ENTITY_COUNT_)
    return 2;
  if (registry.entries[ID].registered)
    return 1;
  if (!entry || !entry->create || !entry->destroy || !entry->exec ||
      !entry->deduce_setup)
    return 2;
  registry.entries[ID].API = *entry;
  registry.entries[ID].registered = mtrue;
  registry.entities_registered++;
  return 0;
}

EntityRegistryEntry *mvs_registry_get_entry(msize_t ID) {
  // This function is available to Graves only and it will only use valid ID
  return registry.entries[ID].registered ? &registry.entries[ID].API : NULL;
}

msize_t mvs_registry_get_count() { return registry.entities_registered; }
