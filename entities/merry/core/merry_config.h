#ifndef _MERRY_CONFIG_
#define _MERRY_CONFIG_

#include <mvs_types.h>
#include <mvs_tools.h>

typedef struct MerryConfig MerryConfig;

struct MerryConfig {
  msize_t NOTHING; // literally nothing at the moment
};

_MVS_ATTR_ALWAYS_INLINE_ void merry_config_init_default(MerryConfig *config) {
  config->NOTHING = 0;
}

#endif
