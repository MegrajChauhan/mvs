#include <merry_logger.h>

_MVS_ATTR_INTERNAL_ GravesAPI API;

void merry_logger_init(GravesAPI api) {
  API = api;
}

void MERRY_ERR(mstr_t fmt, ...) {
  msize_t to_write = strlen(fmt);
  char msg[to_write + 7];
  strcpy(msg, "error:");
  strcpy(msg+7, fmt);
  va_list _l;
  va_start(_l, msg);
  API.LOG_CUSTOM(msg, _l);
  va_end(_l);
}

void MERRY_NOTE(mstr_t fmt, ...){
  msize_t to_write = strlen(fmt);
  char msg[to_write + 6];
  strcpy(msg, "note:");
  strcpy(msg+7, fmt);
  va_list _l;
  va_start(_l, msg);
  API.LOG_CUSTOM(msg, _l);
  va_end(_l);
}

void MERRY_DBG(mstr_t fmt, ...){
  msize_t to_write = strlen(fmt);
  char msg[to_write + 7];
  strcpy(msg, "debug:");
  strcpy(msg+7, fmt);
  va_list _l;
  va_start(_l, msg);
  API.LOG_DBG(msg, _l);
  va_end(_l);
}

void MERRY_WARN(mstr_t fmt, ...){
  msize_t to_write = strlen(fmt);
  char msg[to_write + 6];
  strcpy(msg, "warn:");
  strcpy(msg+7, fmt);
  va_list _l;
  va_start(_l, msg);
  API.LOG_CUSTOM(msg, _l);
  va_end(_l);
}
