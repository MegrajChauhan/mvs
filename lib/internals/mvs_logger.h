#ifndef _MVS_LOGGER_
#define _MVS_LOGGER_

/*
 * For the logger subsystem, I have decided to use a buffer and a consumer.
 * The producers, the rest of MVS, will push messages for the consumer.
 * The consumer will keep a buffer of 1024 bytes in size. If the buffer is full,
 * the consumer will flush the buffer(or if no new message can be added). The
 * producer may request an immediate flush for important messages. If MVS
 * terminates, the logger will flush whatever remains in the buffer and
 * terminate.
 */

#include <mvs_config.h>
#include <mvs_threads.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <stdio.h>

typedef enum mLogLvl_t mLogLvl_t;
typedef struct MVSLogger MVSLogger;

enum mLogLvl_t {
  MVS_LOG_ERR,
  MVS_LOG_WARN,
  MVS_LOG_NOTE,
  MVS_LOG_DBG,
};

typedef void (*mlogFunc_t)(mLogLvl_t, mstr_t, ...);

struct MVSLogger {
  mLogLvl_t lvl;
  mlogFunc_t func;
};

_MVS_ATTR_EXTERNAL_ MVSLogger logger;

void mvs_init_logger(mlogFunc_t func, mLogLvl_t lvl);

#define MVS_LOG(lvl, msg, ...)                                                 \
  do {                                                                         \
    if (logger.lvl <= lvl)                                                     \
      logger.logger(lvl, msg, ##__VA_ARGS__);                                  \
  } while (0)

#define MVS_NOTE(msg, ...) MVS_LOG(MVS_LOG_NOTE, msg, __VA_ARGS__)
#define MVS_ERR(msg, ...) MVS_LOG(MVS_LOG_ERR, msg, __VA_ARGS__)
#define MVS_WARN(msg, ...) MVS_LOG(MVS_LOG_WARN, msg, __VA_ARGS__)
#define MVS_DBG(msg, ...) MVS_LOG(MVS_LOG_DBG, msg, __VA_ARGS__)

#endif
