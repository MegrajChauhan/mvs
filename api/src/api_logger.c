#include <api_logger.h>
#include <mvs_logger.h>

void LOG(mstr_t fmt, ...) {
  if (!fmt)
    return;
  va_list args;
  va_start(args, fmt);
  mvs_vlog(fmt, args);
  va_end(args);
}

void VLOG(mstr_t fmt, va_list _l) {
  if (!fmt || !_l)
    return;
  mvs_vlog(fmt, _l);
}
