#include <mvs_interface.h>

mResult_t mvs_interface_init(MVSInterface **interface, mInterface_t type) {
  if (!interface || type >= MINTERFACE_TYPE_LIMIT)
    return MRES_INVALID_ARGS;
  *interface = (MVSInterface *)malloc(sizeof(MVSInterface));
  if (!(*interface)) {
    return MRES_SYS_FAILURE;
  }
  (*interface)->interface = type;
  return MRES_SUCCESS;
}

_MVS_ATTR_ALWAYS_INLINE_ mResult_t
mvs_interface_destroy(MVSInterface *interface) {
  if (!interface)
    return MRES_INVALID_ARGS;
  free(interface);
  return MRES_SUCCESS;
}
