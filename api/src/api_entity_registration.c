#include <api_entity_registration.h>
#include <mvs_entity_registry.h>

msize_t api_register_component(msize_t ID, EntityRegistryEntry *entry) {
  return mvs_register_component(ID, entry);
}
