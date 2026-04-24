#ifndef _MVS_DYNL_
#define _MVS_DYNL_

#include <mvs_config.h>
#include <mvs_interface.h>
#include <mvs_platform.h>
#include <mvs_results.h>
#include <mvs_types.h>
#include <mvs_utils.h>

// we don't know what type of function we might want
typedef MVSInterface MVSDynamicLib;

mResult_t mvs_dynamic_lib_create(MVSDynamicLib **lib);

mResult_t mvs_dynamic_lib_load_library(MVSDynamicLib *lib, mstr_t path);

mResult_t mvs_dynamic_lib_unload_library(MVSDynamicLib *lib);

mResult_t mvs_dynamic_lib_get_symbol(MVSDynamicLib *lib, mstr_t sym_name,
                                     mptr_t *res);

mResult_t mvs_dynamic_lib_destroy(MVSDynamicLib *lib);

#endif
