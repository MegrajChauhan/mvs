#include <mvs_entity_registry.h>

MVSEntityRegistry registry = {0};

msize_t mvs_register_component(msize_t ID, mentcreate_t create,
                               mentdestroy_t destroy, mentexec_t exec) {
  if (ID >= _MVS_CONSTANT_ENTITY_COUNT_)
    return 2;
  if (registry.entries[ID].registered)
    return 1;
  if (!create || !destroy || !exec)
    return 2;
  registry.entries[ID].API.create = create;
  registry.entries[ID].API.destroy = destroy;
  registry.entries[ID].API.exec = exec;
  registry.entries[ID].registered = mtrue;
  registry.entities_registered++;
  return 0;
}

MVSEntityRegistryEntry *mvs_registry_get_entry(msize_t ID) {
  // This function is available to Graves only and it will only use valid ID
  return registry.entries[ID].registered ? &registry.entries[ID].API : NULL;
}

msize_t mvs_registry_get_count() { return registry.entities_registered; }
