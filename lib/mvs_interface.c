#include <mvs_interface.h>

mResult_t mvs_interface_create(MVSInterface **interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  *interface = (MVSInterface *)malloc(sizeof(MVSInterface));
  if (!(*interface)) {
    return MRES_SYS_FAILURE;
  }
  if (mvs_mutex_init(&(*interface)->lock) != MRES_SUCCESS) {
    free(*interface);
	return MRES_SYS_FAILURE;
  }
  (*interface)->configured = mfalse;
  (*interface)->config = 0;
  atomic_init(&(*interface)->state, 0);
  atomic_init(&(*interface)->flags, 0);
  atomic_init(&(*interface)->owner_count, 0);
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

mResult_t mvs_interface_lock(MVSInterface *interface) {
  /*
   * In the case of locking, there won't be any excessive checks.
   * */
   if (!interface)
     return MRES_INVALID_ARGS;
   mvs_mutex_lock(&interface->lock);
   return MRES_SUCCESS;
}

mResult_t mvs_interface_unlock(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  mvs_mutex_unlock(&interface->lock);
  return MRES_SUCCESS;
}

mResult_t mvs_interface_free(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  if (interface->configured) {
    if ((interface->state & MVS_INTERFACE_STATE_SHARED) &&
        interface->owner_count)
      return MRES_RESOURCE_SHARED;
  }
  interface->state = 0; // not initialized i.e free to reuse
  interface->interface = MINTERFACE_TYPE_LIMIT;
  interface->configured = mfalse;
  return MRES_SUCCESS;
}

mResult_t mvs_interface_destroy(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  if (interface->configured &&
      (interface->state & MVS_INTERFACE_STATE_SHARED) && interface->owner_count)
    return MRES_RESOURCE_SHARED;
  mvs_mutex_destroy(&interface->lock);
  free(interface);
  return MRES_SUCCESS;
}

mResult_t mvs_interface_init(MVSInterface *interface, mInterface_t type) {
  if (!interface || (type >= MINTERFACE_TYPE_LIMIT))
    return MRES_INVALID_ARGS;
  if (!interface->configured)
    mvs_interface_configure(interface, 0);                // default configure
  if (interface->state & MVS_INTERFACE_STATE_INITIALIZED) // already initialized
    return MRES_RESOURCE_STATE_INVALID;
  interface->state = MVS_INTERFACE_STATE_INITIALIZED;
  interface->interface = type;
  interface->owner_count = 1;
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
  atm_mqword_t state = atomic_load_explicit(&(*source)->state, memory_order_relaxed);
  if (!(state & MVS_INTERFACE_STATE_INITIALIZED))
    return MRES_RESOURCE_STATE_INVALID;
  if (!((*source)->config & MVS_INTERFACE_CONF_SHAREABLE))
    return MRES_RESOURCE_STATE_INVALID;
  atomic_fetch_add(&(*source)->owner_count, 1);
  atomic_store_explicit(&(*source)->state, state | MVS_INTERFACE_STATE_SHARED, memory_order_release);
  *dest = *source;
  return MRES_SUCCESS;
}

mResult_t mvs_interface_disown(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  if (!interface->configured)
    return MRES_RESOURCE_NOT_CONFIGURED;
  atm_mqword_t state = atomic_load_explicit(&interface->state, memory_order_relaxed);
  if (!(state & MVS_INTERFACE_STATE_INITIALIZED) ||
      !(interface->config & MVS_INTERFACE_CONF_SHAREABLE))
    return MRES_RESOURCE_STATE_INVALID;
  if (!(state & MVS_INTERFACE_STATE_SHARED))
    return MRES_RESOURCE_STATE_INVALID;
  if (atomic_load_explicit(&interface->owner_count, memory_order_relaxed))
    atomic_fetch_sub(&interface->owner_count, 1, memory_order_release);
  return MRES_SUCCESS;
}

mResult_t mvs_interface_check_freeable(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  if (!interface->configured)
    return MRES_RESOURCE_NOT_CONFIGURED;
  atm_mqword_t state = atomic_load_explicit(&interface->state, memory_order_relaxed);
  if (!(state & MVS_INTERFACE_STATE_INITIALIZED))
    return MRES_RESOURCE_STATE_INVALID;
  if (!(interface->config & MVS_INTERFACE_CONF_SHAREABLE))
    return MRES_SUCCESS; // it is freeable
  if (!(state & MVS_INTERFACE_STATE_SHARED))
    return MRES_SUCCESS; // freeable
  /*
   * The reason the condition checks for one owner is because the last owner is the one making
   * the check.
   * */
  if (atomic_load_explicit(&interface->owner_count, memory_order_relaxed) > 1)
    return MRES_RESOURCE_SHARED;
  return MRES_SUCCESS; // freeable
}
