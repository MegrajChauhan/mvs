#ifndef _MERRY_CORE_SYSINT_LIST_
#define _MERRY_CORE_SYSINT_LIST_

#include <merry_core.h>
#include <merry_core_sysint_handler.h>

#define _MSIHDLR_FUNC_(name) merry_core_si##name

_MERRY_DEFINE_FUNC_PTR_(void, mcsihdlr_t, MerryCore *);

mcsihdlr_t SIHDLRS[] = {
	_MSIHDLR_FUNC_(enable_request_list),
};

#endif
