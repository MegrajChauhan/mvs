#include <mvs_dynamic_lib.h>

/*
 * This implementation of the dynamically loading a library is limited. The entirety of the 'lib' needs more work. 
 * For now, this will fulfill the requirements of MVS.
 * */

mResult_t mvs_dynamic_lib_create(MVSDynamicLib **lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  mResult_t res = mvs_interface_create(lib);
  if (res != MRES_SUCCESS) {
    *lib = NULL;
    return res;
  }
  mvs_interface_configure(*lib, 0);
  if ((res = mvs_interface_init(*lib, MINTERFACE_TYPE_DYNAMIC_LIBRARY)) !=
      MRES_SUCCESS) {
    mvs_interface_destroy(*lib);
    return res;
  }
  (*lib)->dynamic_lib.entry = NULL;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_load_library(MVSDynamicLib *lib, mstr_t path) {
  if (!lib || !path)
    return MRES_INVALID_ARGS;
  if (lib->interface != MINTERFACE_TYPE_DYNAMIC_LIBRARY)
    return MRES_RESOURCE_TYPE_INVALID;
  if (lib->dynamic_lib.entry)
    return MRES_RESOURCE_STATE_INVALID; // already in use!
#if defined(_USE_LINUX_)
  lib->dynamic_lib.entry = dlopen(path, RTLD_LAZY);
  if (lib->dynamic_lib.entry == NULL) {
    return MRES_SYS_FAILURE;
  }
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_unload_library(MVSDynamicLib *lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  if (lib->interface != MINTERFACE_TYPE_DYNAMIC_LIBRARY)
    return MRES_RESOURCE_TYPE_INVALID;
  if (!lib->dynamic_lib.entry)
    return MRES_RESOURCE_STATE_INVALID; // not in use!
#if defined(_USE_LINUX_)
  dlclose(lib->dynamic_lib.entry);
#elif defined(_USE_WIN_)
#endif
  lib->dynamic_lib.entry = NULL;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_get_symbol(MVSDynamicLib *lib, mstr_t sym_name,
                                     mptr_t *res) {
  if (!lib || !sym_name || !res)
    return MRES_INVALID_ARGS;
  if (lib->interface != MINTERFACE_TYPE_DYNAMIC_LIBRARY)
    return MRES_RESOURCE_TYPE_INVALID;
  if (!lib->dynamic_lib.entry)
    return MRES_RESOURCE_STATE_INVALID; // not in use!
#if defined(_USE_LINUX_)
  *res = dlsym(lib->dynamic_lib.entry, sym_name);
  if (!(*res))
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_destroy(MVSDynamicLib *lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  if (lib->interface != MINTERFACE_TYPE_DYNAMIC_LIBRARY)
    return MRES_RESOURCE_TYPE_INVALID;
  if (lib->dynamic_lib.entry)
    mvs_dynamic_lib_unload_library(lib);
  mvs_interface_destroy(lib);
  return MRES_SUCCESS;
}
