#ifndef _MVS_SYSTEM_CONFIG_
#define _MVS_SYSTEM_CONFIG_

#include <mvs_types.h>

typedef struct MVSSystemConfig MVSSystemConfig;

struct MVSSystemConfig {
  msize_t MAX_EID;
  msize_t LOG_DEPTH;
};

void mvs_system_config_populate_default(MVSSystemConfig *conf);

#endif
