#ifndef _MVS_ENTITY_REGISTRY_INTERFACE_
#define _MVS_ENTITY_REGISTRY_INTERFACE_

#include <mvs_entity_interface.h>
#include <mvs_types.h>

typedef msize_t (*mentcreate_t)(MVSEntityContext *, mbptr_t *); // entity create
typedef msize_t (*mentdestroy_t)(mptr_t); // entity destroy
typedef msize_t (*mentexec_t)(mptr_t);    // entity execute

_MVS_ATTR_EXTERNAL_ msize_t mvs_register_component(msize_t ID,
                                                   mentcreate_t create,
                                                   mentdestroy_t destroy,
                                                   mentexec_t exec);

#endif
