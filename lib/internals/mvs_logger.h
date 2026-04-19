#ifndef _MVS_LOGGER_
#define _MVS_LOGGER_

#include <mvs_config.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdio.h>

typedef enum mLogLvl_t mLogLvl_t;
typedef struct MVSLogger MVSLogger;

enum mLogLvl_t {
  MVS_LOG_WARN,
  MVS_LOG_ERR,
  MVS_LOG_DBG,
};

_MVS_MFUNC_DEFINE_FUNC_PTR_(void, mlogFunc_t, mLogLvl_t, mstr_t, ...);

struct MVSLogger {
  mLogLvl_t log_upto;
  mlogFunc_t logger;
  mbool_t info_enabled;
};

_MVS_ATTR_EXTERNAL_ MVSLogger logger;

void mvs_init_logger(mlogFunc_t func, mLogLvl_t log_upto);

void mvs_logger_enable_info();

#define MVS_LOG(lvl, msg, ...)                                                 \
  do {                                                                         \
    if (logger.log_upto >= lvl)                                                \
      logger.logger(lvl, msg, __VA_ARGS__);                                    \
  } while (0)

#define MVS_NOTE(msg, ...)                                                     \
  do {                                                                         \
    if (logger.info_enabled)                                                   \
      logger.logger(MVS_LOG_WARN, msg, __VA_ARGS__);                                      \
  } while (0)

#define MVS_ERR(msg, ...) MVS_LOG(MVS_LOG_ERR, msg, __VA_ARGS__)
#define MVS_WARN(msg, ...) MVS_LOG(MVS_LOG_WARN, msg, __VA_ARGS__)
#define MVS_DBG(msg, ...) MVS_LOG(MVS_LOG_DBG, msg, __VA_ARGS__)

#endif
