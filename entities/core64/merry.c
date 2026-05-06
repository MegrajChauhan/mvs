#include <merry_core.h>
#include <mvs_tools.h>
#include <mvs_entity_interface.h>

#define _MERRY_EID_ 0

_MVS_ATTR_CONSTRUCTOR_ void merry_register_self() {
   if (mvs_register_component(_MERRY_EID_, merry_core_create, merry_core_destroy, merry_core_run) != 0) {
       // Merry failed to register
	   // There is nothing that can be done
   }
}
