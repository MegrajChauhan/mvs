#ifndef _API_LOGGER_
#define _API_LOGGER_

#include <mvs_types.h>
#include <stdarg.h>

void LOG(mstr_t fmt, ...);
void VLOG(mstr_t fmt, va_list _l);

#endif
