#ifndef _MERRY_CORE_SYSINT_HANDLER_
#define _MERRY_CORE_SYSINT_HANDLER_

#include <merry_core.h>

#define merry_core_siHDLR(name) void merry_core_si##name(MerryCore *)

merry_core_siHDLR(enable_request_list);

#endif
