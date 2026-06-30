#include <mvs_dynamic_lib.h>

/*
 * This implementation of the dynamically loading a library is limited. The
 * entirety of the 'lib' needs more work. For now, this will fulfill the
 * requirements of MVS.
 * */

mResult_t mvs_dynamic_lib_create(MVSDynamicLib **lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  *lib = NULL;
  MVSDynamicLib *l = (MVSDynamicLib*)malloc(sizeof(MVSDynamicLib));
  if (!l)
    return MRES_SYS_FAILURE;
  l->entry = NULL;
  l->in_use = mfalse;
  *lib = l;
  return MRES_SUCCESS;
}

#include <stdio.h>
mResult_t mvs_dynamic_lib_load_library(MVSDynamicLib *lib, mstr_t path) {
  if (!lib || !path)
    return MRES_INVALID_ARGS;
  if (lib->in_use)
    return MRES_RESOURCE_ALREADY_IN_USE;
#if defined(_USE_LINUX_)
  lib->entry = dlopen(path, RTLD_LAZY);
  if (lib->entry == NULL) {
    printf("ERROR: %s\n", dlerror());
    return MRES_SYS_FAILURE;
  }
#endif
  lib->in_use = mtrue;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_unload_library(MVSDynamicLib *lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  if (!lib->in_use)
    return MRES_RESOURCE_STATE_INVALID;
#if defined(_USE_LINUX_)
  dlclose(lib->entry);
#elif defined(_USE_WIN_)
#endif
  lib->entry = NULL;
  lib->in_use = mfalse;
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_get_symbol(MVSDynamicLib *lib, mstr_t sym_name,
                                     mptr_t *res) {
  if (!lib || !sym_name || !res)
    return MRES_INVALID_ARGS;
  if (!lib->in_use)
    return MRES_RESOURCE_STATE_INVALID; // not in use!
#if defined(_USE_LINUX_)
  *res = dlsym(lib->entry, sym_name);
  if (!(*res)) {
    return MRES_SYS_FAILURE;
  }
#endif
  return MRES_SUCCESS;
}

mResult_t mvs_dynamic_lib_destroy(MVSDynamicLib *lib) {
  if (!lib)
    return MRES_INVALID_ARGS;
  if (lib->in_use)
    mvs_dynamic_lib_unload_library(lib);
  free(lib);
  return MRES_SUCCESS;
}
