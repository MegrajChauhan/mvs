#include <mvs_entity_registry.h>

_MVS_ATTR_INTERNAL_ MVSEntityRegistry registry = {0};

mbool_t mvs_registry_init(MVSSystemConfig *conf) {
  registry.config = conf;
  if ((registry.entries = (MVSEntityRegistryPackage *)malloc(
           sizeof(MVSEntityRegistryPackage) * conf->MAX_EID)) == NULL)
    return mfalse;
  memset(registry.entries, 0, sizeof(MVSEntityRegistryPackage) * conf->MAX_EID);
  return mtrue;
}

void mvs_registry_destroy() {
  if (registry.entries)
    free(registry.entries);
}

_MVS_ATTR_EXPORT_
msize_t mvs_register_component(msize_t ID, EntityRegistryEntry *entry) {
  if (ID >= registry.config->MAX_EID)
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
