#include <mvs_system_config.h>
#include <mvs_defaults.h>

void mvs_system_config_populate_default(MVSSystemConfig *conf) {
  conf->LOG_DEPTH = _MVS_DEFAULT_LOG_DEPTH_;
}
