#ifndef _MERRY_LOGGER_SUBSYSTEM_
#define _MERRY_LOGGER_SUBSYSTEM_

#include <mvs_logger.h>
#include <mvs_types.h>

_MVS_ATTR_EXTERNAL_ _MVS_ATTR_THREAD_LOCAL_ msize_t signature;

#define MNOTE(msg, ...) mvs_log_note("Core64[%zu]: "msg, signature, ##__VA_ARGS__)
#define MWARN(msg, ...) mvs_log_warn("Core64[%zu]: "msg, signature, ##__VA_ARGS__)
#define MERR(msg, ...) mvs_log_err("Core64[%zu]: "msg, signature, ##__VA_ARGS__)
#define MDBG(msg, ...) mvs_log_dbg("Core64[%zu]: "msg, signature, ##__VA_ARGS__)

void merry_set_signature(msize_t sig);

#endif
