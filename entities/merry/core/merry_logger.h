#ifndef _MERRY_LOGGER_
#define _MERRY_LOGGER_

#include <api_graves.h>

void merry_logger_init(GravesAPI api, EntityContext *ctx);

void MERRY_ERR(mstr_t fmt, ...);
void MERRY_NOTE(mstr_t fmt, ...);
void MERRY_DBG(mstr_t fmt, ...);
void MERRY_WARN(mstr_t fmt, ...);

#endif
