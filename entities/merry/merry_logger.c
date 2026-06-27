#include <merry_logger.h>

_MVS_ATTR_INTERNAL_ _MVS_ATTR_THREAD_LOCAL_ EntityContext *context;

void merry_logger_init(EntityContext *ctx) { context = ctx; }

void MERRY_ERR(mstr_t fmt, ...) {
  msize_t to_write = strlen(fmt);
  char msg[to_write + 7];
  strcpy(msg, "error:");
  strcpy(msg + 6, fmt);
  msg[to_write + 6] = 0;
  va_list _l;
  va_start(_l, fmt);
  context->API.LOG_VCUSTOM(context->self, msg, _l);
  va_end(_l);
}

void MERRY_NOTE(mstr_t fmt, ...) {
  msize_t to_write = strlen(fmt);
  char msg[to_write + 6];
  strcpy(msg, "note:");
  strcpy(msg + 5, fmt);
  msg[to_write + 5] = 0;
  va_list _l;
  va_start(_l, fmt);
  context->API.LOG_VCUSTOM(context->self, msg, _l);
  va_end(_l);
}

void MERRY_DBG(mstr_t fmt, ...) {
  msize_t to_write = strlen(fmt);
  char msg[to_write + 7];
  strcpy(msg, "debug:");
  strcpy(msg + 6, fmt);
  msg[to_write + 6] = 0;
  va_list _l;
  va_start(_l, fmt);
  context->API.LOG_VCUSTOM(context->self, msg, _l);
  va_end(_l);
}

void MERRY_WARN(mstr_t fmt, ...) {
  msize_t to_write = strlen(fmt);
  char msg[to_write + 6];
  strcpy(msg, "warn:");
  strcpy(msg + 5, fmt);
  msg[to_write + 5] = 0;
  va_list _l;
  va_start(_l, fmt);
  context->API.LOG_VCUSTOM(context->self, msg, _l);
  va_end(_l);
}
