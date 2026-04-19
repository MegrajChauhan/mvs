#include <mvs_interface.h>

mResult_t mvs_interface_create(MVSInterface **interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  *interface = (MVSInterface *)malloc(sizeof(MVSInterface));
  if (!(*interface)) {
    return MRES_SYS_FAILURE;
  }
  (*interface)->configured = mfalse;
  (*interface)->config = 0;
  (*interface)->state = 0;
  (*interface)->flags = 0;
  (*interface)->owner_count = 0;
  (*interface)->interface = MINTERFACE_TYPE_LIMIT;
  return MRES_SUCCESS;
}

mResult_t mvs_interface_configure(MVSInterface *interface, mqword_t conf) {
	if (!interface) 
	  return MRES_INVALID_ARGS;
    if (interface->configured)
      return MRES_SUCCESS;

    interface->config = conf;
    interface->configured = mtrue;
    return MRES_SUCCESS;	
}

mResult_t mvs_interface_free(MVSInterface *interface) {
	if (!interface) 
	  return MRES_INVALID_ARGS;
    if (interface->configured) {
      if ((interface->state & MVS_INTERFACE_STATE_SHARED) && interface->owner_count)
        return MRES_RESOURCE_SHARED;
    }
    interface->state = 0; // not initialized i.e free to reuse
    interface->interface = MINTERFACE_TYPE_LIMIT;
    interface->configured = mfalse;
    return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_interface_destroy(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  if (interface->configured && (interface->state & MVS_INTERFACE_STATE_SHARED) && interface->owner_count)
    return MRES_RESOURCE_SHARED;
  free(interface);
  return MRES_SUCCESS;
}

mResult_t mvs_interface_init(MVSInterface *interface, mInterface_t type) {
  if (!interface || (type >= MINTERFACE_TYPE_LIMIT))
    return MRES_INVALID_ARGS;
  if (!interface->configured)
    mvs_interface_configure(interface, 0); // default configure
  if (interface->state & MVS_INTERFACE_STATE_INITIALIZED) // already initialized
    return MRES_RESOURCE_STATE_INVALID;
  interface->state = MVS_INTERFACE_STATE_INITIALIZED;
  interface->type = type;
  return MRES_SUCCESS;
}

mResult_t mvs_interface_set_flags(MVSInterface *interface, mqword_t flags) {
	if (!interface)
	  return MRES_INVALID_ARGS;
	if (!interface->configured) 
      return MRES_RESOURCE_NOT_CONFIGURED;
	if (!(interface->state & MVS_INTERFACE_STATE_INITIALIZED))
	  return MRES_RESOURCE_STATE_INVALID;
	// check flags
	// set flags
	return MRES_SUCCESS;
}

mResult_t mvs_interface_share(MVSInterface **source, MVSInterface **dest) {
	if (!source || !dest)
	  return MRES_INVALID_ARGS;
	if (!(*source)->configured) 
      return MRES_RESOURCE_NOT_CONFIGURED;
	if (!((*source)->state & MVS_INTERFACE_STATE_INITIALIZED))
       return MRES_RESOURCE_STATE_INVALID;
    if (!((*source)->conf & MVS_INTERFACE_CONF_SHAREABLE))
       return MRES_RESOURCE_STATE_INVALID;
    (*source)->owner_count++;
    (*source)->state |= MVS_INTERFACE_STATE_SHARED;   
    *dest = *source;
    return MRES_SUCCESS;
}

mResult_t mvs_interface_disown(MVSInterface *interface) {
	if (!interface)
	  return MRES_INVALID_ARGS;
	if (!interface->configured) 
      return MRES_RESOURCE_NOT_CONFIGURED;
	if (!(interface->state & MVS_INTERFACE_STATE_INITIALIZED) || !(interface->conf & MVS_INTERFACE_CONF_SHAREABLE))
	  return MRES_RESOURCE_STATE_INVALID;
	if (!(interface->state & MVS_INTERFACE_STATE_SHARED))
	  return MRES_RESOURCE_STATE_INVALID;
    if (interface->owner_count)
      interface->owner_count--;
    return MRES_SUCCESS;		
}
