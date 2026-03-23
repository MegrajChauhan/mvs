#include <mvs_logger.h>

MVSLogger logger;

_MVS_ATTR_ALWAYS_INLINE_ void mvs_init_logger(mlogFunc_t func,
                                              mLogLvl_t log_upto) {
  logger.logger = func;
  logger.log_upto = log_upto;
  logger.info_enabled = mfalse;
}

_MVS_ATTR_ALWAYS_INLINE_ void mvs_logger_enable_info() {
  logger.info_enabled = mtrue;
}
